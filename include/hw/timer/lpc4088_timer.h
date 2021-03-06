#ifndef __LPC4088_TIMER_H__
#define __LPC4088_TIMER_H__

#include "hw/sysbus.h"
#include "qemu/timer.h"
#include "hw/misc/lpc4088_sc.h"
#include "hw/remotectrl/remotectrl.h"

#define TYPE_LPC4088_TIMER "lpc4088-timer"

#define LPC4088TIMER(obj) OBJECT_CHECK(LPC4088TimerState, (obj), TYPE_LPC4088_TIMER)
#define LPC4088TIMER_GET_CLASS(obj) OBJECT_GET_CLASS(LPC4088TimerClass, (obj), TYPE_LPC4088_TIMER)
#define LPC4088TIMER_CLASS(klass) OBJECT_CLASS_CHECK(LPC4088TimerClass, (klass), TYPE_LPC4088_TIMER)

#define LPC4088_TIMER_FREQUENCY (60*1000*1000)

#define LPC4088_TIMER_MEM_SIZE 0x080

#define LPC4088_TIMER_REG_IR 0x000
#define LPC4088_TIMER_REG_TCR 0x004
#define LPC4088_TIMER_REG_TC 0x008
#define LPC4088_TIMER_REG_PR 0x00C
#define LPC4088_TIMER_REG_PC 0x010
#define LPC4088_TIMER_REG_MCR 0x014
#define LPC4088_TIMER_REG_MR0 0x018
#define LPC4088_TIMER_REG_MR1 0x01C
#define LPC4088_TIMER_REG_MR2 0x020
#define LPC4088_TIMER_REG_MR3 0x024
#define LPC4088_TIMER_REG_CCR 0x028
#define LPC4088_TIMER_REG_CC0 0x02C
#define LPC4088_TIMER_REG_CC1 0x030
#define LPC4088_TIMER_REG_EMR 0x03C
#define LPC4088_TIMER_REG_CTCR 0x070

typedef struct LPC4088TimerState LPC4088TimerState;

typedef void (*TimerMatchEvent)(LPC4088TimerState *s);
typedef void (*TimerSimulateCaptureEvent)(LPC4088TimerState *s, uint8_t capture_pin, uint8_t rising_edge);

typedef struct LPC4088TimerClass {
	DeviceClass parent;

	TimerMatchEvent match_events[4];
	TimerSimulateCaptureEvent simulate_capture;
} LPC4088TimerClass;

struct LPC4088TimerState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion iomem;
    QEMUTimer *timer;
    qemu_irq irq;
	
	char *timer_name;
	bool check_syscon;
    bool enable_rc;
	bool enable_debug_print;
	
    uint64_t freq_hz;

	int64_t tc_last_checked_at;
	uint8_t next_match_interrupt;
	
	uint32_t timer_IR;
	uint32_t timer_TCR;
	uint32_t timer_TC;
	uint32_t timer_PR;
	uint32_t timer_PC;
	uint32_t timer_MCR;
	uint32_t timer_MR0;
	uint32_t timer_MR1;
	uint32_t timer_MR2;
	uint32_t timer_MR3;
	uint32_t timer_CCR;
	uint32_t timer_CC0;
	uint32_t timer_CC1;
	uint32_t timer_EMR;
	uint32_t timer_CTCR;
	
	RemoteCtrlState rcs;
	LPC4088SCState *syscon;
	
};


void lpc4088_timer_do_match_event(LPC4088TimerState *s, int n);
void lpc4088_timer_do_simulate_capture(LPC4088TimerState *s, uint8_t capture_pin, uint8_t rising_edge);


#endif
