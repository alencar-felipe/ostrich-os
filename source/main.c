#include "main.h"

#define enable_interrupts() asm(" cpsie i ")
#define disable_interrupts() asm(" cpsid i ")

int main() {
    SystemInit(); //clock setup
    SysTick_Config(8000);

    gpio_setup(GPIOC, GPIO_OUT, 13);
    gpio_write(GPIOC, 13, 0);

    while(1);
}

void systick_handler() {
    gpio_write(GPIOC, 13, 1);
}


