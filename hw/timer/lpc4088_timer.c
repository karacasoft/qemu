#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/timer/lpc4088_timer.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "qemu/main-loop.h"

#ifndef LPC4088_TIMER_ERROR_DEBUG
#define LPC4088_TIMER_ERROR_DEBUG 1
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_TIMER_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_TIMER,__func__, ##args);}

#define REMOTE_CTRL_TIMER_MAGIC 0xDEEDBEE3
#define REMOTE_CTRL_CMD_TIMER_CAPTURE   0x11000100

static void lpc4088_timer_set_alarm(LPC4088TimerState *s);

static const char *lpc4088_timer_register_name(uint32_t offset) {
    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        return "Timer_IR";
    case LPC4088_TIMER_REG_TCR:
        return "Timer_TCR";
    case LPC4088_TIMER_REG_TC:
        return "Timer_TC";
    case LPC4088_TIMER_REG_PR:
       return "Timer_PR";
    case LPC4088_TIMER_REG_PC:
        return "Timer_PC";
    case LPC4088_TIMER_REG_MCR:
        return "Timer_MCR";
    case LPC4088_TIMER_REG_MR0:
        return "Timer_MR0";
    case LPC4088_TIMER_REG_MR1:
       return "Timer_MR1";
    case LPC4088_TIMER_REG_MR2:
        return "Timer_MR2";
    case LPC4088_TIMER_REG_MR3:
        return "Timer_MR3";
    case LPC4088_TIMER_REG_CCR:
        return "Timer_CCR";
    case LPC4088_TIMER_REG_CC0:
       return "Timer_CC0";
    case LPC4088_TIMER_REG_CC1:
        return "Timer_CC1";
    case LPC4088_TIMER_REG_EMR:
        return "Timer_EMR";
    case LPC4088_TIMER_REG_CTCR:
        return "Timer_CTCR";
    default:
        return "Timer_[?]";
    }
}

static void lpc4088_timer_update_tc(LPC4088TimerState *s)
{
    int64_t curr_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    if(s->timer_TCR == 0x1 && (s->timer_CTCR & 0x3) == 0) {
        uint64_t cycles_per_us = s->freq_hz / 1000000;
        
        int64_t time_diff = curr_time - s->tc_last_checked_at;
        // Divide by 1000 because the time_diff is in nanoseconds
        uint64_t passed_cycles = (time_diff * cycles_per_us) / 1000;
        uint64_t after_prescale = passed_cycles / (s->timer_PR + 1);

        s->timer_TC += after_prescale;
    }
    if(s->timer_TCR & 0x2) {
        s->timer_TC = 0;
    }

    s->tc_last_checked_at = curr_time;
}

static void lpc4088_timer_interrupt(void *opaque) {
    LPC4088TimerState *s = opaque;
    int64_t cycles_per_us = s->freq_hz / 1000000;

    lpc4088_timer_update_tc(s);

    bool interrupt_requested = false;
    bool reset_requested = false;
    bool stop_requested = false;
    
    int64_t mr0_time_left;
    int64_t mr1_time_left;
    int64_t mr2_time_left;
    int64_t mr3_time_left;
    if(s->timer_MCR & (7 << 0)) {
        // check if we have an MR0 event here by checking the remaining time
        int64_t mr0_tc_left = (int64_t) s->timer_MR0 - (int64_t) s->timer_TC;
        mr0_time_left = (mr0_tc_left / cycles_per_us) / (s->timer_PR + 1);

        // There is some inconsistency between time measurements so we
        // use a threshold to determine if we have hit the MATCH register
        if(mr0_time_left > -400 && mr0_time_left < 0) {
            
            if(s->timer_MCR & (1 << 0) && !(s->timer_IR & (1 << 0))) {
                s->timer_IR |= (1 << 0);
                interrupt_requested = true;
            }
            if(s->timer_MCR & (1 << 1)) {
                reset_requested = true;
            }
            if(s->timer_MCR & (1 << 2)) {
                stop_requested = true;
            }

            // set external match register bit in the event of MR0
            uint32_t emr_mr0 = (s->timer_EMR >> 4) & 3;
            if(emr_mr0 == 3) {
                if(s->timer_EMR & 1) { s->timer_EMR &= ~1; }
                else { s->timer_EMR |= 1; }
            } else if(emr_mr0 == 2) {
                s->timer_EMR |= 1;
            } else if(emr_mr0 == 1) {
                s->timer_EMR &= ~1;
            }
        }

        
    }

    if(s->timer_MCR & (7 << 3)) {
        // check if we have an MR1 event here by checking the remaining time
        int64_t mr1_tc_left = (int64_t) s->timer_MR1 - (int64_t) s->timer_TC;
        mr1_time_left = (mr1_tc_left / cycles_per_us) / (s->timer_PR + 1);

        // TODO we probably could have used an array to simulate
        // match registers and iterate over them using a loop
        if(mr1_time_left > -400 && mr1_time_left < 0) {
            if(s->timer_MCR & (1 << 3) && !(s->timer_IR & (1 << 1))) {
                s->timer_IR |= (1 << 1);
                interrupt_requested = true;
            }
            if(s->timer_MCR & (1 << 4)) {
                reset_requested = true;
            }
            if(s->timer_MCR & (1 << 5)) {
                stop_requested = true;
            }

            // set external match register bit in the event of MR1
            uint32_t emr_mr1 = (s->timer_EMR >> 6) & 3;
            if(emr_mr1 == 3) {
                if(s->timer_EMR & (1 << 1)) { s->timer_EMR &= ~(1 << 1); }
                else { s->timer_EMR |= (1 << 1); }
            } else if(emr_mr1 == 2) {
                s->timer_EMR |= (1 << 1);
            } else if(emr_mr1 == 1) {
                s->timer_EMR &= ~(1 << 1);
            }
        }
    }

    if(s->timer_MCR & (7 << 6)) {
        // check if we have an MR2 event here by checking the remaining time
        int64_t mr2_tc_left = (int64_t) s->timer_MR2 - (int64_t) s->timer_TC;
        mr2_time_left = (mr2_tc_left / cycles_per_us) / (s->timer_PR + 1);

        if(mr2_time_left > -400 && mr2_time_left < 0) {
            if(s->timer_MCR & (1 << 6) && !(s->timer_IR & (1 << 2))) {
                s->timer_IR |= (1 << 2);
                interrupt_requested = true;
            }
            if(s->timer_MCR & (1 << 7)) {
                reset_requested = true;
            }
            if(s->timer_MCR & (1 << 8)) {
                stop_requested = true;
            }

            // set external match register bit in the event of MR2
            uint32_t emr_mr2 = (s->timer_EMR >> 8) & 3;
            if(emr_mr2 == 3) {
                if(s->timer_EMR & (1 << 2)) { s->timer_EMR &= ~(1 << 2); }
                else { s->timer_EMR |= (1 << 2); }
            } else if(emr_mr2 == 2) {
                s->timer_EMR |= (1 << 2);
            } else if(emr_mr2 == 1) {
                s->timer_EMR &= ~(1 << 2);
            }
        }
    }

    if(s->timer_MCR & (7 << 9)) {
        // check if we have an MR2 event here by checking the remaining time
        int64_t mr3_tc_left = (int64_t) s->timer_MR3 - (int64_t) s->timer_TC;
        mr3_time_left = (mr3_tc_left / cycles_per_us) / (s->timer_PR + 1);

        if(mr3_time_left > -400 && mr3_time_left < 0) {
            if(s->timer_MCR & (1 << 9) && !(s->timer_IR & (1 << 3))) {
                s->timer_IR |= (1 << 3);
                interrupt_requested = true;
            }
            if(s->timer_MCR & (1 << 10)) {
                reset_requested = true;
            }
            if(s->timer_MCR & (1 << 11)) {
                stop_requested = true;
            }

            // set external match register bit in the event of MR3
            uint32_t emr_mr3 = (s->timer_EMR >> 10) & 3;
            if(emr_mr3 == 3) {
                if(s->timer_EMR & (1 << 3)) { s->timer_EMR &= ~(1 << 3); }
                else { s->timer_EMR |= (1 << 3); }
            } else if(emr_mr3 == 2) {
                s->timer_EMR |= (1 << 3);
            } else if(emr_mr3 == 1) {
                s->timer_EMR &= ~(1 << 3);
            }
        }
    }
    
    if(interrupt_requested) {
        qemu_irq_pulse(s->irq);
        DEBUG_PRINT("Timer irq sent\n");
    }

    if(reset_requested) {
        s->timer_TC = 0;
        s->tc_last_checked_at = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
        DEBUG_PRINT("Timer is reset\n");
    }

    if(stop_requested) {
        s->timer_TCR = 0;
        s->tc_last_checked_at = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    }
    
    DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","Timer_Interrupt", (uint32_t) s->timer_IR);
    
    lpc4088_timer_set_alarm(s);
}

static inline int64_t lpc4088_ns_to_ticks(LPC4088TimerState *s, int64_t t) {
    return muldiv64(t, s->freq_hz, LPC4088_TIMER_FREQUENCY) / (s->timer_PR + 1);
}

static void lpc4088_timer_set_alarm(LPC4088TimerState *s) {
    if((s->timer_TCR & 0x1) && !(s->timer_TCR & 0x2)) {
        int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
        int64_t cycles_per_us = s->freq_hz / 1000000;

        int64_t mr0_time_left;
        int64_t mr1_time_left;
        int64_t mr2_time_left;
        int64_t mr3_time_left;
        if(s->timer_MCR & (1 << 0) || s->timer_MCR & (1 << 1) || s->timer_MCR & (1 << 2)) {
            int64_t mr0_tc_left = (int64_t) s->timer_MR0 - (int64_t) s->timer_TC;
            mr0_time_left = (mr0_tc_left / cycles_per_us) / (s->timer_PR + 1);
        } else {
            mr0_time_left = 0x7FFFFFFFFFFFFFFF;
        }
        

        if(s->timer_MCR & (1 << 3) || s->timer_MCR & (1 << 4) || s->timer_MCR & (1 << 5)) {
            int64_t mr1_tc_left = (int64_t) s->timer_MR1 - (int64_t) s->timer_TC;
            mr1_time_left = (mr1_tc_left / cycles_per_us) / (s->timer_PR + 1);
        } else {
            mr1_time_left = 0x7FFFFFFFFFFFFFFF;
        }

        if(s->timer_MCR & (1 << 6) || s->timer_MCR & (1 << 7) || s->timer_MCR & (1 << 8)) {
            int64_t mr2_tc_left = (int64_t) s->timer_MR2 - (int64_t) s->timer_TC;
            mr2_time_left = (mr2_tc_left / cycles_per_us) / (s->timer_PR + 1);
        } else {
            mr2_time_left = 0x7FFFFFFFFFFFFFFF;
        }

        if(s->timer_MCR & (1 << 9) || s->timer_MCR & (1 << 10) || s->timer_MCR & (1 << 11)) {
            int64_t mr3_tc_left = (int64_t) s->timer_MR3 - (int64_t) s->timer_TC;
            mr3_time_left = (mr3_tc_left / cycles_per_us) / (s->timer_PR + 1);
        } else {
            mr3_time_left = 0x7FFFFFFFFFFFFFFF;
        }
        
        int64_t min_time_left = MAX(MIN(MIN(MIN(mr0_time_left, mr1_time_left), mr2_time_left), mr3_time_left), 0);
        int64_t min_time_left_ns = min_time_left * 1000;

        DEBUG_PRINT("Min time left: %ld\n", min_time_left);

        if(min_time_left_ns != 0) {
            if(timer_pending(s->timer)) {
                timer_del(s->timer);
            }
            timer_mod(s->timer, now + min_time_left_ns);
        }
    }
}

static void lpc4088_timer_reset(DeviceState *dev) {
    LPC4088TimerState *s = LPC4088TIMER(dev);
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    s->tc_last_checked_at = now;

    s->timer_IR = 0;
    s->timer_TCR = 0;
    s->timer_TC = 0;
    s->timer_PR = 0;
    s->timer_PC = 0;
    s->timer_MCR = 0;
    s->timer_MR0 = 0;
    s->timer_MR1 = 0;
    s->timer_MR2 = 0;
    s->timer_MR3 = 0;
    s->timer_CCR = 0;
    s->timer_CC0 = 0;
    s->timer_CC1 = 0;
    s->timer_EMR = 0;
    s->timer_CTCR = 0;
    
}

static uint64_t lpc4088_timer_read(void *opaque, hwaddr offset, unsigned size) {
    LPC4088TimerState *s = opaque;

    if(
        ((s->timer_name[0] == '0') && !(s->syscon->sc_PCONP & (1 << 1))) ||
        ((s->timer_name[0] == '1') && !(s->syscon->sc_PCONP & (1 << 2))) ||
        ((s->timer_name[0] == '2') && !(s->syscon->sc_PCONP & (1 << 22))) ||
        ((s->timer_name[0] == '3') && !(s->syscon->sc_PCONP & (1 << 23)))
    ) {
        qemu_irq_pulse(s->syscon->hard_fault_irq);
    }

    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        return s->timer_IR;
    case LPC4088_TIMER_REG_TCR:
        return s->timer_TCR;
    case LPC4088_TIMER_REG_TC:
        if(s->timer_TCR & 0x2) {
            return 0;
        } else if(s->timer_TCR & 0x1) {
            lpc4088_timer_update_tc(s);
        }
        s->tc_last_checked_at = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
        DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_timer_register_name(offset), (uint32_t) s->timer_TC);
        return s->timer_TC;
    case LPC4088_TIMER_REG_PR:
        return s->timer_PR;
    case LPC4088_TIMER_REG_PC:
        DEBUG_PRINT("PC is not emulated correctly. Reading PC may present inconsistent results.\n");
        return s->timer_PC;
    case LPC4088_TIMER_REG_MCR:
        return s->timer_MCR;
    case LPC4088_TIMER_REG_MR0:
        return s->timer_MR0;
    case LPC4088_TIMER_REG_MR1:
        return s->timer_MR1;
    case LPC4088_TIMER_REG_MR2:
        return s->timer_MR2;
    case LPC4088_TIMER_REG_MR3:
        return s->timer_MR3;
    case LPC4088_TIMER_REG_CCR:
        return s->timer_CCR;
    case LPC4088_TIMER_REG_CC0:
        return s->timer_CC0;
    case LPC4088_TIMER_REG_CC1:
        return s->timer_CC1;
    case LPC4088_TIMER_REG_EMR:
        return s->timer_EMR;
    case LPC4088_TIMER_REG_CTCR:
        return s->timer_CTCR;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
    }

    return 0;
}

static void lpc4088_timer_write(void *opaque, hwaddr offset, uint64_t val64, unsigned size) {
    LPC4088TimerState *s = opaque;
    uint32_t value = val64;

    if(
        ((s->timer_name[0] == '0') && !(s->syscon->sc_PCONP & (1 << 1))) ||
        ((s->timer_name[0] == '1') && !(s->syscon->sc_PCONP & (1 << 2))) ||
        ((s->timer_name[0] == '2') && !(s->syscon->sc_PCONP & (1 << 22))) ||
        ((s->timer_name[0] == '3') && !(s->syscon->sc_PCONP & (1 << 23)))
    ) {
        qemu_irq_pulse(s->syscon->hard_fault_irq);
    }

    
    DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_timer_register_name(offset), (uint32_t) value);

    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        s->timer_IR &= ~value;
        return;
    case LPC4088_TIMER_REG_TCR:
        s->timer_TCR = value;
        lpc4088_timer_update_tc(s);
        lpc4088_timer_set_alarm(s);	
        return;
    case LPC4088_TIMER_REG_TC:
        lpc4088_timer_update_tc(s);
        s->timer_TC = value;
        return;
    case LPC4088_TIMER_REG_PR:
        s->timer_PR = value;
        return;
    case LPC4088_TIMER_REG_PC:
        DEBUG_PRINT("PC is not emulated correctly. Writing to PC may present inconsistent results.\n");
        s->timer_PC = value;
        return;
    case LPC4088_TIMER_REG_MCR:
        s->timer_MCR = value;
        lpc4088_timer_update_tc(s);
        lpc4088_timer_set_alarm(s);
        return;
    case LPC4088_TIMER_REG_MR0:
        s->timer_MR0 = value;
        lpc4088_timer_update_tc(s);
        lpc4088_timer_set_alarm(s);
        return;
    case LPC4088_TIMER_REG_MR1:
        s->timer_MR1 = value;
        lpc4088_timer_update_tc(s);
        lpc4088_timer_set_alarm(s);
        return;
    case LPC4088_TIMER_REG_MR2:
        s->timer_MR2 = value;
        lpc4088_timer_update_tc(s);
        lpc4088_timer_set_alarm(s);
        return;
    case LPC4088_TIMER_REG_MR3:
        s->timer_MR3 = value;
        lpc4088_timer_update_tc(s);
        lpc4088_timer_set_alarm(s);
        return;
    case LPC4088_TIMER_REG_CCR:
        s->timer_CCR = value;
        return;
    case LPC4088_TIMER_REG_CC0:
        // TODO hard fault??
        return;
    case LPC4088_TIMER_REG_CC1:
        // TODO hard fault??
        return;
    case LPC4088_TIMER_REG_EMR:
        s->timer_EMR = value;
        return;
    case LPC4088_TIMER_REG_CTCR:
        s->timer_CTCR = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
        return;
    }
}

static void lpc4088_timer_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg) {
    LPC4088TimerState *s = LPC4088TIMER(rcs->connected_device);
    
    if(s->enable_rc) {
        if(msg->arg1 == s->timer_name[0] && msg->arg2 < 2) {
            if(msg->cmd == REMOTE_CTRL_CMD_TIMER_CAPTURE) {
                lpc4088_timer_update_tc(s);
                if(msg->arg2 == 0) {
                    // copy the TC value into CC0 in the case of a rising edge
                    if((s->timer_CCR & (1 << 0)) && msg->arg3 == 1) {
                        s->timer_CC0 = s->timer_TC;
                    }
                    // copy the TC value into CC0 in the case of a falling edge
                    if((s->timer_CCR & (1 << 1)) && msg->arg3 == 2) {
                        s->timer_CC0 = s->timer_TC;
                    }
                    // trigger an interrupt if corresponding flag is set
                    if(s->timer_CCR & (1 << 2) && !(s->timer_IR & (1 << 4))) {
                        s->timer_IR |= (1 << 4);
                        qemu_mutex_lock_iothread();
                        qemu_irq_pulse(s->irq);
                        qemu_mutex_unlock_iothread();
                    }
                } else if(msg->arg2 == 1) {
                    // copy the TC value into CC1 in the case of a rising edge
                    if((s->timer_CCR & (1 << 3)) && msg->arg3 == 1) {
                        s->timer_CC1 = s->timer_TC;
                    }
                    // copy the TC value into CC1 in the case of a falling edge
                    if((s->timer_CCR & (1 << 4)) && msg->arg3 == 2) {
                        s->timer_CC1 = s->timer_TC;
                    }
                    // trigger an interrupt if corresponding flag is set
                    if(s->timer_CCR & (1 << 5) && !(s->timer_IR & (1 << 5))) {
                        s->timer_IR |= (1 << 5);
                        qemu_mutex_lock_iothread();
                        qemu_irq_pulse(s->irq);
                        qemu_mutex_unlock_iothread();
                    }
                }
                if((s->timer_CTCR & 0x3) & msg->arg3) {
                    // increment TC if sent edge matches
                    if((s->timer_CTCR & (0x3 << 2)) == msg->arg2) {
                        // increment TC if the signal is sent to the correct capture pin
                        if(!(s->timer_CCR & (0x7 << (msg->arg2 * 3)))) {
                            // p. 698 of user manual:
                            // If Counter mode is selected for a particular CAPn input in
                            // the TnCTCR, the 3 bits for that input in the Capture Control 
                            // Register (TnCCR) must be programmed as 000.
                            s->timer_TC += 1;
                        }
                    }
                }
            }
        }
    }
}

static const MemoryRegionOps lpc4088_timer_ops = {
    .read = lpc4088_timer_read,
    .write = lpc4088_timer_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_lpc4088_timer = {
    .name = TYPE_LPC4088_TIMER,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_INT64(tc_last_checked_at, LPC4088TimerState),
        VMSTATE_UINT32(timer_IR, LPC4088TimerState),
        VMSTATE_UINT32(timer_TCR, LPC4088TimerState),
        VMSTATE_UINT32(timer_TC, LPC4088TimerState),
        VMSTATE_UINT32(timer_PR, LPC4088TimerState),
        VMSTATE_UINT32(timer_PC, LPC4088TimerState),
        VMSTATE_UINT32(timer_MCR, LPC4088TimerState),
        VMSTATE_UINT32(timer_MR0, LPC4088TimerState),
        VMSTATE_UINT32(timer_MR1, LPC4088TimerState),
        VMSTATE_UINT32(timer_MR2, LPC4088TimerState),
        VMSTATE_UINT32(timer_MR3, LPC4088TimerState),
        VMSTATE_UINT32(timer_CCR, LPC4088TimerState),
        VMSTATE_UINT32(timer_CC0, LPC4088TimerState),
        VMSTATE_UINT32(timer_CC1, LPC4088TimerState),
        VMSTATE_UINT32(timer_EMR, LPC4088TimerState),
        VMSTATE_UINT32(timer_CTCR, LPC4088TimerState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_timer_properties[] = {
    DEFINE_PROP_UINT64("clock-frequency", LPC4088TimerState, freq_hz, LPC4088_TIMER_FREQUENCY),
    DEFINE_PROP_STRING("timer-name", LPC4088TimerState, timer_name),
    DEFINE_PROP_BOOL("enable-rc", LPC4088TimerState, enable_rc, false),
    DEFINE_PROP_LINK("syscon", LPC4088TimerState, syscon, TYPE_LPC4088_SC, LPC4088SCState *),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_timer_init(Object *obj) {
    LPC4088TimerState *s = LPC4088TIMER(obj);
    DeviceState *ds = DEVICE(obj);
    
    object_initialize_child_with_props(
        obj, "RemoteCtrl", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, NULL,
        NULL
    );

    s->rcs.connected_device = ds;
}

static void lpc4088_timer_realize(DeviceState *dev, Error **errp) {
    LPC4088TimerState *s = LPC4088TIMER(dev);

    sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);
    
    memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_timer_ops, s, TYPE_LPC4088_TIMER, LPC4088_TIMER_MEM_SIZE);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

    s->tc_last_checked_at = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_timer_interrupt, s);

    s->rcs.callback = lpc4088_timer_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), NULL, NULL);
}

static void lpc4088_timer_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_timer_reset;
    device_class_set_props(dc, lpc4088_timer_properties);
    dc->vmsd = &vmstate_lpc4088_timer;
    dc->realize = lpc4088_timer_realize;
}

static const TypeInfo lpc4088_timer_info = {
    .name          = TYPE_LPC4088_TIMER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088TimerState),
    .instance_init = lpc4088_timer_init,
    .class_init    = lpc4088_timer_class_init,
};

static void lpc4088_timer_register_types(void) {
    type_register_static(&lpc4088_timer_info);
}

type_init(lpc4088_timer_register_types)
