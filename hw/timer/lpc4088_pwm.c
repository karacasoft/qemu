#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/timer/lpc4088_pwm.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef LPC4088_PWM_ERR_DEBUG
#define LPC4088_PWM_ERR_DEBUG 1
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (LPC4088_PWM_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

#define DPRINTF2(fmt, args...) \
    if(LPC4088_PWM_ERR_DEBUG) { \
        fprintf(stderr, "[%s]%s: " fmt, TYPE_LPC4088_PWM, \
                __func__, ##args); \
    }

static void lpc4088_pwm_set_alarm(LPC4088PWMState *s, int64_t now);

static void lpc4088_pwm_interrupt(void *opaque)
{
    LPC4088PWMState *s = opaque;

    DB_PRINT("Interrupt\n");
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Interrupt", (uint32_t) s->pwm_IR);
	
	lpc4088_pwm_set_alarm(s, s->hit_time);
}

static inline int64_t lpc4088_pwm_ns_to_ticks(LPC4088PWMState *s, int64_t t)
{
    return muldiv64(t, s->freq_hz, 1000000000ULL) / (s->pwm_PR + 1);
}

static void lpc4088_pwm_set_alarm(LPC4088PWMState *s, int64_t now)
{
    uint64_t ticks;
    int64_t now_ticks;

	now_ticks = lpc4088_pwm_ns_to_ticks(s, now);
    ticks = 200000000 - (now_ticks - s->tick_offset);
	
	s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->pwm_PR + 1),
                               1000000000ULL, s->freq_hz);
	
	timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
	
    /*if (s->tim_arr == 0) {
        return;
    }

    DB_PRINT("Alarm set at: 0x%x\n", s->tim_cr1);

    now_ticks = stm32f2xx_ns_to_ticks(s, now);
    ticks = s->tim_arr - (now_ticks - s->tick_offset);

    DB_PRINT("Alarm set in %d ticks\n", (int) ticks);

    s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->tim_psc + 1),
                               1000000000ULL, s->freq_hz);

    timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
    DB_PRINT("Wait Time: %" PRId64 " ticks\n", s->hit_time);*/
}

static void lpc4088_pwm_reset(DeviceState *dev)
{
    LPC4088PWMState *s = LPC4088PWM(dev);
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

    s->pwm_IR = 0;
	s->pwm_TCR = 0;
	s->pwm_TC = 0;
	s->pwm_PR = 0;
	s->pwm_PC = 0;
	s->pwm_MCR = 0;
	s->pwm_MR0 = 0;
	s->pwm_MR1 = 0;
	s->pwm_MR2 = 0;
	s->pwm_MR3 = 0;
	s->pwm_CCR = 0;
	s->pwm_CC0 = 0;
	s->pwm_CC1 = 0;
	s->pwm_CC2 = 0;
	s->pwm_CC3 = 0;
	s->pwm_MR4 = 0;
	s->pwm_MR5 = 0;
	s->pwm_MR6 = 0;
	s->pwm_PCR = 0;
	s->pwm_LER = 0;
	s->pwm_CTCR = 0;

    s->tick_offset = lpc4088_pwm_ns_to_ticks(s, now);
}

static uint64_t lpc4088_pwm_read(void *opaque, hwaddr offset,
                           unsigned size)
{
    LPC4088PWMState *s = opaque;

    DB_PRINT("Read 0x%"HWADDR_PRIx"\n", offset);

    switch (offset) {
    case LPC4088_PWM_REG_IR:
        return s->pwm_IR;
    case LPC4088_PWM_REG_TCR:
        return s->pwm_TCR;
    case LPC4088_PWM_REG_TC:
        return s->pwm_TC;
	case LPC4088_PWM_REG_PR:
        return s->pwm_PR;
	case LPC4088_PWM_REG_PC:
        return s->pwm_PC;
	case LPC4088_PWM_REG_MCR:
        return s->pwm_MCR;	
    case LPC4088_PWM_REG_MR0:
        return s->pwm_MR0;
	case LPC4088_PWM_REG_MR1:
        return s->pwm_MR1;
	case LPC4088_PWM_REG_MR2:
        return s->pwm_MR2;
	case LPC4088_PWM_REG_MR3:
        return s->pwm_MR3;
	case LPC4088_PWM_REG_CCR:
        return s->pwm_CCR;
	case LPC4088_PWM_REG_CC0:
        return s->pwm_CC0;
	case LPC4088_PWM_REG_CC1:
        return s->pwm_CC1;
	case LPC4088_PWM_REG_CC2:
        return s->pwm_CC2;
	case LPC4088_PWM_REG_CC3:
        return s->pwm_CC3;
	case LPC4088_PWM_REG_MR4:
        return s->pwm_MR4;
	case LPC4088_PWM_REG_MR5:
        return s->pwm_MR5;
	case LPC4088_PWM_REG_MR6:
        return s->pwm_MR6;
	case LPC4088_PWM_REG_PCR:
        return s->pwm_PCR;
	case LPC4088_PWM_REG_LER:
        return s->pwm_LER;
	case LPC4088_PWM_REG_CTCR:
        return s->pwm_CTCR;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
    }

    return 0;
}

static void lpc4088_pwm_write(void *opaque, hwaddr offset,
                        uint64_t val64, unsigned size)
{
    LPC4088PWMState *s = opaque;
    uint32_t value = val64;
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    uint32_t timer_val = 0;

    DB_PRINT("Write 0x%x, 0x%"HWADDR_PRIx"\n", value, offset);
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Deneme",
            (uint32_t) value);

    switch (offset) {
    case LPC4088_PWM_REG_IR:
        s->pwm_IR = value;
		
		timer_val = lpc4088_pwm_ns_to_ticks(s, now) - s->tick_offset;
	
		s->tick_offset = lpc4088_pwm_ns_to_ticks(s, now) - timer_val;
		lpc4088_pwm_set_alarm(s, now);
	
        return;
    case LPC4088_PWM_REG_TCR:
        s->pwm_TCR = value;
        return;
    case LPC4088_PWM_REG_TC:
        s->pwm_TC = value;
        return;
    case LPC4088_PWM_REG_PR:
        s->pwm_PR = value;
        return;
    case LPC4088_PWM_REG_PC:
        s->pwm_PC = value;
        return;
    case LPC4088_PWM_REG_MCR:
        s->pwm_MCR = value;
        return;
	case LPC4088_PWM_REG_MR0:
        s->pwm_MR0 = value;
        return;
	case LPC4088_PWM_REG_MR1:
        s->pwm_MR1 = value;
        return;
	case LPC4088_PWM_REG_MR2:
        s->pwm_MR2 = value;
        return;
	case LPC4088_PWM_REG_MR3:
        s->pwm_MR3 = value;
        return;
	case LPC4088_PWM_REG_CCR:
        s->pwm_CCR = value;
        return;
	case LPC4088_PWM_REG_CC0:
        s->pwm_CC0 = value;
        return;
	case LPC4088_PWM_REG_CC1:
        s->pwm_CC1 = value;
        return;
	case LPC4088_PWM_REG_CC2:
        s->pwm_CC2 = value;
        return;
	case LPC4088_PWM_REG_CC3:
        s->pwm_CC3 = value;
        return;
	case LPC4088_PWM_REG_MR4:
        s->pwm_MR4 = value;
        return;
	case LPC4088_PWM_REG_MR5:
        s->pwm_MR5 = value;
        return;
	case LPC4088_PWM_REG_MR6:
        s->pwm_MR6 = value;
        return;
	case LPC4088_PWM_REG_PCR:
        s->pwm_PCR = value;
        return;
	case LPC4088_PWM_REG_LER:
        s->pwm_LER = value;
        return;
	case LPC4088_PWM_REG_CTCR:
        s->pwm_CTCR = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
        return;
    }
	
    
}

static const MemoryRegionOps lpc4088_pwm_ops = {
    .read = lpc4088_pwm_read,
    .write = lpc4088_pwm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_lpc4088_pwm = {
    .name = TYPE_LPC4088_PWM,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_INT64(tick_offset, LPC4088PWMState),
        VMSTATE_UINT32(pwm_IR, LPC4088PWMState),
        VMSTATE_UINT32(pwm_TCR, LPC4088PWMState),
        VMSTATE_UINT32(pwm_TC, LPC4088PWMState),
        VMSTATE_UINT32(pwm_PR, LPC4088PWMState),
        VMSTATE_UINT32(pwm_PC, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MCR, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MR0, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MR1, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MR2, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MR3, LPC4088PWMState),
        VMSTATE_UINT32(pwm_CCR, LPC4088PWMState),
        VMSTATE_UINT32(pwm_CC0, LPC4088PWMState),
        VMSTATE_UINT32(pwm_CC1, LPC4088PWMState),
        VMSTATE_UINT32(pwm_CC2, LPC4088PWMState),
        VMSTATE_UINT32(pwm_CC3, LPC4088PWMState),
		VMSTATE_UINT32(pwm_MR4, LPC4088PWMState),
		VMSTATE_UINT32(pwm_MR5, LPC4088PWMState),
		VMSTATE_UINT32(pwm_MR6, LPC4088PWMState),
		VMSTATE_UINT32(pwm_PCR, LPC4088PWMState),
		VMSTATE_UINT32(pwm_LER, LPC4088PWMState),
		VMSTATE_UINT32(pwm_CTCR, LPC4088PWMState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_pwm_properties[] = {
    DEFINE_PROP_UINT64("clock-frequency", struct LPC4088PWMState,
                       freq_hz, 1000000000),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_pwm_init(Object *obj)
{
    LPC4088PWMState *s = LPC4088PWM(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->iomem, obj, &lpc4088_pwm_ops, s,
                          TYPE_LPC4088_PWM, LPC4088_PWM_MEM_SIZE);
						  
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
}

static void lpc4088_pwm_realize(DeviceState *dev, Error **errp)
{
    LPC4088PWMState *s = LPC4088PWM(dev);
    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_pwm_interrupt, s);
}

static void lpc4088_pwm_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_pwm_reset;
    device_class_set_props(dc, lpc4088_pwm_properties);
    dc->vmsd = &vmstate_lpc4088_pwm;
    dc->realize = lpc4088_pwm_realize;
}

static const TypeInfo lpc4088_pwm_info = {
    .name          = TYPE_LPC4088_PWM,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088PWMState),
    .instance_init = lpc4088_pwm_init,
    .class_init    = lpc4088_pwm_class_init,
};

static void lpc4088_pwm_register_types(void)
{
    type_register_static(&lpc4088_pwm_info);
}

type_init(lpc4088_pwm_register_types)
