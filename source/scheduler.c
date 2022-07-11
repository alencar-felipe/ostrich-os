#include "scheduler.h"

#define enable_interrupts() asm(" cpsie i ")
#define disable_interrupts() asm(" cpsid i ")

#define PROCESSES_LEN (10)
#define MSP_RETURN (0xFFFFFFF9) 
#define PSP_RETURN (0xFFFFFFFD)

static scheduler_process_t processes[PROCESSES_LEN];
static uint32_t current = 0;
static uint32_t *stack;

static void context_switcher(); 
static inline void push_context();
static inline void pop_context();
static inline void get_stack_pointer();
static inline void set_stack_pointer();

void scheduler_init()
{
    disable_interrupts();

    SysTick_Config(8000); //setup system tick timer  
    
    for(uint32_t i = 0; i < PROCESSES_LEN; i++) {
        processes[i].state = NONE;
    }

    processes[0].state = READY;

    enable_interrupts();
}

uint8_t scheduler_new_process(void *(*p)(void*), void *arg, void *stack_addr,
    int stack_size)
{
    disable_interrupts();

    uint32_t i;
    hw_stack_frame_t * process_frame;

    for(i = 1; i < PROCESSES_LEN; i++){
        if(processes[i].state == NONE) break;
    }

    //error
    if(i == PROCESSES_LEN) return 1;

    process_frame = (hw_stack_frame_t *)(stack_addr - sizeof(hw_stack_frame_t));
    process_frame->r0 = (uint32_t)arg;
    process_frame->r1 = 0;
    process_frame->r2 = 0;
    process_frame->r3 = 0;
    process_frame->r12 = 0;
    process_frame->pc = ((uint32_t) p);
    process_frame->lr = 0; //(uint32_t) del_process;
    process_frame->psr = 0x21000000; //default PSR value
    
    processes[i].state = READY;
    processes[i].stack = stack_addr + stack_size - sizeof(hw_stack_frame_t) -
        sizeof(sw_stack_frame_t);

    enable_interrupts();

    return i;
}

void systick_handler()
{
    push_context();  
    disable_interrupts();

    get_stack_pointer();
    context_switcher();
    set_stack_pointer();

     asm volatile("mvn lr, #0xFFFFFFF9");
     
    enable_interrupts();
    pop_context();
    
}

void context_switcher()
{
    //processes[current].stack = stack;

    while(1) {
        current+=1;
        //if(++current >= PROCESSES_LEN) current = 0;
        break;
        
        // if (processes[current].state == READY || 1) {
        //   //stack = processes[current].stack;
        //   //*stack = PSP_RETURN;
        //   break;
        // }
    }
}

void push_context()
{
  uint32_t temp;

  asm volatile(
    "mrs %0, msp                  \n\t"
    "stmdb %0!, {r4-r11}          \n\t"
    "msr msp, %0                  \n\t"
    : "=r" (temp));

}

void pop_context()
{
  uint32_t temp;

  asm volatile(
    "mrs %0, msp                  \n\t"
    "ldmfd %0!, {r4-r11}          \n\t"
    "msr msp, %0                  \n\t"
    : "=r" (temp));
}

void get_stack_pointer()
{
  asm volatile(
    "mrs %0, msp                  \n\t"
    : "=r" (stack));
}

void set_stack_pointer()
{
  asm volatile(
    "msr msp, %0                  \n\t"
    : "=r" (stack));
}