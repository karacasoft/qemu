/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#ifndef __LPC4088FET208_H__
#define __LPC4088FET208_H__

#include "hw/arm/armv7m.h"
#include "hw/gpio/lpc4088_gpio.h"
#include "hw/timer/lpc4088_timer.h"
#include "hw/timer/lpc4088_pwm.h"
#include "hw/adc/lpc4088_adc.h"
#include "hw/adc/lpc4088_dac.h"
#include "hw/char/lpc4088_usart.h"
#include "hw/misc/lpc4088_sc.h"
#include "hw/misc/lpc4088_iocon.h"

#define TYPE_LPC4088FET208 "lpc4088fet208"
#define LPC4088FET208(obj) OBJECT_CHECK(LPC4088FET208State, (obj), TYPE_LPC4088FET208)

#define LPC4088_FLASH_BASE_ADDRESS 0x00000000
#define LPC4088FET208_FLASH_SIZE 0x00080000

#define LPC4088FET208_SRAM_BASE_ADDRESS 0x10000000
#define LPC4088FET208_SRAM_SIZE (16*1024)

#define LPC4088_NR_GPIO_PORTS 6
#define LPC4088_GPIO_BASE_ADDR 0x20098000

#define LPC4088_NR_SC_IRQ 4
#define LPC4088_NR_TIMERS 4
#define LPC4088_NR_PWMS 2
#define LPC4088_NR_ADCS 1
#define LPC4088_NR_DACS 1
#define LPC4088_NR_USARTS 5

typedef struct LPC4088FET208State {
    /*<private>*/
    SysBusDevice parent_obj;

    /*<public>*/
    char *cpu_type;
    ARMv7MState armv7m;

    LPC4088SCState syscon;
    LPC4088IOCONState iocon;

    LPC4088GPIOPortState gpio[LPC4088_NR_GPIO_PORTS];
	LPC4088TimerState timer[LPC4088_NR_TIMERS];
	LPC4088PWMState pwm[LPC4088_NR_PWMS];
	LPC4088ADCState adc[LPC4088_NR_ADCS];
	LPC4088DACState dac[LPC4088_NR_DACS];
	LPC4088USARTState usart[LPC4088_NR_USARTS];

    qemu_irq hardfault_input_irq;

} LPC4088FET208State;

#endif // __LPC4088FET208_H__