#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>

#include "stm32f10x.h"

#include "gpio.h"
#include "scheduler.h"

#define MAX_HEAP_SIZE (0x400)

int main();

#endif