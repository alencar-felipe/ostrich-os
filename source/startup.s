.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global	interrupt_vectors
.global	main

/* linker defined values ==================================================== */

.word _eram
.word _etext
.word _sdata
.word _edata
.word _sbss
.word _ebss

/* reset_handler ============================================================ */

.section .text.reset_handler
.weak reset_handler
.type reset_handler, %function

reset_handler:

//copy the data segment initializers from flash to SRAM
copy_data:
    ldr r0, =_etext         //src
    ldr r1, =_sdata         //dest
    ldr r2, =_edata         //end_dest
copy_data_loop:
    cmp r1, r2
    bcs copy_data_end       //break if dest >= end_edata
    
    ldr r3, [r0], #4        //r3 = *src++
    str r3, [r1], #4        //*dest++ = r3

    b copy_data_loop
copy_data_end:

//zero fill the bss segment.
zero_data:
    ldr r0, =_sbss          //i
    ldr r1, =_ebss          //end
    mvn r2, #0              //0

zero_data_loop:
    cmp r0, r1
    bcs zero_data_end       //break if i >= end

    str r2, [r0], #4        //*i++ = 0

    b zero_data_loop
zero_data_end:

//setup libc
bl __libc_init_array

//execute main function
bl	main

//halt
halt:
    b halt

.size reset_handler, .-reset_handler

/* interrupt vectors ======================================================== */

.section .interrupt_vectors,"a",%progbits
.type interrupt_vectors, %object
    
interrupt_vectors:
	.word _eram                      /* 0x000 Stack Pointer                   */
	.word reset_handler              /* 0x004 Reset                           */
	.word 0                          /* 0x008 Non maskable interrupt          */
	.word 0                          /* 0x00C HardFault                       */
	.word 0                          /* 0x010 Memory Management               */
	.word 0                          /* 0x014 BusFault                        */
	.word 0                          /* 0x018 UsageFault                      */
	.word 0                          /* 0x01C Reserved                        */
	.word 0                          /* 0x020 Reserved                        */
	.word 0                          /* 0x024 Reserved                        */
	.word 0                          /* 0x028 Reserved                        */
	.word 0                          /* 0x02C System service call             */
	.word 0                          /* 0x030 Debug Monitor                   */
	.word 0                          /* 0x034 Reserved                        */
	.word pendsv_handler             /* 0x038 PendSV                          */
	.word systick_handler            /* 0x03C System tick timer               */
	.word 0                          /* 0x040 Window watchdog                 */
	.word 0                          /* 0x044 PVD through EXTI Line detection */
	.word 0                          /* 0x048 Tamper                          */
	.word 0                          /* 0x04C RTC global                      */
	.word 0                          /* 0x050 FLASH global                    */
	.word 0                          /* 0x054 RCC global                      */
	.word 0                          /* 0x058 EXTI Line0                      */
	.word 0                          /* 0x05C EXTI Line1                      */
	.word 0                          /* 0x060 EXTI Line2                      */
	.word 0                          /* 0x064 EXTI Line3                      */
	.word 0                          /* 0x068 EXTI Line4                      */
	.word 0                          /* 0x06C DMA1_Ch1                        */
	.word 0                          /* 0x070 DMA1_Ch2                        */
	.word 0                          /* 0x074 DMA1_Ch3                        */
	.word 0                          /* 0x078 DMA1_Ch4                        */
	.word 0                          /* 0x07C DMA1_Ch5                        */
	.word 0                          /* 0x080 DMA1_Ch6                        */
	.word 0                          /* 0x084 DMA1_Ch7                        */
	.word 0                          /* 0x088 ADC1 and ADC2 global            */
	.word 0                          /* 0x08C CAN1_TX                         */
	.word 0                          /* 0x090 CAN1_RX0                        */
	.word 0                          /* 0x094 CAN1_RX1                        */
	.word 0                          /* 0x098 CAN1_SCE                        */
	.word 0                          /* 0x09C EXTI Lines 9:5                  */
	.word 0                          /* 0x0A0 TIM1 Break                      */
	.word 0                          /* 0x0A4 TIM1 Update                     */
	.word 0                          /* 0x0A8 TIM1 Trigger and Communication  */
	.word 0                          /* 0x0AC TIM1 Capture Compare            */
	.word 0                          /* 0x0B0 TIM2                            */
	.word 0                          /* 0x0B4 TIM3                            */
	.word 0                          /* 0x0B8 TIM4                            */
	.word 0                          /* 0x0BC I2C1 event                      */
	.word 0                          /* 0x0C0 I2C1 error                      */
	.word 0                          /* 0x0C4 I2C2 event                      */
	.word 0                          /* 0x0C8 I2C2 error                      */
	.word 0                          /* 0x0CC SPI1                            */
	.word 0                          /* 0x0D0 SPI2                            */
	.word 0                          /* 0x0D4 USART1                          */
	.word 0                          /* 0x0D8 USART2                          */
	.word 0                          /* 0x0DC USART3                          */
	.word 0                          /* 0x0E0 EXTI15_10                       */
	.word 0                          /* 0x0E4 RTCAlarm                        */
	.word 0                          /* 0x0E8 USBWakeup                       */
	.word 0                          /* 0x0EC TIM8_BRK                        */
	.word 0                          /* 0x0F0 TIM8_UP                         */
	.word 0                          /* 0x0F4 TIM8_TRG_COM                    */
	.word 0                          /* 0x0F8 TIM8_CC                         */
	.word 0                          /* 0x0FC ADC3                            */
	.word 0                          /* 0x100 FSMC                            */
	.word 0                          /* 0x104 SDIO                            */
	.word 0                          /* 0x108 TIM5                            */
	.word 0                          /* 0x10C SPI3                            */
	.word 0                          /* 0x110 USART4                          */
	.word 0                          /* 0x114 USART5                          */
	.word 0                          /* 0x118 TIM6                            */
	.word 0                          /* 0x11C TIM7                            */
	.word 0                          /* 0x120 DMA2_Ch1                        */
	.word 0                          /* 0x124 DMA2_Ch2                        */
	.word 0                          /* 0x128 DMA2_Ch3                        */
	.word 0                          /* 0x12C DMA2_Ch4                        */

.size interrupt_vectors, .-interrupt_vectors

/* weak aliases for interrupt vectors ======================================= */

.weak systick_handler
  .thumb_set systick_handler, reset_handler

.weak pendsv_handler
  .thumb_set pendsv_handler, reset_handler