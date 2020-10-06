#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/misc/lpc4088_sc.h"

#ifndef LPC4088_SC_ERROR_DEBUG
#define LPC4088_SC_ERROR_DEBUG 0
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_SC_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_SC,__func__, ##args);}

static void lpc4088_sc_reset(DeviceState *dev) {
    LPC4088SCState *s = LPC4088SC(dev);
	
	s->pll0_feed_state = PLL_FEED_STATE_DEFAULT;
	s->pll1_feed_state = PLL_FEED_STATE_DEFAULT;

	s->sc_FLASHCFG = 0x00000000;
	
	s->sc_PLL0CON = 0x00000000;
	s->sc_PLL0CFG = 0x00000000;
	s->sc_PLL0STAT = 0x00000000;
	
	
	s->sc_PLL1CON = 0x00000000;
	s->sc_PLL1CFG = 0x00000000;
	s->sc_PLL1STAT = 0x00000000;
	
	s->sc_PCON = 0x00000000;
	s->sc_PCONP = 0x408829E;
	s->sc_PCONP1 = 0x8;
	
	s->sc_EMCCLKSEL = 0x00000000;
	s->sc_CCLKSEL = 0x1;
	s->sc_USBCLKSEL = 0x00000000;
	s->sc_CLKSRCSEL = 0x00000000;

	s->sc_CANSLEEPCLR = 0x00000000;
	s->sc_CANWAKEFLAGS = 0x00000000;
	
	s->sc_EXTINT = 0x00000000;
	s->sc_EXTMODE = 0x00000000;
	s->sc_EXTPOLAR = 0x00000000;
	
	s->sc_RSID = 0x00000000;
	
	s->sc_SCS = 0x00000000;
	s->sc_IRCCTRL = 0x80;
	s->sc_IRCTRIM = 0x00000000;
	s->sc_PCLKSEL = 0x4;
	
	s->sc_PBOOST = 0x3;
	s->sc_SPIFICLKSEL = 0x00000000;
	s->sc_LCD_CFG = 0x00000000;
	
	s->sc_USBIntSt = 0x80000000;
	s->sc_DMAREQSEL = 0x00000000;
	s->sc_CLKOUTCFG = 0x00000000;
	s->sc_RSTCON0 = 0x00000000;
	s->sc_RSTCON1 = 0x00000000;

	s->sc_EMCDLYCTL = 0x210;
	s->sc_EMCCALd = 0x1F00;
}

static uint64_t lpc4088_sc_read(void *opaque, hwaddr addr, unsigned int size) {
    LPC4088SCState *s = opaque;

    switch (addr) {
    case LPC4088_SC_FLASHCFG:
		return s->sc_FLASHCFG;
	case LPC4088_SC_REG_PLL0CON:
		return s->sc_PLL0CON;
	case LPC4088_SC_REG_PLL0CFG:
		return s->sc_PLL0CFG;
	case LPC4088_SC_REG_PLL0STAT:
		return s->sc_PLL0STAT;
	case LPC4088_SC_REG_PLL0FEED:
		return s->sc_PLL0FEED;
	case LPC4088_SC_REG_PLL1CON:
		return s->sc_PLL1CON;
	case LPC4088_SC_REG_PLL1CFG:
		return s->sc_PLL1CFG;
	case LPC4088_SC_REG_PLL1STAT:
		return s->sc_PLL1STAT;
	case LPC4088_SC_REG_PLL1FEED:
		return s->sc_PLL1FEED;
	case LPC4088_SC_REG_PCON:
		return s->sc_PCON;
	case LPC4088_SC_REG_PCONP:
		return s->sc_PCONP;
	case LPC4088_SC_REG_PCONP1:
		return s->sc_PCONP1;
	case LPC4088_SC_REG_EMCCLKSEL:
		return s->sc_EMCCLKSEL;
	case LPC4088_SC_REG_CCLKSEL:
		return s->sc_CCLKSEL;
	case LPC4088_SC_REG_USBCLKSEL:
		return s->sc_USBCLKSEL;
	case LPC4088_SC_REG_CLKSRCSEL:
		return s->sc_CLKSRCSEL;
	case LPC4088_SC_REG_CANSLEEPCLR:
		return s->sc_CANSLEEPCLR;
	case LPC4088_SC_REG_CANWAKEFLAGS:
		return s->sc_CANWAKEFLAGS;
	case LPC4088_SC_REG_EXTINT:
		return s->sc_EXTINT;
	case LPC4088_SC_REG_EXTMODE:
		return s->sc_EXTMODE;
	case LPC4088_SC_REG_EXTPOLAR:
		return s->sc_EXTPOLAR;
	case LPC4088_SC_REG_RSID:
		return s->sc_RSID;
	case LPC4088_SC_REG_SCS:
		return s->sc_SCS;
	case LPC4088_SC_REG_IRCTRIM:
		return s->sc_IRCTRIM;
	case LPC4088_SC_REG_PCLKSEL:
		return s->sc_PCLKSEL;
	case LPC4088_SC_REG_PBOOST:
		return s->sc_PBOOST;
	case LPC4088_SC_REG_SPIFICLKSEL:
		return s->sc_SPIFICLKSEL;
	case LPC4088_SC_REG_LCD_CFG:
		return s->sc_LCD_CFG;
	case LPC4088_SC_REG_USBIntSt:
		return s->sc_USBIntSt;
	case LPC4088_SC_REG_DMAREQSEL:
		return s->sc_DMAREQSEL;
	case LPC4088_SC_REG_CLKOUTCFG:
		return s->sc_CLKOUTCFG;
	case LPC4088_SC_REG_RSTCON0:
		return s->sc_RSTCON0;
	case LPC4088_SC_REG_RSTCON1:
		return s->sc_RSTCON1;
	case LPC4088_SC_REG_EMCDLYCTL:
		return s->sc_EMCDLYCTL;
	case LPC4088_SC_REG_EMCCALd:
		return s->sc_EMCCALd;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }

    return 0;
}

static void lpc4088_sc_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size) {
    LPC4088SCState *s = opaque;
    uint32_t value = (uint32_t) val64;
			 
	DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","SC_Write",(uint32_t) value);

    switch (addr) {
    case LPC4088_SC_FLASHCFG:
		s->sc_FLASHCFG = value;
		break;
	case LPC4088_SC_REG_PLL0CON:
		s->sc_PLL0CON = value;
		break;
	case LPC4088_SC_REG_PLL0CFG:
		s->sc_PLL0CFG = value;
		break;
	case LPC4088_SC_REG_PLL0STAT:
		s->sc_PLL0STAT = value;
		break;
	case LPC4088_SC_REG_PLL0FEED:
		s->sc_PLL0FEED = value;
		break;
	case LPC4088_SC_REG_PLL1CON:
		s->sc_PLL1CON = value;
		break;
	case LPC4088_SC_REG_PLL1CFG:
		s->sc_PLL1CFG = value;
		break;
	case LPC4088_SC_REG_PLL1STAT:
		s->sc_PLL1STAT = value;
		break;
	case LPC4088_SC_REG_PLL1FEED:
		s->sc_PLL1FEED = value;
		break;
	case LPC4088_SC_REG_PCON:
		s->sc_PCON = value;
		break;
	case LPC4088_SC_REG_PCONP:
		s->sc_PCONP = value;
		break;
	case LPC4088_SC_REG_PCONP1:
		s->sc_PCONP1 = value;
		break;
	case LPC4088_SC_REG_EMCCLKSEL:
		s->sc_EMCCLKSEL = value;
		break;
	case LPC4088_SC_REG_CCLKSEL:
		s->sc_CCLKSEL = value;
		break;
	case LPC4088_SC_REG_USBCLKSEL:
		s->sc_USBCLKSEL = value;
		break;
	case LPC4088_SC_REG_CLKSRCSEL:
		s->sc_CLKSRCSEL = value;
		break;
	case LPC4088_SC_REG_CANSLEEPCLR:
		s->sc_CANSLEEPCLR = value;
		break;
	case LPC4088_SC_REG_CANWAKEFLAGS:
		s->sc_CANWAKEFLAGS = value;
		break;
	case LPC4088_SC_REG_EXTINT:
		s->sc_EXTINT = value;
		break;
	case LPC4088_SC_REG_EXTMODE:
		s->sc_EXTMODE = value;
		break;
	case LPC4088_SC_REG_EXTPOLAR:
		s->sc_EXTPOLAR = value;
		break;
	case LPC4088_SC_REG_RSID:
		s->sc_RSID = value;
		break;
	case LPC4088_SC_REG_SCS:
		s->sc_SCS = value;
		break;
	case LPC4088_SC_REG_IRCTRIM:
		s->sc_IRCTRIM = value;
		break;
	case LPC4088_SC_REG_PCLKSEL:
		s->sc_PCLKSEL = value;
		break;
	case LPC4088_SC_REG_PBOOST:
		s->sc_PBOOST = value;
		break;
	case LPC4088_SC_REG_SPIFICLKSEL:
		s->sc_SPIFICLKSEL = value;
		break;
	case LPC4088_SC_REG_LCD_CFG:
		s->sc_LCD_CFG = value;
		break;
	case LPC4088_SC_REG_USBIntSt:
		s->sc_USBIntSt = value;
		break;
	case LPC4088_SC_REG_DMAREQSEL:
		s->sc_DMAREQSEL = value;
		break;
	case LPC4088_SC_REG_CLKOUTCFG:
		s->sc_CLKOUTCFG = value;
		break;
	case LPC4088_SC_REG_RSTCON0:
		s->sc_RSTCON0 = value;
		break;
	case LPC4088_SC_REG_RSTCON1:
		s->sc_RSTCON1 = value;
		break;
	case LPC4088_SC_REG_EMCDLYCTL:
		s->sc_EMCDLYCTL = value;
		break;
	case LPC4088_SC_REG_EMCCALd:
		s->sc_EMCCALd = value;
		break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps lpc4088_sc_ops = {
    .read = lpc4088_sc_read,
    .write = lpc4088_sc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .impl.min_access_size = 4,
    .impl.max_access_size = 4,
};

static const VMStateDescription vmstate_lpc4088_sc = {
    .name = TYPE_LPC4088_SC,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(sc_FLASHCFG, LPC4088SCState),
		VMSTATE_UINT32(sc_PLL0CON, LPC4088SCState),
		VMSTATE_UINT32(sc_PLL0CFG, LPC4088SCState),
		VMSTATE_UINT32(sc_PLL0STAT, LPC4088SCState),
		VMSTATE_UINT8(pll0_feed_state, LPC4088SCState),
		VMSTATE_UINT32(sc_PLL1CON, LPC4088SCState),
		VMSTATE_UINT32(sc_PLL1CFG, LPC4088SCState),
		VMSTATE_UINT32(sc_PLL1STAT, LPC4088SCState),
		VMSTATE_UINT8(pll1_feed_state, LPC4088SCState),
		VMSTATE_UINT32(sc_PCON, LPC4088SCState),
		VMSTATE_UINT32(sc_PCONP, LPC4088SCState),
		VMSTATE_UINT32(sc_PCONP1, LPC4088SCState),
		VMSTATE_UINT32(sc_EMCCLKSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_CCLKSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_USBCLKSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_CLKSRCSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_CANSLEEPCLR, LPC4088SCState),
		VMSTATE_UINT32(sc_CANWAKEFLAGS, LPC4088SCState),
		VMSTATE_UINT32(sc_EXTINT, LPC4088SCState),
		VMSTATE_UINT32(sc_EXTMODE, LPC4088SCState),
		VMSTATE_UINT32(sc_EXTPOLAR, LPC4088SCState),
		VMSTATE_UINT32(sc_RSID, LPC4088SCState),
		VMSTATE_UINT32(sc_SCS, LPC4088SCState),
		VMSTATE_UINT32(sc_IRCCTRL, LPC4088SCState),
		VMSTATE_UINT32(sc_IRCTRIM, LPC4088SCState),
		VMSTATE_UINT32(sc_PCLKSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_PBOOST, LPC4088SCState),
		VMSTATE_UINT32(sc_SPIFICLKSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_LCD_CFG, LPC4088SCState),
		VMSTATE_UINT32(sc_USBIntSt, LPC4088SCState),
		VMSTATE_UINT32(sc_DMAREQSEL, LPC4088SCState),
		VMSTATE_UINT32(sc_CLKOUTCFG, LPC4088SCState),
		VMSTATE_UINT32(sc_RSTCON0, LPC4088SCState),
		VMSTATE_UINT32(sc_RSTCON1, LPC4088SCState),
		VMSTATE_UINT32(sc_EMCDLYCTL, LPC4088SCState),
		VMSTATE_UINT32(sc_EMCCALd, LPC4088SCState),
        VMSTATE_END_OF_LIST()
    }
};

static void lpc4088_sc_init(Object *obj) {
    LPC4088SCState *s = LPC4088SC(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &lpc4088_sc_ops, s,TYPE_LPC4088_SC, LPC4088_SC_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void lpc4088_sc_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_sc_reset;
    dc->vmsd = &vmstate_lpc4088_sc;
}

static const TypeInfo lpc4088_sc_info = {
    .name          = TYPE_LPC4088_SC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088SCState),
    .instance_init = lpc4088_sc_init,
    .class_init    = lpc4088_sc_class_init,
};

static void lpc4088_sc_register_types(void) {
    type_register_static(&lpc4088_sc_info);
}

type_init(lpc4088_sc_register_types)
