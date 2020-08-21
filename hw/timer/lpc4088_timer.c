#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/timer/lpc4088_timer.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef LPC4088_TIMER_ERR_DEBUG
#define LPC4088_TIMER_ERR_DEBUG 1
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (LPC4088_TIMER_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

#define DPRINTF2(fmt, args...) \
    if(LPC4088_TIMER_ERR_DEBUG) { \
        fprintf(stderr, "[%s]%s: " fmt, TYPE_LPC4088_TIMER, \
                __func__, ##args); \
    }

static void lpc4088_timer_set_alarm(LPC4088TimerState *s, int64_t now);

static void lpc4088_timer_interrupt(void *opaque)
{
    LPC4088TimerState *s = opaque;

    DB_PRINT("Interrupt\n");
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Interrupt", (uint32_t) s->timer_IR);
	
	lpc4088_timer_set_alarm(s, s->hit_time);
}

static inline int64_t lpc4088_ns_to_ticks(LPC4088TimerState *s, int64_t t)
{
    return muldiv64(t, s->freq_hz, 1000000000ULL) / (s->timer_PR + 1);
}

static void lpc4088_timer_set_alarm(LPC4088TimerState *s, int64_t now)
{
    uint64_t ticks;
    int64_t now_ticks;

	now_ticks = lpc4088_ns_to_ticks(s, now);
    ticks = 200000000 - (now_ticks - s->tick_offset);
	
	s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->timer_PR + 1),
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

static void lpc4088_timer_reset(DeviceState *dev)
{
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

    s->tick_offset = lpc4088_ns_to_ticks(s, now);
}

static uint64_t lpc4088_timer_read(void *opaque, hwaddr offset,
                           unsigned size)
{
    LPC4088TimerState *s = opaque;

    DB_PRINT("Read 0x%"HWADDR_PRIx"\n", offset);

    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        return s->timer_IR;
    case LPC4088_TIMER_REG_TCR:
        return s->timer_TCR;
    case LPC4088_TIMER_REG_TC:
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
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
    }

    return 0;
}

static void lpc4088_timer_write(void *opaque, hwaddr offset,
                        uint64_t val64, unsigned size)
{
    LPC4088TimerState *s = opaque;
    uint32_t value = val64;
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    uint32_t timer_val = 0;

    DB_PRINT("Write 0x%x, 0x%"HWADDR_PRIx"\n", value, offset);
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Deneme",
            (uint32_t) value);

    switch (offset) {
    case LPC4088_TIMER_REG_IR:
        s->timer_IR = value;
		
		timer_val = lpc4088_ns_to_ticks(s, now) - s->tick_offset;
	
		s->tick_offset = lpc4088_ns_to_ticks(s, now) - timer_val;
		lpc4088_timer_set_alarm(s, now);
	
        return;
    case LPC4088_TIMER_REG_TCR:
        s->timer_TCR = value;
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
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
        return;
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
    DEFINE_PROP_UINT64("clock-frequency", struct LPC4088TimerState,
                       freq_hz, 1000000000),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_timer_init(Object *obj)
{
    LPC4088TimerState *s = LPC4088TIMER(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->iomem, obj, &lpc4088_timer_ops, s,
                          TYPE_LPC4088_TIMER, LPC4088_TIMER_MEM_SIZE);
						  
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
}

static void lpc4088_timer_realize(DeviceState *dev, Error **errp)
{
    LPC4088TimerState *s = LPC4088TIMER(dev);
    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_timer_interrupt, s);
}

static void lpc4088_timer_class_init(ObjectClass *klass, void *data)
{
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

static void lpc4088_timer_register_types(void)
{
    type_register_static(&lpc4088_timer_info);
}

type_init(lpc4088_timer_register_types)
