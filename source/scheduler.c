#include "scheduler.h"

volatile uint32_t *scheduler_current_sp;
volatile uint32_t *scheduler_next_sp;

static scheduler_state_t state = SCHEDULER_DEFAULT;
static process_t processes[SCHEDULER_MAX];
static uint32_t current;
static uint64_t time;

static void task_finished(void)
{
	/* This function is called when some task handler returns. */
	volatile unsigned int i = 0;
	while (1) i++;
}

uint8_t scheduler_init()
{
	if (state != SCHEDULER_DEFAULT) return -1;

	processes[0].state = READY;
	for(uint32_t i = 1; i < SCHEDULER_MAX; i++) {
		processes[i].state = NONE;
	}
	
	current = 0;
	time = 0;

	state = SCHEDULER_INITED;

	return 0;
}

uint8_t scheduler_create(
	process_handler_t handler, 
	void *params, 
	uint32_t *stack,
	uint32_t stack_size)
{
	if (state != SCHEDULER_INITED) return -1;

	uint32_t i = 0;
	for(; i < SCHEDULER_MAX; i++) if(processes[i].state == NONE) break;
	if(i == SCHEDULER_MAX) return -1;

	if ((stack_size % sizeof(uint32_t)) != 0) return -1;

	uint32_t stack_offset = (stack_size/sizeof(uint32_t));

	// Initialize the task structure and set SP to the top of the stack
	// minus 16 words (64 bytes) to leave space for storing 16 registers:
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

	return 1;
}

uint8_t scheduler_start()
{
	NVIC_SetPriority(PendSV_IRQn, 0xff); // lowest possible priority
	NVIC_SetPriority(SysTick_IRQn, 0x00); // highest possible priority

	// start the SysTick timer (1ms period)
	uint32_t ret = SysTick_Config(72000);
	if (ret != 0) return -1;

	// start the first task
	scheduler_current_sp = &(processes[current].sp);
	scheduler_next_sp = scheduler_current_sp;
	state = SCHEDULER_STARTED;

	return 0;
}

uint64_t scheduler_time()
{
	return time;
}

void systick_handler()
{
	scheduler_current_sp = scheduler_next_sp; 

	while(1) {
		if(++current > SCHEDULER_MAX) current = 0;
		if(processes[current].state == READY) break;
	}

	scheduler_next_sp = &(processes[current].sp);

	time += 1;

	// trigger PendSV which performs the actual context switch
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}