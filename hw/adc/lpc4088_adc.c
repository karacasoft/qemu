#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/adc/lpc4088_adc.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"

#ifndef LPC4088_ADC_ERROR_DEBUG
#define LPC4088_ADC_ERROR_DEBUG 1
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_ADC_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_ADC,__func__, ##args);}

#define REMOTE_CTRL_ADC_MAGIC 0xDEEDBEE5
#define REMOTE_CTRL_CMD_ADC_DATA   0x21000100

static const char *lpc4088_adc_register_name(uint32_t offset) __attribute__((unused));

static const char *lpc4088_adc_register_name(uint32_t offset) {
	switch (offset) {
    case LPC4088_ADC_REG_CR:
        return "ADC_CR";
    case LPC4088_ADC_REG_GDR:
        return "ADC_GDR";
	case LPC4088_ADC_REG_INTEN:
        return "ADC_INTERN";
	case LPC4088_ADC_REG_DR0:
        return "ADC_DR0";
	case LPC4088_ADC_REG_DR1:
        return "ADC_DR1";
	case LPC4088_ADC_REG_DR2:
        return "ADC_DR2";
	case LPC4088_ADC_REG_DR3:
        return "ADC_DR3";
	case LPC4088_ADC_REG_DR4:
        return "ADC_DR4";
	case LPC4088_ADC_REG_DR5:
        return "ADC_DR5";
	case LPC4088_ADC_REG_DR6:
        return "ADC_DR6";
	case LPC4088_ADC_REG_DR7:
        return "ADC_DR7";
	case LPC4088_ADC_REG_STAT:
        return "ADC_STAT";
	case LPC4088_ADC_REG_ADTRM:
        return "ADC_ADTRM";
    default:
        return "ADC_[?]";
    }
}

static void lpc4088_adc_timer_set_alarm(LPC4088ADCState *s, int64_t now);

static void lpc4088_adc_timer_interrupt(void *opaque) {
    LPC4088ADCState *s = opaque;
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
	uint32_t lastDRData;
	
	if(((s->adc_CR & (1 << 24)) != 0) && ((s->adc_CR & (1 << 21)) != 0)) {
		if(((s->adc_CR & (1 << 1)) != 0)) {
			lastDRData = s->adc_DR1;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD1Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR1 = lastDRData;
			
			if((s->adc_INTEN & (1 << 1)) != 0) {
				qemu_irq_pulse(s->irq);
			}			
		}
		else if(((s->adc_CR & (1 << 2)) != 0)) {
			lastDRData = s->adc_DR2;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD2Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR2 = lastDRData;
					
			if((s->adc_INTEN & (1 << 2)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
		else if(((s->adc_CR & (1 << 3)) != 0)) {
			lastDRData = s->adc_DR3;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD3Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR3 = lastDRData;
					
			if((s->adc_INTEN & (1 << 3)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
		else if(((s->adc_CR & (1 << 4)) != 0)) {
			lastDRData = s->adc_DR4;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD4Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR4 = lastDRData;
					
			if((s->adc_INTEN & (1 << 4)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
		else if(((s->adc_CR & (1 << 5)) != 0)) {
			lastDRData = s->adc_DR5;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD5Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR5 = lastDRData;
					
			if((s->adc_INTEN & (1 << 5)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
		else if(((s->adc_CR & (1 << 6)) != 0)) {
			lastDRData = s->adc_DR6;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD6Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR6 = lastDRData;
					
			if((s->adc_INTEN & (1 << 6)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
		else if(((s->adc_CR & (1 << 7)) != 0)) {
			lastDRData = s->adc_DR7;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD7Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR7 = lastDRData;
					
			if((s->adc_INTEN & (1 << 7)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
		else {
			lastDRData = s->adc_DR0;
			lastDRData = lastDRData & ~(0xFFF << 4);
			lastDRData = lastDRData | s->lastD0Data << 4;
			lastDRData = lastDRData | 1 << 31;
			s->adc_DR0 = lastDRData;
					
			if((s->adc_INTEN & (1 << 0)) != 0) {
				qemu_irq_pulse(s->irq);
			}
		}
	}
	
	lpc4088_adc_timer_set_alarm(s, now);
}

static inline int64_t lpc4088_ns_to_ticks(LPC4088ADCState *s, int64_t t) {
    return muldiv64(t, s->freq_hz, LPC4088_ADC_TIMER_FREQUENCY) / (s->ADCPR + 1);
}

static void lpc4088_adc_timer_set_alarm(LPC4088ADCState *s, int64_t now) {
    uint64_t ticks;
    int64_t now_ticks;
	//DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","ADC_Set_Alarm", (uint32_t) 0);
	
	now_ticks = lpc4088_ns_to_ticks(s, now);
	
	ticks = 1000000 - (now_ticks - s->tick_offset);
	
	s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->ADCPR + 1), LPC4088_ADC_TIMER_FREQUENCY, s->freq_hz);
	
	timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
}

static void lpc4088_adc_reset(DeviceState *dev) {
    LPC4088ADCState *s = LPC4088ADC(dev);
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

	s->adc_CR = 0x00000000;
    s->adc_GDR = 0x00000000;
    s->adc_INTEN = 0x00000000;
    s->adc_DR0 = 0x00000000;
    s->adc_DR1 = 0x00000000;
	s->adc_DR2 = 0x00000000;
	s->adc_DR3 = 0x00000000;
	s->adc_DR4 = 0x00000000;
	s->adc_DR5 = 0x00000000;
	s->adc_DR6 = 0x00000000;
	s->adc_DR7 = 0x00000000;
	s->adc_STAT = 0x00000000;
	s->adc_ADTRM = 0x00000000;
	
	s->enableRemoteInterrupt = 0;
	
	s->lastD0Data = 0;
	s->lastD1Data = 0;
	s->lastD2Data = 0;
	s->lastD3Data = 0;
	s->lastD4Data = 0;
	s->lastD5Data = 0;
	s->lastD6Data = 0;
	s->lastD7Data = 0;
	
	s->ADCPR = 50;
	s->tick_offset = lpc4088_ns_to_ticks(s, now);
}

// FIXME: Please remove this function if it is actually unused
static uint32_t lpc4088_adc_generate_value(LPC4088ADCState *s) __attribute__((unused));

static uint32_t lpc4088_adc_generate_value(LPC4088ADCState *s)
{
    /* Attempts to fake some ADC values */
    s->adc_DR0 = s->adc_DR0 + 7;

    return s->adc_DR0;
}

static uint64_t lpc4088_adc_read(void *opaque, hwaddr offset, unsigned int size)
{
    LPC4088ADCState *s = opaque;
	uint32_t read_val;

    switch (offset) {
    case LPC4088_ADC_REG_CR:
        read_val = s->adc_CR;
		break;
    case LPC4088_ADC_REG_GDR:
		s->adc_GDR = s->adc_GDR & ~(1 << 31);
        read_val = s->adc_GDR;
		break;
	case LPC4088_ADC_REG_INTEN:
        read_val = s->adc_INTEN;
		break;
	case LPC4088_ADC_REG_DR0:
		s->adc_DR0 = s->adc_DR0 & ~(1 << 31);
        read_val = s->adc_DR0;
		break;
	case LPC4088_ADC_REG_DR1:
		s->adc_DR1 = s->adc_DR1 & ~(1 << 31);
        read_val = s->adc_DR1;
		break;
	case LPC4088_ADC_REG_DR2:
		s->adc_DR2 = s->adc_DR2 & ~(1 << 31);
        read_val = s->adc_DR2;
		break;
	case LPC4088_ADC_REG_DR3:
		s->adc_DR3 = s->adc_DR3 & ~(1 << 31);
        read_val = s->adc_DR3;
		break;
	case LPC4088_ADC_REG_DR4:
		s->adc_DR4 = s->adc_DR4 & ~(1 << 31);
        read_val = s->adc_DR4;
		break;
	case LPC4088_ADC_REG_DR5:
		s->adc_DR5 = s->adc_DR5 & ~(1 << 31);
        read_val = s->adc_DR5;
		break;
	case LPC4088_ADC_REG_DR6:
		s->adc_DR6 = s->adc_DR6 & ~(1 << 31);
        read_val = s->adc_DR6;
		break;
	case LPC4088_ADC_REG_DR7:
		s->adc_DR7 = s->adc_DR7 & ~(1 << 31);
        read_val = s->adc_DR7;
		break;
	case LPC4088_ADC_REG_STAT:
        read_val = s->adc_STAT;
		break;
	case LPC4088_ADC_REG_ADTRM:
        read_val = s->adc_ADTRM;
		break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, offset);
    }

	DEBUG_PRINT("[Read from ADC%c](%s, value = 0x%" PRIx32 ")\n", s->adc_name[0], lpc4088_adc_register_name(offset), (uint32_t) read_val);

    return 0;
}

static void lpc4088_adc_write(void *opaque, hwaddr offset, uint64_t val64, unsigned int size) {
    LPC4088ADCState *s = opaque;
    uint32_t value = (uint32_t) val64;
	//int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    //uint32_t timer_val = 0;

	DEBUG_PRINT("[Write to ADC%c](%s, value = 0x%" PRIx32 ")\n", s->adc_name[0], lpc4088_adc_register_name(offset), (uint32_t) value);

    switch (offset) {
    case LPC4088_ADC_REG_CR:
		s->adc_CR = value;
		/*if(((s->adc_CR & (1 << 24)) != 0)) {
			timer_val = lpc4088_ns_to_ticks(s, now) - s->tick_offset;
			s->tick_offset = lpc4088_ns_to_ticks(s, now) - timer_val;
			lpc4088_adc_timer_set_alarm(s, now);
		}*/
        break;
    case LPC4088_ADC_REG_GDR:
        s->adc_GDR = value;
        break;
	case LPC4088_ADC_REG_INTEN:
        s->adc_INTEN = value;
        break;
	case LPC4088_ADC_REG_DR0:
        s->adc_DR0 = value;
        break;
	case LPC4088_ADC_REG_DR1:
        s->adc_DR1 = value;
        break;
	case LPC4088_ADC_REG_DR2:
        s->adc_DR2 = value;
        break;
	case LPC4088_ADC_REG_DR3:
        s->adc_DR3 = value;
        break;
	case LPC4088_ADC_REG_DR4:
        s->adc_DR4 = value;
        break;
	case LPC4088_ADC_REG_DR5:
        s->adc_DR5 = value;
        break;
	case LPC4088_ADC_REG_DR6:
        s->adc_DR6 = value;
        break;
	case LPC4088_ADC_REG_DR7:
        s->adc_DR7 = value;
        break;
	case LPC4088_ADC_REG_STAT:
        s->adc_STAT = value;
        break;
	case LPC4088_ADC_REG_ADTRM:
        s->adc_ADTRM = value;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, offset);
    }
}

static void lpc4088_adc_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg) {
    LPC4088ADCState *s = LPC4088ADC(rcs->connected_device);
	
	if(s->enable_rc) {
		if(msg->arg1 == s->adc_name[0] && msg->arg2 < 32) {
			if(msg->cmd == REMOTE_CTRL_CMD_ADC_DATA) {
				if(msg->arg2 == 0) {
					s->lastD0Data = msg->arg3;
				}
				else if(msg->arg2 == 1) {
					s->lastD1Data = msg->arg3;
				}
				else if(msg->arg2 == 2) {
					s->lastD2Data = msg->arg3;
				}
				else if(msg->arg2 == 3) {
					s->lastD3Data = msg->arg3;
				}
				else if(msg->arg2 == 4) {
					s->lastD4Data = msg->arg3;
				}
				else if(msg->arg2 == 5) {
					s->lastD5Data = msg->arg3;
				}
				else if(msg->arg2 == 6) {
					s->lastD6Data = msg->arg3;
				}
				else if(msg->arg2 == 7) {
					s->lastD7Data = msg->arg3;
				}
			}
		}
    }
}

static const MemoryRegionOps lpc4088_adc_ops = {
    .read = lpc4088_adc_read,
    .write = lpc4088_adc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    //.impl.min_access_size = 4,
    //.impl.max_access_size = 4,
};

static const VMStateDescription vmstate_lpc4088_adc = {
    .name = TYPE_LPC4088_ADC,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
		VMSTATE_INT64(tick_offset, LPC4088ADCState),
        VMSTATE_UINT32(adc_CR, LPC4088ADCState),
        VMSTATE_UINT32(adc_GDR, LPC4088ADCState),
        VMSTATE_UINT32(adc_INTEN, LPC4088ADCState),
        VMSTATE_UINT32(adc_DR0, LPC4088ADCState),
        VMSTATE_UINT32(adc_DR1, LPC4088ADCState),
		VMSTATE_UINT32(adc_DR2, LPC4088ADCState),
		VMSTATE_UINT32(adc_DR3, LPC4088ADCState),
		VMSTATE_UINT32(adc_DR4, LPC4088ADCState),
		VMSTATE_UINT32(adc_DR5, LPC4088ADCState),
		VMSTATE_UINT32(adc_DR6, LPC4088ADCState),
		VMSTATE_UINT32(adc_DR7, LPC4088ADCState),
		VMSTATE_UINT32(adc_STAT, LPC4088ADCState),
		VMSTATE_UINT32(adc_ADTRM, LPC4088ADCState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_adc_properties[] = {
	DEFINE_PROP_UINT64("clock-frequency", struct LPC4088ADCState, freq_hz, LPC4088_ADC_TIMER_FREQUENCY),
	DEFINE_PROP_STRING("adc-name", struct LPC4088ADCState, adc_name),
    DEFINE_PROP_BOOL("enable-rc", struct LPC4088ADCState, enable_rc, false),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_adc_init(Object *obj) {
    LPC4088ADCState *s = LPC4088ADC(obj);
	
	DeviceState *ds = DEVICE(obj);
	
	object_initialize_child_with_props(
        obj, "RemoteCtrl", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, &error_abort,
        NULL
    );

    s->rcs.connected_device = ds;
	
    /*sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &lpc4088_adc_ops, s,
                          TYPE_LPC4088_ADC, LPC4088_ADC_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);*/
}

static void lpc4088_adc_realize(DeviceState *dev, Error **errp) {	
    LPC4088ADCState *s = LPC4088ADC(dev);
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    uint32_t timer_val = 0;
	
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);
	
	memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_adc_ops, s, TYPE_LPC4088_ADC, LPC4088_ADC_MEM_SIZE);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

	s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_adc_timer_interrupt, s);
	
	timer_val = lpc4088_ns_to_ticks(s, now) - s->tick_offset;
	s->tick_offset = lpc4088_ns_to_ticks(s, now) - timer_val;
	lpc4088_adc_timer_set_alarm(s, now);
	
    s->rcs.callback = lpc4088_adc_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), qdev_get_parent_bus(DEVICE(&s->rcs)), errp);
}

static void lpc4088_adc_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_adc_reset;
	device_class_set_props(dc, lpc4088_adc_properties);
    dc->vmsd = &vmstate_lpc4088_adc;
	dc->realize = lpc4088_adc_realize;
}

static const TypeInfo lpc4088_adc_info = {
    .name          = TYPE_LPC4088_ADC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088ADCState),
    .instance_init = lpc4088_adc_init,
    .class_init    = lpc4088_adc_class_init,
};

static void lpc4088_adc_register_types(void) {
    type_register_static(&lpc4088_adc_info);
}

type_init(lpc4088_adc_register_types)
