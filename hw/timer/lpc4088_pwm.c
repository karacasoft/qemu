#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/timer/lpc4088_pwm.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"

#ifndef LPC4088_PWM_ERR_DEBUG
#define LPC4088_PWM_ERR_DEBUG 1
#endif

#define REMOTE_CTRL_PWM_MAGIC 0xEADDBEE3
#define REMOTE_CTRL_PWM_CMD_SEND_CAPTURE 0x30100

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (LPC4088_PWM_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DEBUG_PRINT(fmt, args...) if(LPC4088_PWM_ERR_DEBUG) {\
    fprintf(stderr, "[%s]" fmt, __func__, ##args);\
}

static const char *lpc4088_pwm_register_name(uint32_t offset) __attribute__((unused));

static const char *lpc4088_pwm_register_name(uint32_t offset) {
    switch (offset) {
    case LPC4088_PWM_REG_IR:
        return "PWM_IR";
    case LPC4088_PWM_REG_TCR:
        return "PWM_TCR";
    case LPC4088_PWM_REG_TC:
        return "PWM_TC";
    case LPC4088_PWM_REG_PR:
        return "PWM_PR";
    case LPC4088_PWM_REG_PC:
        return "PWM_PC";
    case LPC4088_PWM_REG_MCR:
        return "PWM_MCR";
    case LPC4088_PWM_REG_MR0:
        return "PWM_MR0";
    case LPC4088_PWM_REG_MR1:
        return "PWM_MR1";
    case LPC4088_PWM_REG_MR2:
        return "PWM_MR2";
    case LPC4088_PWM_REG_MR3:
        return "PWM_MR3";
    case LPC4088_PWM_REG_CCR:
        return "PWM_CCR";
    case LPC4088_PWM_REG_CC0:
        return "PWM_CC0";
    case LPC4088_PWM_REG_CC1:
        return "PWM_CC1";
    case LPC4088_PWM_REG_CC2:
        return "PWM_CC2";
    case LPC4088_PWM_REG_CC3:
        return "PWM_CC3";
    case LPC4088_PWM_REG_MR4:
        return "PWM_MR4";
    case LPC4088_PWM_REG_MR5:
        return "PWM_MR5";
    case LPC4088_PWM_REG_MR6:
        return "PWM_MR6";
    case LPC4088_PWM_REG_PCR:
        return "PWM_PCR";
    case LPC4088_PWM_REG_LER:
        return "PWM_LER";
    case LPC4088_PWM_REG_CTCR:
        return "PWM_CTCR";
    default:
        return "PWM_[?]";
    }
}

static void lpc4088_pwm_match_event0(LPC4088TimerState *s) {
    LPC4088TimerState *ts = LPC4088TIMER(s);
    LPC4088PWMState *pwms = LPC4088PWM(s);
    LPC4088PWMClass *pwmc = LPC4088PWM_GET_CLASS(pwms);

    pwmc->parent_match_events[0](s);

    if(ts->timer_TCR & (1 << 3)) {
        if(pwms->pwm_LER & 0x1) ts->timer_MR0 = pwms->pwm_shadow_mr[0];
        if(pwms->pwm_LER & 0x2) ts->timer_MR1 = pwms->pwm_shadow_mr[1];
        if(pwms->pwm_LER & 0x4) ts->timer_MR2 = pwms->pwm_shadow_mr[2];
        if(pwms->pwm_LER & 0x8) ts->timer_MR3 = pwms->pwm_shadow_mr[3];
        if(pwms->pwm_LER & 0x10) pwms->pwm_MR4 = pwms->pwm_shadow_mr[4];
        if(pwms->pwm_LER & 0x20) pwms->pwm_MR5 = pwms->pwm_shadow_mr[5];
        if(pwms->pwm_LER & 0x40) pwms->pwm_MR6 = pwms->pwm_shadow_mr[6];
    }
}

static void lpc4088_pwm_reset(DeviceState *dev) {
    LPC4088PWMState *s = LPC4088PWM(dev);
    LPC4088PWMClass *pwmc = LPC4088PWM_GET_CLASS(s);

    pwmc->parent_reset(dev);

    s->pwm_MR4 = 0;
    s->pwm_MR5 = 0;
    s->pwm_MR6 = 0;
    s->pwm_PCR = 0;
    s->pwm_LER = 0;

    size_t i;
    for(i = 0; i < 7; i++) {
        s->pwm_shadow_mr[i] = 0;
    }
}

static void lpc4088_pwm_remote_ctrl_send_register(LPC4088PWMState *s, hwaddr offset, uint32_t val) {
    if(s->enable_rc) {
        LPC4088TimerState *ts = LPC4088TIMER(s);
        RemoteCtrlClass *rcc = REMOTE_CTRL_GET_CLASS(&ts->rcs);

        RemoteCtrlMessage msg = {
            .magic = REMOTE_CTRL_PWM_MAGIC,
            .cmd = 0,
            .arg1 = s->pwm_name[0],
            .arg2 = offset,
            .arg3 = val,
            .arg4 = 0,
            .arg5 = 0,
            .arg6 = 0
        };

        rcc->send_message(&ts->rcs, (void *)&msg, sizeof(RemoteCtrlMessage));
    }
}

static uint64_t lpc4088_pwm_read(void *opaque, hwaddr offset, unsigned size) {
    LPC4088PWMState *s = opaque;
    LPC4088TimerState *ts = LPC4088TIMER(opaque);
    uint32_t read_val;
    
    //int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    const MemoryRegionOps *ops = s->parent_mem_ops;

    

    switch (offset) {
    case LPC4088_PWM_REG_IR:
    case LPC4088_PWM_REG_TCR:
    case LPC4088_PWM_REG_TC:
    case LPC4088_PWM_REG_PR:
    case LPC4088_PWM_REG_PC:
    case LPC4088_PWM_REG_MCR:
    case LPC4088_PWM_REG_CCR:
    case LPC4088_PWM_REG_CC0:
    case LPC4088_PWM_REG_CC1:
    case LPC4088_PWM_REG_CTCR:
        read_val = ops->read(opaque, offset, size);
    case LPC4088_PWM_REG_MR0:
    case LPC4088_PWM_REG_MR1:
    case LPC4088_PWM_REG_MR2:
    case LPC4088_PWM_REG_MR3:
        if(ts->timer_TCR & (1 << 3)) {
            read_val = s->pwm_shadow_mr[(offset - 0x18) / 4];
        } else {
            read_val = ops->read(opaque, offset, size);
        }
    case LPC4088_PWM_REG_MR4:
        if(ts->timer_TCR & (1 << 3)) {
            read_val = s->pwm_shadow_mr[4];
        } else {
            read_val = s->pwm_MR4;
        }
    case LPC4088_PWM_REG_MR5:
        if(ts->timer_TCR & (1 << 3)) {
            read_val = s->pwm_shadow_mr[5];
        } else {
            read_val = s->pwm_MR5;
        }
    case LPC4088_PWM_REG_MR6:
        if(ts->timer_TCR & (1 << 3)) {
            read_val = s->pwm_shadow_mr[6];
        } else {
            read_val = s->pwm_MR6;
        }
    case LPC4088_PWM_REG_PCR:
        read_val = s->pwm_PCR;
    case LPC4088_PWM_REG_LER:
        read_val = s->pwm_LER;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
    }

    DEBUG_PRINT("[Read from PWM%c] offset: 0x%" PRIx64 " value: 0x%" PRIx32 "\n", s->pwm_name[0], offset, read_val);
    return 0;
}

static void lpc4088_pwm_write(void *opaque, hwaddr offset, uint64_t val64, unsigned size) {
    LPC4088PWMState *s = LPC4088PWM(opaque);
    LPC4088TimerState *ts = LPC4088TIMER(opaque);

    const MemoryRegionOps *ops = s->parent_mem_ops;

    uint32_t value = (uint32_t) val64;
    DEBUG_PRINT("[Write to PWM%c] offset: 0x%" PRIx64 " value: 0x%" PRIx32 "\n", s->pwm_name[0], offset, value);
    
    lpc4088_pwm_remote_ctrl_send_register(s, offset, value);

    switch (offset) {
    case LPC4088_PWM_REG_IR:
    
    case LPC4088_PWM_REG_TC:
    case LPC4088_PWM_REG_PR:
    case LPC4088_PWM_REG_PC:
    case LPC4088_PWM_REG_MCR:
    case LPC4088_PWM_REG_CCR:
    case LPC4088_PWM_REG_CC0:
    case LPC4088_PWM_REG_CC1:
    case LPC4088_PWM_REG_CTCR:
        ops->write(opaque, offset, val64, size);
        return;
    case LPC4088_PWM_REG_TCR:
        if(!(ts->timer_TCR & (1 << 3)) && (value & (1 << 3))) {
            s->pwm_shadow_mr[0] = ts->timer_MR0;
            s->pwm_shadow_mr[1] = ts->timer_MR1;
            s->pwm_shadow_mr[2] = ts->timer_MR2;
            s->pwm_shadow_mr[3] = ts->timer_MR3;

            s->pwm_shadow_mr[4] = s->pwm_MR4;
            s->pwm_shadow_mr[5] = s->pwm_MR5;
            s->pwm_shadow_mr[6] = s->pwm_MR6;
        } else {
            ops->write(opaque, offset, val64, size);
        }
        return;
    case LPC4088_PWM_REG_MR0:
    case LPC4088_PWM_REG_MR1:
    case LPC4088_PWM_REG_MR2:
    case LPC4088_PWM_REG_MR3:
        if(ts->timer_TCR & (1 << 3)) {
            s->pwm_shadow_mr[(offset - 0x18) / 4] = value;
        } else {
            ops->write(opaque, offset, val64, size);
        }
        return;
    case LPC4088_PWM_REG_MR4:
        if(ts->timer_TCR & (1 << 3)) {
            s->pwm_shadow_mr[4] = value;
        } else {
            s->pwm_MR4 = value;
        }
        return;
    case LPC4088_PWM_REG_MR5:
        if(ts->timer_TCR & (1 << 3)) {
            s->pwm_shadow_mr[5] = value;
        } else {
            s->pwm_MR5 = value;
        }
        return;
    case LPC4088_PWM_REG_MR6:
        if(ts->timer_TCR & (1 << 3)) {
            s->pwm_shadow_mr[6] = value;
        } else {
            s->pwm_MR6 = value;
        }
        return;
    case LPC4088_PWM_REG_PCR:
        s->pwm_PCR = value;
        return;
    case LPC4088_PWM_REG_LER:
        if(ts->timer_TCR & (1 << 3)) {
            if(s->pwm_LER & 0x1) ts->timer_MR0 = s->pwm_shadow_mr[0];
            if(s->pwm_LER & 0x2) ts->timer_MR1 = s->pwm_shadow_mr[1];
            if(s->pwm_LER & 0x4) ts->timer_MR2 = s->pwm_shadow_mr[2];
            if(s->pwm_LER & 0x8) ts->timer_MR3 = s->pwm_shadow_mr[3];
            if(s->pwm_LER & 0x10) s->pwm_MR4 = s->pwm_shadow_mr[4];
            if(s->pwm_LER & 0x20) s->pwm_MR5 = s->pwm_shadow_mr[5];
            if(s->pwm_LER & 0x40) s->pwm_MR6 = s->pwm_shadow_mr[6];
        }
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
        return;
    }
}

static void lpc4088_pwm_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg) {
    LPC4088PWMState *s = LPC4088PWM(rcs->connected_device);
    
    if(s->enable_rc) {
        if(msg->cmd == REMOTE_CTRL_PWM_CMD_SEND_CAPTURE) {
            if(s->pwm_name[0] == msg->arg1) {
                lpc4088_timer_do_simulate_capture(LPC4088TIMER(s), msg->arg2, msg->arg3);
            }
        }
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
        VMSTATE_UINT32(pwm_MR4, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MR5, LPC4088PWMState),
        VMSTATE_UINT32(pwm_MR6, LPC4088PWMState),
        VMSTATE_UINT32(pwm_PCR, LPC4088PWMState),
        VMSTATE_UINT32(pwm_LER, LPC4088PWMState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_pwm_properties[] = {
    DEFINE_PROP_STRING("pwm-name", LPC4088PWMState, pwm_name),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_pwm_init(Object *obj) {
}

static void lpc4088_pwm_realize(DeviceState *dev, Error **errp) {
    LPC4088TimerState *ts = LPC4088TIMER(dev);
    LPC4088PWMState *s = LPC4088PWM(dev);
    LPC4088PWMClass *pwmc = LPC4088PWM_GET_CLASS(dev);

    ts->timer_name = s->pwm_name;
    ts->rcs.callback = lpc4088_pwm_remote_ctrl_callback;
    ts->check_syscon = false;

    s->enable_rc = ts->enable_rc;
    ts->enable_rc = false;

    pwmc->parent_realize(dev, errp);

    s->parent_mem_ops = ts->iomem.ops;

	memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_pwm_ops, s, TYPE_LPC4088_PWM, LPC4088_PWM_MEM_SIZE);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);	
    ts->enable_debug_print = false;
}

static void lpc4088_pwm_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);
    LPC4088TimerClass *tc = LPC4088TIMER_CLASS(klass);
    LPC4088PWMClass *pwmc = LPC4088PWM_CLASS(klass);

    pwmc->parent_reset = dc->reset;
    pwmc->parent_realize = dc->realize;

    size_t i;
    for(i = 0; i < 3; i++) {
        pwmc->parent_match_events[i] = tc->match_events[i];
    }
    tc->match_events[0] = lpc4088_pwm_match_event0;

    dc->reset = lpc4088_pwm_reset;
    device_class_set_props(dc, lpc4088_pwm_properties);
    dc->vmsd = &vmstate_lpc4088_pwm;
    dc->realize = lpc4088_pwm_realize;
}

static const TypeInfo lpc4088_pwm_info = {
    .name          = TYPE_LPC4088_PWM,
    .parent        = TYPE_LPC4088_TIMER,
    .instance_size = sizeof(LPC4088PWMState),
    .instance_init = lpc4088_pwm_init,
    .class_size    = sizeof(LPC4088PWMClass),
    .class_init    = lpc4088_pwm_class_init,
};

static void lpc4088_pwm_register_types(void) {
    type_register_static(&lpc4088_pwm_info);
}

type_init(lpc4088_pwm_register_types)
