#include "scheduler.h"

enum os_task_status {
	OS_TASK_STATUS_IDLE = 1,
	OS_TASK_STATUS_ACTIVE,
};

struct os_task {
	/* The stack pointer (sp) has to be the first element as it is located
	   at the same address as the structure itself (which makes it possible
	   to locate it safely from assembly implementation of PendSV_Handler).
	   The compiler might add padding between other structure elements. */
	volatile uint32_t sp;
	void (*handler)(void *params);
	void *params;
	volatile enum os_task_status status;
};

static enum state {
	STATE_DEFAULT = 1,
	STATE_INITIALIZED,
	STATE_TASKS_INITIALIZED,
	STATE_STARTED,
} state = STATE_DEFAULT;

static struct task_table {
	struct os_task tasks[SCHEDULER_MAX];
	volatile uint32_t current_task;
	uint32_t size;
} task_table;

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

	for(uint32_t i = 0; i < sizeof(task_table); i++) {
		((uint8_t *) &task_table)[i] = 0;
	}

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
		
	if (task_table.size >= SCHEDULER_MAX) return -1;
	if ((stack_size % sizeof(uint32_t)) != 0) return -1;

	uint32_t stack_offset = (stack_size/sizeof(uint32_t));

	// Initialize the task structure and set SP to the top of the stack
	//   minus 16 words (64 bytes) to leave space for storing 16 registers:
	struct os_task *task = &task_table.tasks[task_table.size];
	task->handler = handler;
	task->params = params;
	task->sp = (uint32_t)(stack+stack_offset-16);
	task->status = OS_TASK_STATUS_IDLE;

	registers_t *registers = (registers_t *) (task->sp);

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
	task_table.size++;

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
	os_next_task = &task_table.tasks[task_table.current_task].sp;
	os_curr_task = &task_table.tasks[task_table.current_task].sp;
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

	// select next task
	task_table.current_task++;
	if (task_table.current_task >= task_table.size)
		task_table.current_task = 0;

	os_next_task = &task_table.tasks[task_table.current_task].sp;
	//os_next_task->status = OS_TASK_STATUS_ACTIVE;

	// trigger PendSV which performs the actual context switch
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
