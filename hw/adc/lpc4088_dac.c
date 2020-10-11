#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/adc/lpc4088_dac.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"

#ifndef LPC4088_DAC_ERROR_DEBUG
#define LPC4088_DAC_ERROR_DEBUG 0
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_DAC_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_DAC,__func__, ##args);}

#define REMOTE_CTRL_DAC_MAGIC 0xDEEDBEE4

static const char *lpc4088_dac_register_name(uint32_t offset) {
	switch (offset) {
    case LPC4088_DAC_REG_CR:
        return "DAC_CR";
    case LPC4088_DAC_REG_CTRL:
        return "DAC_CTRL";
	case LPC4088_DAC_REG_CNTVAL:
        return "DAC_CNTVAL";
    default:
        return "DAC_[?]";
    }
}

static void lpc4088_dac_reset(DeviceState *dev) {
    LPC4088DACState *s = LPC4088DAC(dev);

	s->dac_CR = 0x00000000;
    s->dac_CTRL = 0x00000000;
    s->dac_CNTVAL = 0x00000000;
	
	s->enableRemoteInterrupt = 0;
}

static uint64_t lpc4088_dac_read(void *opaque, hwaddr offset, unsigned int size) {
    LPC4088DACState *s = opaque;

    switch (offset) {
    case LPC4088_DAC_REG_CR:
        return s->dac_CR;
    case LPC4088_DAC_REG_CTRL:
        return s->dac_CTRL;
	case LPC4088_DAC_REG_CNTVAL:
        return s->dac_CNTVAL;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, offset);
    }

    return 0;
}

static void lpc4088_dac_write(void *opaque, hwaddr offset, uint64_t val64, unsigned int size) {
    LPC4088DACState *s = opaque;
    uint32_t value = (uint32_t) val64;
	
	DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_dac_register_name(offset), (uint32_t) offset);

    switch (offset) {
    case LPC4088_DAC_REG_CR:
        s->dac_CR = value;
        break;
    case LPC4088_DAC_REG_CTRL:
        s->dac_CTRL = value;
        break;
	case LPC4088_DAC_REG_CNTVAL:
        s->dac_CNTVAL = value;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, offset);
    }
}

static void lpc4088_dac_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg) {
    LPC4088DACState *s = LPC4088DAC(rcs->connected_device);
    
	if(s->enable_rc) {
		if(msg->arg1 == s->dac_name[0] && msg->arg2 < 32) {
		}
    }
}

static const MemoryRegionOps lpc4088_dac_ops = {
    .read = lpc4088_dac_read,
    .write = lpc4088_dac_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .impl.min_access_size = 4,
    .impl.max_access_size = 4,
};

static const VMStateDescription vmstate_lpc4088_dac = {
    .name = TYPE_LPC4088_DAC,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(dac_CR, LPC4088DACState),
        VMSTATE_UINT32(dac_CTRL, LPC4088DACState),
        VMSTATE_UINT32(dac_CNTVAL, LPC4088DACState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_dac_properties[] = {
	DEFINE_PROP_STRING("dac-name", struct LPC4088DACState, dac_name),
    DEFINE_PROP_BOOL("enable-rc", struct LPC4088DACState, enable_rc, false),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_dac_init(Object *obj) {
    LPC4088DACState *s = LPC4088DAC(obj);
	
	DeviceState *ds = DEVICE(obj);
	
	object_initialize_child_with_props(
        obj, "RemoteCtrl", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, &error_abort,
        NULL
    );

    s->rcs.connected_device = ds;

    /*sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &lpc4088_dac_ops, s,TYPE_LPC4088_DAC, LPC4088_DAC_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);*/
}

static void lpc4088_dac_realize(DeviceState *dev, Error **errp) {
    LPC4088DACState *s = LPC4088DAC(dev);

	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);
	
	memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_dac_ops, s, TYPE_LPC4088_DAC, LPC4088_DAC_MEM_SIZE);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

    s->rcs.callback = lpc4088_dac_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), qdev_get_parent_bus(DEVICE(&s->rcs)), errp);
}

static void lpc4088_dac_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_dac_reset;
	device_class_set_props(dc, lpc4088_dac_properties);
    dc->vmsd = &vmstate_lpc4088_dac;
	dc->realize = lpc4088_dac_realize;
}

static const TypeInfo lpc4088_dac_info = {
    .name          = TYPE_LPC4088_DAC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088DACState),
    .instance_init = lpc4088_dac_init,
    .class_init    = lpc4088_dac_class_init,
};

static void lpc4088_dac_register_types(void) {
    type_register_static(&lpc4088_dac_info);
}

type_init(lpc4088_dac_register_types)
