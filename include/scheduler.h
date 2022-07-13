#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "stm32f10x.h"

#include "stdint.h"

#define SCHEDULER_MAX (10)

typedef void (*process_handler_t)(void *);

typedef struct __attribute__((__packed__)) {
    uint32_t r8;  	// -16
	uint32_t r9;	// -15
	uint32_t r10;	// -14
	uint32_t r11;	// -13
	uint32_t r4;	// -12
	uint32_t r5;	// -11
	uint32_t r6;	// -10
	uint32_t r7;	// -9
	uint32_t r0;	// -8
	uint32_t r1;	// -7
	uint32_t r2;	// -6
	uint32_t r3;	// -5
	uint32_t r12;	// -4
	uint32_t lr;	// -3
	uint32_t pc;	// -2
	uint32_t xpsr;	// -1
} registers_t;

uint8_t scheduler_init();

uint8_t scheduler_create(
	process_handler_t handler, 
	void *params, 
	uint32_t *stack,
	uint32_t stack_size);

uint8_t scheduler_start(uint32_t systick_ticks);

#endif /* SCHEDULER_H */
