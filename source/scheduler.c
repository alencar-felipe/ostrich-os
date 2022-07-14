#include "scheduler.h"

enum os_task_status {
	OS_TASK_STATUS_IDLE = 1,
	OS_TASK_STATUS_ACTIVE,
};

static enum state {
	STATE_DEFAULT = 1,
	STATE_INITIALIZED,
	STATE_TASKS_INITIALIZED,
	STATE_STARTED,
} state = STATE_DEFAULT;

static process_t processes[SCHEDULER_MAX];
static uint32_t current;

volatile uint32_t *os_curr_task;
volatile uint32_t *os_next_task;

static void task_finished(void)
{
	/* This function is called when some task handler returns. */

	volatile unsigned int i = 0;
	while (1)
		i++;
}

uint8_t scheduler_init(void)
{
	if (state != STATE_DEFAULT)
		return 0;

	for(uint32_t i = 0; i < SCHEDULER_MAX; i++) {
		processes[i].state = NONE;
	}

	processes[0].state = READY;
	current = 0;

	state = STATE_INITIALIZED;

	return 1;
}

uint8_t scheduler_create(
	process_handler_t handler, 
	void *params, 
	uint32_t *stack,
	uint32_t stack_size)
{
	if (state != STATE_INITIALIZED && state != STATE_TASKS_INITIALIZED) {
		return -1;
	}

	uint32_t i = 0;
	for(; i < SCHEDULER_MAX; i++) if(processes[i].state == NONE) break;
	if(i == SCHEDULER_MAX) return -1;

	if ((stack_size % sizeof(uint32_t)) != 0) return -1;

	uint32_t stack_offset = (stack_size/sizeof(uint32_t));

	// Initialize the task structure and set SP to the top of the stack
	//   minus 16 words (64 bytes) to leave space for storing 16 registers:

	processes[i].sp = (uint32_t) (stack+stack_offset-16);
	processes[i].state = READY;

	registers_t *registers = (registers_t *) (processes[i].sp);

	// Save init. values of registers which will be restored on exc. return:
	// - XPSR: Default value (0x01000000)
	// - PC: Point to the handler function (with LSB masked because the
	//     behavior is unpredictable if pc<0> == '1' on exc. return)
	// - LR: Point to a function to be called when the handler returns
	// - R0: Point to the handler function's parameter
	registers->xpsr = 0x01000000;
	registers->pc = (uint32_t) handler & ~0x01UL;
	registers->lr = (uint32_t) &task_finished;
	registers->r0 = (uint32_t) params;

	state = STATE_TASKS_INITIALIZED;

	return 1;
}

uint8_t scheduler_start(uint32_t systick_ticks)
{
	if (state != STATE_TASKS_INITIALIZED) return -1;

	NVIC_SetPriority(PendSV_IRQn, 0xff); // lowest possible priority
	NVIC_SetPriority(SysTick_IRQn, 0x00); // highest possible priority

	// start the SysTick timer
	uint32_t ret_val = SysTick_Config(systick_ticks);
	if (ret_val != 0) return -1;

	// start the first task
	os_next_task = &(processes[current].sp);
	os_curr_task = &(processes[current].sp);
	state = STATE_STARTED;

	//__set_MSP(os_curr_task->sp+64); // set PSP to the top of task's stack
	//__set_CONTROL(0x03); // switch to Unprivilleged Thread Mode with PSP
	//__ISB(); // execute ISB after changing CONTORL (recommended)

	while(1);
	//os_curr_task->handler(os_curr_task->params);

	return 0;
}

void systick_handler(void)
{
	os_curr_task = os_next_task; //&task_table.tasks[task_table.current_task].sp;
	//os_curr_task->status = OS_TASK_STATUS_IDLE;

	while(1) {
		if(++current > SCHEDULER_MAX) current = 0;
		if(processes[current].state == READY) break;
	}

	os_next_task = &(processes[current].sp);
	//os_next_task->status = OS_TASK_STATUS_ACTIVE;

	// trigger PendSV which performs the actual context switch
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
