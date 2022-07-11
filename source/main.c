#include "main.h"

static char base_heap[MAX_HEAP_SIZE];
static char *heap = base_heap;

uint8_t stack[100];

void test() {
    gpio_write(GPIOC, 13, 1);
    while(1);
}

int main() {
    SystemInit(); //clock setup
    scheduler_init();

    gpio_setup(GPIOC, GPIO_OUT, 13);
    gpio_write(GPIOC, 13, 0);

    scheduler_new_process(test, 0, stack, 100);
   
    while(1);
}

caddr_t _sbrk(int incr) 
{
    char *prev_heap;
    prev_heap = heap;

    if ((heap + incr) > base_heap + MAX_HEAP_SIZE) {
        errno = ENOMEM;
        return (caddr_t) -1;
    }
  
    heap += incr;
    return (caddr_t) prev_heap;
}


