/**
 * @file  startup.s
 * @brief Processor vector table and interrupts handlers (including reset)
 *
 * @author Saint-Genest Gwenael <gwen@cowlab.fr>
 * @copyright Cowlab (c) 2017
 */

    .syntax unified
    .arch armv6-m

/* -- Stack and Head sections ---------------------------------------------- */
    .section .stack
    .align 3
    .equ    Stack_Size, 0x2000
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

/* -- Vector Table --------------------------------------------------------- */

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    /* Cortex M0 Handlers */
    .long   __StackTop                  /* Top of Stack                     */
    .long   Reset_Handler               /* Reset Handler                    */
    .long   NMI_Handler                 /* NMI Handler                      */
    .long   HardFault_Handler           /* Hard Fault Handler               */
    .long   0                           /* Reserved                         */
    .long   0                           /* Reserved                         */
    .long   0                           /* Reserved                         */
    .long   0                           /* Reserved                         */
    .long   0                           /* Reserved                         */
    .long   0                           /* Reserved                         */
    .long   0                           /* Reserved                         */
    .long   SVC_Handler                 /* SVCall Handler                   */
    .long   0                           /* Debug Monitor Handler            */
    .long   0                           /* Reserved                         */
    .long   PendSV_Handler              /* PendSV Handler                   */
    .long   SysTick_Handler             /* SysTick Handler                  */
    /* SAMD21 Peripherals Handlers */
    .long   PM_Handler                  /* Power Manager Handler            */
    .long   SYSCTRL_Handler             /* System Control Handler           */
    .long   WDT_Handler                 /* Watchdog Timer Handler           */
    .long   RTC_Handler                 /* Real-Time Counter Handler        */
    .long   EIC_Handler                 /* External Interrupt Controller    */
    .long   NVMCTRL_Handler             /* Non-Volatile Memory Controller   */
    .long   DMAC_Handler                /* Direct Memory Access Controller  */
    .long   USB_Handler                 /* Universal Serial Bus             */
    .long   EVSYS_Handler               /* Event System Interface           */
    .long   SERCOM0_Handler             /* Serial Communication Interface 0 */
    .long   SERCOM1_Handler             /* Serial Communication Interface 1 */
    .long   SERCOM2_Handler	        /* Serial Communication Interface 2 */
    .long   SERCOM3_Handler             /* Serial Communication Interface 3 */ 
    .long   SERCOM4_Handler             /* Serial Communication Interface 4 */
    .long   SERCOM5_Handler             /* Serial Communication Interface 5 */
    .long   TCC0_Handler                /* Timer Counter Control 0          */
    .long   TCC1_Handler                /* Timer Counter Control 1          */
    .long   TCC2_Handler                /* Timer Counter Control 2          */
    .long   TC3_Handler                 /* Basic Timer Counter 3            */
    .long   TC4_Handler                 /* Basic Timer Counter 4            */
    .long   TC5_Handler                 /* Basic Timer Counter 5            */
    .long   0                           /* Reserved                         */
    .long   0	                        /* Reserved                         */
    .long   ADC_Handler                 /* Analog Digital Converter         */
    .long   AC_Handler                  /* Analog Comparator                */
    .long   DAC_Handler                 /* Digital Analog Converter         */
    .long   PTC_Handler                 /* Peripheral Touch Controller      */
    .long   I2S_Handler                 /* Inter-IC Sound Interface         */
    .long   0                           /* Reserved                         */
    .size    __isr_vector, . - __isr_vector

/**
 *
 */
    .text
    .thumb
    .thumb_func
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    /* Copy datas from flash to SRAM */
    ldr    r1, =_etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__
    subs    r3, r2
    ble    .copy_end
.copy_loop:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .copy_loop
.copy_end:
    /* Call C code entry ("main" function) */
    bl  main

/**
 * @brief Default handler is an infinite loop for all unsupported events
 *
 */
.section	.text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
	b	Infinite_Loop
	.size	Default_Handler, .-Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_default_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .thumb_set \handler_name,Default_Handler
    .endm

    /* Default handlers for Cortex M0 internal blocks */
    def_default_handler    NMI_Handler
    def_default_handler    HardFault_Handler
    def_default_handler    SVC_Handler
    def_default_handler    PendSV_Handler
    def_default_handler    SysTick_Handler
    /* Default handlers for SAMD21 peripherals */
    def_default_handler    PM_Handler
    def_default_handler    SYSCTRL_Handler
    def_default_handler    WDT_Handler
    def_default_handler    RTC_Handler
    def_default_handler    EIC_Handler
    def_default_handler    NVMCTRL_Handler
    def_default_handler    DMAC_Handler
    def_default_handler    USB_Handler
    def_default_handler    EVSYS_Handler
    def_default_handler    SERCOM0_Handler
    def_default_handler    SERCOM1_Handler
    def_default_handler    SERCOM2_Handler
    def_default_handler    SERCOM3_Handler
    def_default_handler    SERCOM4_Handler
    def_default_handler    SERCOM5_Handler
    def_default_handler    TCC0_Handler
    def_default_handler    TCC1_Handler
    def_default_handler    TCC2_Handler
    def_default_handler    TC3_Handler
    def_default_handler    TC4_Handler
    def_default_handler    TC5_Handler
    def_default_handler    ADC_Handler
    def_default_handler    AC_Handler
    def_default_handler    DAC_Handler
    def_default_handler    PTC_Handler
    def_default_handler    I2S_Handler

    .end
