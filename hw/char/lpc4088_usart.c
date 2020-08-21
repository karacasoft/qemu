#include "qemu/osdep.h"
#include "hw/char/lpc4088_usart.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef LPC4088_USART_ERR_DEBUG
#define LPC4088_USART_ERR_DEBUG 1
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (LPC4088_USART_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

#define DPRINTF2(fmt, args...) \
    if(LPC4088_USART_ERR_DEBUG) { \
        fprintf(stderr, "[%s]%s: " fmt, TYPE_LPC4088_USART, \
                __func__, ##args); \
    }
	
static int lpc4088_usart_can_receive(void *opaque)
{
    LPC4088USARTState *s = opaque;

    if (!(s->usart_RBR)) {
        return 1;
    }

    return 0;
}

static void lpc4088_usart_receive(void *opaque, const uint8_t *buf, int size)
{
    LPC4088USARTState *s = opaque;

    s->usart_RBR = *buf;

    qemu_set_irq(s->irq, 1);

    DB_PRINT("Receiving: %c\n", s->usart_RBR);
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Deneme Uart",
            (uint32_t) s->usart_RBR);
}

static void lpc4088_usart_reset(DeviceState *dev)
{
    LPC4088USARTState *s = LPC4088USART(dev);


	s->usart_RBR = 0x00000000;
	s->usart_THR = 0x00000000;
	s->usart_DLL = 0x00000000;

	s->usart_DLM = 0x00000000;
	s->usart_IER = 0x00000000;

	s->usart_IIR = 0x00000000;
	s->usart_FCR = 0x00000000;

	s->usart_LCR = 0x00000000;
	s->usart_LSR = 0x00000000;
	s->usart_SCR = 0x00000000;
	s->usart_ACR = 0x00000000;
	s->usart_FDR = 0x00000000;
	s->usart_TER = 0x00000000;
	s->usart_RS485CTRL = 0x00000000;
	s->usart_RS485ADRMATCH = 0x00000000;
	s->usart_RS485DLY = 0x00000000;

    qemu_set_irq(s->irq, 0);
}

static uint64_t lpc4088_usart_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
    LPC4088USARTState *s = opaque;
    uint64_t retvalue;

    DB_PRINT("Read 0x%"HWADDR_PRIx"\n", addr);
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Deneme Uart",
            (uint32_t) s->usart_RBR);

    switch (addr) {
	case LPC4088_USART_REG_RBR:
        retvalue = s->usart_RBR;
        qemu_chr_fe_accept_input(&s->chr);
        return retvalue;
	//case LPC4088_USART_REG_THR:
    //    retvalue = s->usart_THR;
    //    qemu_chr_fe_accept_input(&s->chr);
	//	qemu_set_irq(s->irq, 0);
    //    return retvalue;	
    //case LPC4088_USART_REG_DLL:
    //    return s->usart_DLL;
		
    case LPC4088_USART_REG_DLM:
        return s->usart_DLM;
    //case LPC4088_USART_REG_IER:
    //    return s->usart_IER;
		
    case LPC4088_USART_REG_IIR:
        return s->usart_IIR;
    //case LPC4088_USART_REG_FCR:
    //    return s->usart_FCR;
		
	case LPC4088_USART_REG_LCR:
        return s->usart_LCR;
	case LPC4088_USART_REG_LSR:
        return s->usart_LSR;
	case LPC4088_USART_REG_SCR:
        return s->usart_SCR;
	case LPC4088_USART_REG_ACR:
        return s->usart_ACR;
	case LPC4088_USART_REG_FDR:
        return s->usart_FDR;
	case LPC4088_USART_REG_TER:
        return s->usart_TER;
		
	case LPC4088_USART_REG_RS485CTRL:
        return s->usart_RS485CTRL;
	case LPC4088_USART_REG_RS485ADRMATCH:
        return s->usart_RS485ADRMATCH;
	case LPC4088_USART_REG_RS485DLY:
        return s->usart_RS485DLY;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, addr);
        return 0;
    }

    return 0;
}

static void lpc4088_usart_write(void *opaque, hwaddr addr,
                                  uint64_t val64, unsigned int size)
{
    LPC4088USARTState *s = opaque;
    uint32_t value = val64;
    unsigned char ch;

    DB_PRINT("Write Us 0x%" PRIx32 ", 0x%"HWADDR_PRIx"\n", value, addr);
	
	DPRINTF2("(%s, value = 0x%" PRIx32 ")\n","Deneme Uart",
            (uint32_t) value);

    switch (addr) {
    case LPC4088_USART_REG_RBR:
		ch = value;
		qemu_chr_fe_write_all(&s->chr, &ch, 1);
        qemu_set_irq(s->irq, 0);
        return;
    //case LPC4088_USART_REG_THR:
	//	ch = value;
	//	qemu_chr_fe_write_all(&s->chr, &ch, 1);
    //    return;
    //case LPC4088_USART_REG_DLL:
    //    s->usart_DLL = value;
    //    return;
		
	case LPC4088_USART_REG_DLM:
        s->usart_DLM = value;
        return;
	//case LPC4088_USART_REG_IER:
    //    s->usart_IER = value;
    //    return;
		
	case LPC4088_USART_REG_IIR:
        s->usart_IIR = value;
        return;
	//case LPC4088_USART_REG_FCR:
    //    s->usart_FCR = value;
    //    return;
		
	case LPC4088_USART_REG_LCR:
        s->usart_LCR = value;
        return;
	case LPC4088_USART_REG_LSR:
        s->usart_LSR = value;
        return;
	case LPC4088_USART_REG_SCR:
        s->usart_SCR = value;
        return;
	case LPC4088_USART_REG_ACR:
        s->usart_ACR = value;
        return;
	case LPC4088_USART_REG_FDR:
        s->usart_FDR = value;
        return;
	case LPC4088_USART_REG_TER:
        s->usart_TER = value;
        return;
		
	case LPC4088_USART_REG_RS485CTRL:
        s->usart_RS485CTRL = value;
        return;
	case LPC4088_USART_REG_RS485ADRMATCH:
        s->usart_RS485ADRMATCH = value;
        return;
	case LPC4088_USART_REG_RS485DLY:
        s->usart_RS485DLY = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, addr);
    }
}

static const MemoryRegionOps lpc4088_usart_ops = {
    .read = lpc4088_usart_read,
    .write = lpc4088_usart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property lpc4088_usart_properties[] = {
    DEFINE_PROP_CHR("chardev", LPC4088USARTState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_usart_init(Object *obj)
{			
    LPC4088USARTState *s = LPC4088USART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &lpc4088_usart_ops, s,
                          TYPE_LPC4088_USART, LPC4088_USART_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void lpc4088_usart_realize(DeviceState *dev, Error **errp)
{
    LPC4088USARTState *s = LPC4088USART(dev);

    qemu_chr_fe_set_handlers(&s->chr, lpc4088_usart_can_receive,
                             lpc4088_usart_receive, NULL, NULL,
                             s, NULL, true);
}

static void lpc4088_usart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_usart_reset;
    device_class_set_props(dc, lpc4088_usart_properties);
    dc->realize = lpc4088_usart_realize;
}

static const TypeInfo lpc4088_usart_info = {
    .name          = TYPE_LPC4088_USART,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088USARTState),
    .instance_init = lpc4088_usart_init,
    .class_init    = lpc4088_usart_class_init,
};

static void lpc4088_usart_register_types(void)
{
    type_register_static(&lpc4088_usart_info);
}

type_init(lpc4088_usart_register_types)
