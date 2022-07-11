#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32f10x.h"

typedef enum {
    NONE,
    READY,
    BLOCKED,
} scheduler_state_t;

typedef struct {
    void *stack;
    scheduler_state_t state;
} scheduler_process_t;

typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t psr;
} hw_stack_frame_t;

typedef struct {
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
} sw_stack_frame_t;

void scheduler_init();
uint8_t scheduler_new_process(void *(*p)(void*), void *arg, void *stack_addr,
    int stack_size);

#endif