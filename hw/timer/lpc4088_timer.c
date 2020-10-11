#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/timer/lpc4088_timer.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"

#ifndef LPC4088_TIMER_ERROR_DEBUG
#define LPC4088_TIMER_ERROR_DEBUG 1
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_TIMER_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_TIMER,__func__, ##args);}

#define REMOTE_CTRL_TIMER_MAGIC 0xDEEDBEE3
#define REMOTE_CTRL_CMD_TIMER_CAPTURE   0x11000100

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

static void lpc4088_timer_set_alarm(LPC4088TimerState *s, int64_t now);

static void lpc4088_timer_interrupt(void *opaque) {
    LPC4088TimerState *s = opaque;
	
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
	
	if(((s->timer_MCR & (1 << 0)) != 0)) {
		s->timer_IR |= 1 << 0;
		
		qemu_irq_pulse(s->irq);
		//qemu_set_irq(s->irq,1);
	}
	if(((s->timer_MCR & (1 << 3)) != 0)) {
		s->timer_IR |= 1 << 1;
		
		qemu_irq_pulse(s->irq);
		//qemu_set_irq(s->irq,1);
	}
	if(((s->timer_MCR & (1 << 6)) != 0)) {
		s->timer_IR |= 1 << 2;
		
		qemu_irq_pulse(s->irq);
		//qemu_set_irq(s->irq,1);
	}
	if(((s->timer_MCR & (1 << 9)) != 0)) {
		s->timer_IR |= 1 << 3;
		
		qemu_irq_pulse(s->irq);
		//qemu_set_irq(s->irq,1);
	}
	if(((s->timer_CCR & (1 << 2)) != 0) && ((s->enableRemoteInterrupt & (1 << 4)) != 0)) {
		s->enableRemoteInterrupt &= ~(1 << 4);
		s->timer_IR |= 1 << 4;
		qemu_irq_pulse(s->irq);
	}
	if(((s->timer_CCR & (1 << 5)) != 0) && ((s->enableRemoteInterrupt & (1 << 5)) != 0)) {
		s->enableRemoteInterrupt &= ~(1 << 5);
		s->timer_IR |= 1 << 5;
		qemu_irq_pulse(s->irq);
	}
	
	if(s->timer_IR != 0) {
		DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","Timer_Interrupt", (uint32_t) s->timer_IR);
	}
	
	lpc4088_timer_set_alarm(s, now);
}

static inline int64_t lpc4088_ns_to_ticks(LPC4088TimerState *s, int64_t t) {
    return muldiv64(t, s->freq_hz, LPC4088_TIMER_FREQUENCY) / (s->timer_PR + 1);
}

static void lpc4088_timer_set_alarm(LPC4088TimerState *s, int64_t now) {
    uint64_t ticks;
    int64_t now_ticks;
	//DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","Timer Set Alarm", (uint32_t) s->timer_MCR);
	
	if(((s->timer_MCR & (1 << 0)) != 0)) {
		now_ticks = lpc4088_ns_to_ticks(s, now);
		ticks = s->timer_MR0 - (now_ticks - s->tick_offset);
		
		s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1), LPC4088_TIMER_FREQUENCY, s->freq_hz);
		
		timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	}
	if(((s->timer_MCR & (1 << 3)) != 0)) {
		now_ticks = lpc4088_ns_to_ticks(s, now);
		ticks = s->timer_MR1 - (now_ticks - s->tick_offset);
		
		s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1), LPC4088_TIMER_FREQUENCY, s->freq_hz);
		
		timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	}
	if(((s->timer_MCR & (1 << 6)) != 0)) {
		now_ticks = lpc4088_ns_to_ticks(s, now);
		ticks = s->timer_MR2 - (now_ticks - s->tick_offset);
		
		s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1), LPC4088_TIMER_FREQUENCY, s->freq_hz);
		
		timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	}
	if(((s->timer_MCR & (1 << 9)) != 0)) {
		now_ticks = lpc4088_ns_to_ticks(s, now);
		ticks = s->timer_MR3 - (now_ticks - s->tick_offset);
		
		s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1), LPC4088_TIMER_FREQUENCY, s->freq_hz);
		
		timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	}
	if(((s->timer_CCR & (1 << 2)) != 0)) {
		now_ticks = lpc4088_ns_to_ticks(s, now);
		ticks = 0 - (now_ticks - s->tick_offset);
		s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1), LPC4088_TIMER_FREQUENCY, s->freq_hz);
		
		timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	}
	if(((s->timer_CCR & (1 << 5)) != 0)) {
		now_ticks = lpc4088_ns_to_ticks(s, now);
		ticks = 0 - (now_ticks - s->tick_offset);
		s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1), LPC4088_TIMER_FREQUENCY, s->freq_hz);
		
		timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	}
}

static void lpc4088_timer_reset(DeviceState *dev) {
    LPC4088TimerState *s = LPC4088TIMER(dev);
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

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
	
	s->enableRemoteInterrupt = 0;

    s->tick_offset = lpc4088_ns_to_ticks(s, now);
}

static uint64_t lpc4088_timer_read(void *opaque, hwaddr offset, unsigned size) {
    LPC4088TimerState *s = opaque;

    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        return s->timer_IR;
    case LPC4088_TIMER_REG_TCR:
        return s->timer_TCR;
    case LPC4088_TIMER_REG_TC:
		if(s->timer_CTCR == 0) {
			//Timer Reset
			if(((s->timer_TCR & (1 << 1)) != 0)) {
				s->timer_TC = 0;
				s->tick_offset = lpc4088_ns_to_ticks(s, now);
			}
			//Timer Not Count
			else if(((s->timer_TCR & (1 << 0)) == 0)) {
			}
			//Count Not Reset
			else if(((s->timer_TCR & (1 << 1)) == 0) && (((s->timer_TCR & (1 << 0)) != 0))) {
				s->timer_TC = s->timer_TC + lpc4088_ns_to_ticks(s, now) - s->tick_offset;
				s->tick_offset = lpc4088_ns_to_ticks(s, now);
			}
		}		
		DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_timer_register_name(offset), (uint32_t) s->timer_TC);
        return s->timer_TC;
	case LPC4088_TIMER_REG_PR:
        return s->timer_PR;
	case LPC4088_TIMER_REG_PC:
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
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    uint32_t timer_val = 0;
	
	DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_timer_register_name(offset), (uint32_t) value);

    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        s->timer_IR = value;
		
		if(((s->timer_MCR & (1 << 0)) != 0) && ((s->timer_IR & (1 << 0)) != 0)) {
			s->timer_IR = s->timer_IR & ~(1 << 0);
			qemu_set_irq(s->irq,0);
		}
		if(((s->timer_MCR & (1 << 3)) != 0) && ((s->timer_IR & (1 << 1)) != 0)) {
			s->timer_IR = s->timer_IR & ~(1 << 1);
			qemu_set_irq(s->irq,0);
		}
		if(((s->timer_MCR & (1 << 6)) != 0) && ((s->timer_IR & (1 << 2)) != 0)) {
			s->timer_IR = s->timer_IR & ~(1 << 2);
			qemu_set_irq(s->irq,0);
		}
		if(((s->timer_MCR & (1 << 9)) != 0) && ((s->timer_IR & (1 << 3)) != 0)) {
			s->timer_IR = s->timer_IR & ~(1 << 3);
			qemu_set_irq(s->irq,0);
		}
		if(((s->timer_CCR & (1 << 2)) != 0) && ((s->timer_IR & (1 << 4)) != 0)) {
			s->timer_IR = s->timer_IR & ~(1 << 4);
			qemu_set_irq(s->irq,0);
		}
		if(((s->timer_CCR & (1 << 5)) != 0) && ((s->timer_IR & (1 << 5)) != 0)) {
			s->timer_IR = s->timer_IR & ~(1 << 5);
			qemu_set_irq(s->irq,0);
		}
        return;
    case LPC4088_TIMER_REG_TCR:
        s->timer_TCR = value;
		if(s->timer_CTCR == 0) {
			if(((s->timer_TCR & (1 << 1)) == 0) && (((s->timer_TCR & (1 << 0)) != 0))) {
				timer_val = lpc4088_ns_to_ticks(s, now) - s->tick_offset;
				s->tick_offset = lpc4088_ns_to_ticks(s, now) - timer_val;
				lpc4088_timer_set_alarm(s, now);
			}
		}		
        return;
    case LPC4088_TIMER_REG_TC:
        s->timer_TC = value;
        return;
    case LPC4088_TIMER_REG_PR:
        s->timer_PR = value;
        return;
    case LPC4088_TIMER_REG_PC:
        s->timer_PC = value;
        return;
    case LPC4088_TIMER_REG_MCR:
		s->timer_MCR = value;
        return;
	case LPC4088_TIMER_REG_MR0:
        s->timer_MR0 = value;
        return;
	case LPC4088_TIMER_REG_MR1:
        s->timer_MR1 = value;
        return;
	case LPC4088_TIMER_REG_MR2:
        s->timer_MR2 = value;
        return;
	case LPC4088_TIMER_REG_MR3:
        s->timer_MR3 = value;
        return;
	case LPC4088_TIMER_REG_CCR:
        s->timer_CCR = value;
        return;
	case LPC4088_TIMER_REG_CC0:
        s->timer_CC0 = value;
        return;
	case LPC4088_TIMER_REG_CC1:
        s->timer_CC1 = value;
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
	
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    
	if(s->enable_rc) {
		if(msg->arg1 == s->timer_name[0] && msg->arg2 < 32) {
			if(msg->cmd == REMOTE_CTRL_CMD_TIMER_CAPTURE) {
				s->timer_CC0 = s->timer_TC;
				 
				DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","Remote CC0 is equal TC", (uint32_t) s->timer_TC);
				s->enableRemoteInterrupt |= 1 << 4;
				
				lpc4088_timer_set_alarm(s, now);
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
        VMSTATE_INT64(tick_offset, LPC4088TimerState),
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
    DEFINE_PROP_UINT64("clock-frequency", struct LPC4088TimerState, freq_hz, LPC4088_TIMER_FREQUENCY),
	DEFINE_PROP_STRING("timer-name", struct LPC4088TimerState, timer_name),
    DEFINE_PROP_BOOL("enable-rc", struct LPC4088TimerState, enable_rc, false),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_timer_init(Object *obj) {
    LPC4088TimerState *s = LPC4088TIMER(obj);
	DeviceState *ds = DEVICE(obj);
	
	object_initialize_child_with_props(
        obj, "RemoteCtrl", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, &error_abort,
        NULL
    );

    s->rcs.connected_device = ds;
	
    //sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    //memory_region_init_io(&s->iomem, obj, &lpc4088_timer_ops, s, TYPE_LPC4088_TIMER, LPC4088_TIMER_MEM_SIZE);
						  
    //sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
}

static void lpc4088_timer_realize(DeviceState *dev, Error **errp) {
    LPC4088TimerState *s = LPC4088TIMER(dev);

	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);
	
	memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_timer_ops, s, TYPE_LPC4088_TIMER, LPC4088_TIMER_MEM_SIZE);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

	s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_timer_interrupt, s);

    s->rcs.callback = lpc4088_timer_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), qdev_get_parent_bus(DEVICE(&s->rcs)), errp);
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
