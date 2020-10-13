#ifndef __LPC4088_PWM_H__
#define __LPC4088_PWM_H__

#include "hw/sysbus.h"
#include "qemu/timer.h"
#include "hw/timer/lpc4088_timer.h"
#include "hw/remotectrl/remotectrl.h"

#define TYPE_LPC4088_PWM "lpc4088-pwm"

#define LPC4088PWM(obj) OBJECT_CHECK(LPC4088PWMState, (obj), TYPE_LPC4088_PWM)
#define LPC4088PWM_CLASS(klass) OBJECT_CLASS_CHECK(LPC4088PWMClass, (klass), TYPE_LPC4088_PWM)
#define LPC4088PWM_GET_CLASS(obj) OBJECT_GET_CLASS(LPC4088PWMClass, (obj), TYPE_LPC4088_PWM)

#define LPC4088_PWM_TIMER_FREQUENCY (60 * 1000 * 1000)

#define LPC4088_PWM_MEM_SIZE 0x080

#define LPC4088_PWM_REG_IR 0x000
#define LPC4088_PWM_REG_TCR 0x004
#define LPC4088_PWM_REG_TC 0x008
#define LPC4088_PWM_REG_PR 0x00C
#define LPC4088_PWM_REG_PC 0x010
#define LPC4088_PWM_REG_MCR 0x014
#define LPC4088_PWM_REG_MR0 0x018
#define LPC4088_PWM_REG_MR1 0x01C
#define LPC4088_PWM_REG_MR2 0x020
#define LPC4088_PWM_REG_MR3 0x024
#define LPC4088_PWM_REG_CCR 0x028
#define LPC4088_PWM_REG_CC0 0x02C
#define LPC4088_PWM_REG_CC1 0x030
#define LPC4088_PWM_REG_CC2 0x034
#define LPC4088_PWM_REG_CC3 0x038
#define LPC4088_PWM_REG_MR4 0x040
#define LPC4088_PWM_REG_MR5 0x044
#define LPC4088_PWM_REG_MR6 0x048
#define LPC4088_PWM_REG_PCR 0x04C
#define LPC4088_PWM_REG_LER 0x050
#define LPC4088_PWM_REG_CTCR 0x070

typedef struct LPC4088PWMClass {
	LPC4088TimerClass parent;

	TimerMatchEvent parent_match_events[4];

	DeviceRealize parent_realize;
	DeviceReset parent_reset;
} LPC4088PWMClass;

typedef struct LPC4088PWMState {
    /* <private> */
    LPC4088TimerState parent_obj;

    /* <public> */
	const MemoryRegionOps *parent_mem_ops;
    MemoryRegion iomem;
	bool enable_rc;
	
	char *pwm_name;
	uint64_t freq_hz;
	
	uint32_t pwm_MR4;
	uint32_t pwm_MR5;
	uint32_t pwm_MR6;
	uint32_t pwm_PCR;
	uint32_t pwm_LER;

	// ## IMPORTANT
	// The name is confusing but these registers are 
	// **NOT** used as shadow registers. In fact, the other
	// registers are used as shadow registers in PWM mode.
	uint32_t pwm_shadow_mr[7];
	
} LPC4088PWMState;


#endif
