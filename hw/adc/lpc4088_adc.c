#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/adc/lpc4088_adc.h"

#ifndef LPC4088_ADC_ERR_DEBUG
#define LPC4088_ADC_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (LPC4088_ADC_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

#define DPRINTF2(fmt, args...) \
    if(LPC4088_ADC_ERR_DEBUG) { \
        fprintf(stderr, "[%s]%s: " fmt, TYPE_LPC4088_ADC, \
                __func__, ##args); \
    }

static void lpc4088_adc_reset(DeviceState *dev)
{
    LPC4088ADCState *s = LPC4088ADC(dev);

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
}


// FIXME: Please remove this function if it is actually unused
static uint32_t lpc4088_adc_generate_value(LPC4088ADCState *s) __attribute__((unused));

static uint32_t lpc4088_adc_generate_value(LPC4088ADCState *s)
{
    /* Attempts to fake some ADC values */
    s->adc_DR0 = s->adc_DR0 + 7;

    return s->adc_DR0;
}

static uint64_t lpc4088_adc_read(void *opaque, hwaddr addr,
                                     unsigned int size)
{
    LPC4088ADCState *s = opaque;

    DB_PRINT("Address: 0x%" HWADDR_PRIx "\n", addr);


    switch (addr) {
    case LPC4088_ADC_REG_CR:
        return s->adc_CR;
    case LPC4088_ADC_REG_GDR:
        return s->adc_GDR;
	case LPC4088_ADC_REG_INTEN:
        return s->adc_INTEN;
	case LPC4088_ADC_REG_DR0:
        return s->adc_DR0;
	case LPC4088_ADC_REG_DR1:
        return s->adc_DR1;
	case LPC4088_ADC_REG_DR2:
        return s->adc_DR2;
	case LPC4088_ADC_REG_DR3:
        return s->adc_DR3;
	case LPC4088_ADC_REG_DR4:
        return s->adc_DR4;
	case LPC4088_ADC_REG_DR5:
        return s->adc_DR5;
	case LPC4088_ADC_REG_DR6:
        return s->adc_DR6;
	case LPC4088_ADC_REG_DR7:
        return s->adc_DR7;
	case LPC4088_ADC_REG_STAT:
        return s->adc_STAT;
	case LPC4088_ADC_REG_ADTRM:
        return s->adc_ADTRM;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }

    return 0;
}

static void lpc4088_adc_write(void *opaque, hwaddr addr,
                       uint64_t val64, unsigned int size)
{
    LPC4088ADCState *s = opaque;
    uint32_t value = (uint32_t) val64;

    DB_PRINT("Address: 0x%" HWADDR_PRIx ", Value: 0x%x\n",
             addr, value);
			 
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Deneme ADC",
            (uint32_t) value);

    switch (addr) {
    case LPC4088_ADC_REG_CR:
        s->adc_CR = value;
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
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps lpc4088_adc_ops = {
    .read = lpc4088_adc_read,
    .write = lpc4088_adc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .impl.min_access_size = 4,
    .impl.max_access_size = 4,
};

static const VMStateDescription vmstate_lpc4088_adc = {
    .name = TYPE_LPC4088_ADC,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
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

static void lpc4088_adc_init(Object *obj)
{
    LPC4088ADCState *s = LPC4088ADC(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &lpc4088_adc_ops, s,
                          TYPE_LPC4088_ADC, LPC4088_ADC_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void lpc4088_adc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_adc_reset;
    dc->vmsd = &vmstate_lpc4088_adc;
}

static const TypeInfo lpc4088_adc_info = {
    .name          = TYPE_LPC4088_ADC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088ADCState),
    .instance_init = lpc4088_adc_init,
    .class_init    = lpc4088_adc_class_init,
};

static void lpc4088_adc_register_types(void)
{
    type_register_static(&lpc4088_adc_info);
}

type_init(lpc4088_adc_register_types)
