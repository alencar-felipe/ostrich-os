#include "stm32f10x.h"

#include "gpio.h"
#include "scheduler.h"

static void delay(volatile uint32_t delay_ms);
static void task_handler(void *params);

int main(void)
{
    SystemInit(); //clock setup

	gpio_setup(GPIOA, GPIO_OUT, 10);
	gpio_setup(GPIOA, GPIO_OUT, 11);
	gpio_setup(GPIOA, GPIO_OUT, 12);

    gpio_write(GPIOA, 10, 1);
	gpio_write(GPIOA, 11, 1);
	gpio_write(GPIOA, 12, 1);

	// allocate memory for the stacks
	static uint32_t stack1[128];
	static uint32_t stack2[128];
	static uint32_t stack3[128];

	// setup task parameters
	uint32_t p1[] = {80000, 10};
	uint32_t p2[] = {800000, 11};
	uint32_t p3[] = {700000, 12};

	scheduler_init();

	scheduler_create(&task_handler, (void*)p1, stack1, sizeof(stack1));
	scheduler_create(&task_handler, (void*)p2, stack2, sizeof(stack2));
	scheduler_create(&task_handler, (void*)p3, stack3, sizeof(stack3));

	// context switch with 1ms quantum
	scheduler_start();

	while(1);
}

static void delay(volatile uint32_t time)
{
	while (time > 0)
		time--;
}

static void task_handler(void *params)
{
	uint32_t *data = (uint32_t*) params;

	while (1) {
		__disable_irq();
		gpio_write(GPIOA, data[1], 1);
		__enable_irq();

		delay(data[0]);
        __disable_irq();
		gpio_write(GPIOA, data[1], 0);
		__enable_irq();
        delay(data[0]);
	}
}