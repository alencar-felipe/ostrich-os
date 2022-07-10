#include "main.h"

int main() {
    SystemInit(); //clock setup
    //init_systick();

    gpio_setup(GPIOC, GPIO_OUT, 13);

    gpio_write(GPIOC, 13, LOW);
}

// void init_systick()
// {
// 	// Main clock source is running with HSI by default which is at 8 Mhz.
// 	// SysTick clock source can be set with CTRL register (Bit 2)
// 	// 0: AHB/8 -> (1 MHz)
// 	// 1: Processor clock (AHB) -> (8 MHz)
// 	SYSTICK->CSR |= 0x00000; // Currently set to run at 1 Mhz
// 	// Enable callback
// 	SYSTICK->CSR |= (1 << 1);
// 	// Load the reload value
// 	SYSTICK->RVR = 1000; // 1 millisecond timer (1MHz / 1000)
// 	// Set the current value to 0
// 	SYSTICK->CVR = 0;
// 	// Enable SysTick
// 	SYSTICK->CSR |= (1 << 0);
// }