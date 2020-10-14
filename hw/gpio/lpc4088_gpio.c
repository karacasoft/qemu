/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#include "qemu/osdep.h"
#include "hw/gpio/lpc4088_gpio.h"
#include "migration/vmstate.h"
#include "hw/irq.h"
#include "hw/qdev-core.h"
#include "hw/qdev-properties.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "trace.h"

#ifndef DEBUG_LPC4088_GPIO
#define DEBUG_LPC4088_GPIO 0
#endif

#define REMOTE_CTRL_GPIO_MAGIC 0xDEADBEE2
#define REMOTE_CTRL_CMD_GPIO_SET_PORT_PIN   0x00000100
#define REMOTE_CTRL_CMD_GPIO_SET_PORT_VALUE 0x00000101
#define REMOTE_CTRL_CMD_GPIO_CLR_PORT_PIN   0x00000200
#define REMOTE_CTRL_CMD_GPIO_STATUS         0x00000300

#define DPRINTF(fmt, args...) \
    if(DEBUG_LPC4088_GPIO) { \
        fprintf(stderr, "[%s]%s: " fmt, TYPE_LPC4088_GPIO_PORT, \
                __func__, ##args); \
    }

static const char *lpc4088_gpio_reg_name(uint32_t reg)
{
    switch (reg) {
    case DIR_ADDR:
        return "DIR";
    case MASK_ADDR:
        return "MASK";
    case PIN_ADDR:
        return "PIN";
    case SET_ADDR:
        return "SET";
    case CLR_ADDR:
        return "CLR";
    default:
        return "[?]";
    }
}

static void lpc4088_gpio_set(void *opaque, int line, int level)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(opaque);

    if((s->dir & (1 << line)) == 0)
    {
        if(level)
        {
            s->pin |= (1 << line);
        }
        else
        {
            s->pin &= ~(1 << line);
        }
        // TODO trigger interrupts
    }

    
}

static inline void lpc4088_gpio_set_all_output_lines(LPC4088GPIOPortState *s)
{
    int i;
    for (i = 0; i < LPC4088_GPIO_PORT_PIN_COUNT; i++)
    {
        if((s->dir & (1 << i)) && s->output[i])
        {
            qemu_set_irq(s->output[i], !!(s->pin & (1 < i)));
        }
    }
}

static inline void lpc4088_remote_ctrl_send_status(LPC4088GPIOPortState *s)
{
    
    RemoteCtrlMessage *msg = g_new(RemoteCtrlMessage, 1);

    msg->magic = REMOTE_CTRL_GPIO_MAGIC;
    msg->cmd = 0;
    msg->arg1 = s->port_name[0];
    msg->arg2 = s->dir;
    msg->arg3 = s->mask;
    msg->arg4 = s->pin;

    remote_ctrl_send_message(&s->rcs, (void *)msg, sizeof(RemoteCtrlMessage));
    g_free(msg);
}

static uint64_t lpc4088_gpio_read(void *opaque, hwaddr offset, unsigned size)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(opaque);
    uint32_t reg_value = 0;

    switch (offset) {
    case DIR_ADDR:
        reg_value = s->dir;
        break;
    case MASK_ADDR:
        reg_value = s->mask;
        break;
    case PIN_ADDR:
        reg_value = s->pin;
        break;
    case SET_ADDR:
        reg_value = s->pin;
        break;
    case CLR_ADDR:
        reg_value = 0;
        qemu_log_mask(LOG_GUEST_ERROR, "[%s]%s: CLRx register is write-only\n",
                      TYPE_LPC4088_GPIO_PORT, __func__);
        break;
    default:
        reg_value = 0;
        qemu_log_mask(LOG_GUEST_ERROR, "[%s]%s: Bad register at offset 0x%"
                      HWADDR_PRIx "\n", TYPE_LPC4088_GPIO_PORT, __func__, offset);
        break;
    }

    trace_lpc4088_gpio_read(offset, reg_value);
    DPRINTF("(%s) = 0x%" PRIx32 "\n", lpc4088_gpio_reg_name(offset), reg_value);

    return reg_value;
}

static void lpc4088_gpio_write(void *opaque, hwaddr offset, uint64_t value,
                               unsigned size)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(opaque);

    DPRINTF("(%s, value = 0x%" PRIx32 ")\n", lpc4088_gpio_reg_name(offset),
            (uint32_t) value);

    
    
    trace_lpc4088_gpio_write(offset, value);

    switch (offset) {
    case DIR_ADDR:
        s->dir = value;
        break;
    case MASK_ADDR:
        s->mask = value;
        break;
    case PIN_ADDR:
        s->pin |= value & (~(s->mask)) & s->dir;
        s->pin &= ~((~value) & (~(s->mask)) & s->dir);
        lpc4088_gpio_set_all_output_lines(s);
        break;
    case SET_ADDR:
        s->pin |= (value & (~(s->mask)) & s->dir);
        lpc4088_gpio_set_all_output_lines(s);
        break;
    case CLR_ADDR:
        s->pin &= ~(value & (~(s->mask)) & s->dir);
        lpc4088_gpio_set_all_output_lines(s);
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "[%s]%s: Bad register at offset 0x%"
                      HWADDR_PRIx "\n", TYPE_LPC4088_GPIO_PORT, __func__, offset);
        break;
    }
    lpc4088_remote_ctrl_send_status(s);

    return;
}

static bool lpc4088_gpio_memory_op_accepts(
            void *opaque, hwaddr addr,
            unsigned size, bool is_write,
            MemTxAttrs attrs) {
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(opaque);

    if(!(s->sc->sc_PCONP & LPC4088_PCONP_GPIO_MASK)) {
        qemu_irq_pulse(s->sc->hard_fault_irq);
        DPRINTF("wowow\n");
        return false;
    }
    return true;
}

static const MemoryRegionOps lpc4088_gpio_ops = {
    .read = lpc4088_gpio_read,
    .write = lpc4088_gpio_write,
    .valid.min_access_size = 4,
    .valid.max_access_size = 4,
    .valid.accepts = lpc4088_gpio_memory_op_accepts,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_lpc4088_gpio = {
    .name = TYPE_LPC4088_GPIO_PORT,
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField []) {
        VMSTATE_UINT32(dir, LPC4088GPIOPortState),
        VMSTATE_UINT32(mask, LPC4088GPIOPortState),
        VMSTATE_UINT32(pin, LPC4088GPIOPortState),
        VMSTATE_BOOL(enable_rc, LPC4088GPIOPortState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_gpio_properties[] = {
    DEFINE_PROP_STRING("port-name", LPC4088GPIOPortState, port_name),
    DEFINE_PROP_BOOL("enable-rc", LPC4088GPIOPortState, enable_rc, false),
    DEFINE_PROP_LINK("syscon", LPC4088GPIOPortState, sc, TYPE_LPC4088_SC, LPC4088SCState *),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_gpio_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(rcs->connected_device);
    if(s->enable_rc)
    {
        if(msg->cmd == REMOTE_CTRL_CMD_GPIO_SET_PORT_PIN)
        {
            if(msg->arg1 == s->port_name[0] && msg->arg2 < 32)
            {
                DPRINTF("%s: Write request from remote to PORT%s\n"
                        "set pin %d\n", __func__,
                        s->port_name,
                        msg->arg2
                    );
                qemu_irq irq = qdev_get_gpio_in(DEVICE(s), msg->arg2);
                qemu_irq_raise(irq);
            }
        }
        else if(msg->cmd == REMOTE_CTRL_CMD_GPIO_CLR_PORT_PIN)
        {
            if(msg->arg1 == s->port_name[0] && msg->arg2 < 32)
            {
                DPRINTF("%s: Write request from remote to PORT%s\n"
                        "clr pin %d\n", __func__,
                        s->port_name,
                        msg->arg2
                    );
                qemu_irq irq = qdev_get_gpio_in(DEVICE(s), msg->arg2);
                qemu_irq_lower(irq);
            }
        }
        else if(msg->cmd == REMOTE_CTRL_CMD_GPIO_STATUS)
        {
            lpc4088_remote_ctrl_send_status(s);
        }
    }
}

static void lpc4088_gpio_reset(DeviceState *dev)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(dev);

    s->dir = 0;
    s->mask = 0;
    s->pin = 0;
    lpc4088_gpio_set_all_output_lines(s);
    // TODO interrupt initializations
}

static void lpc4088_gpio_realize(DeviceState *dev, Error **errp)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_gpio_ops,
                          s, TYPE_LPC4088_GPIO_PORT,
                          LPC4088_GPIO_PORT_MEM_SIZE);
    
    qdev_init_gpio_in(DEVICE(s), lpc4088_gpio_set, LPC4088_GPIO_PORT_PIN_COUNT);
    qdev_init_gpio_out(DEVICE(s), s->output, LPC4088_GPIO_PORT_PIN_COUNT);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

    s->rcs.callback = lpc4088_gpio_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), NULL, NULL);
    
}

static void lpc4088_gpio_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = lpc4088_gpio_realize;
    dc->reset = lpc4088_gpio_reset;
    device_class_set_props(dc, lpc4088_gpio_properties);
    dc->vmsd = &vmstate_lpc4088_gpio;
    dc->desc = "LPC4088 GPIO controller";
}

static void lpc4088_gpio_instance_init(Object *obj)
{
    LPC4088GPIOPortState *s = LPC4088_GPIO_PORT(obj);
    DeviceState *ds = DEVICE(obj);
    
    object_initialize_child_with_props(
        obj, "gpio-rcs", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, &error_abort,
        NULL
    );

    s->rcs.connected_device = ds;
}

static const TypeInfo lpc4088_gpio_info = {
    .name = TYPE_LPC4088_GPIO_PORT,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088GPIOPortState),
    .class_init = lpc4088_gpio_class_init,
    .instance_init = lpc4088_gpio_instance_init,
};

static void lpc4088_gpio_register_types(void)
{
    type_register_static(&lpc4088_gpio_info);
}

type_init(lpc4088_gpio_register_types);
