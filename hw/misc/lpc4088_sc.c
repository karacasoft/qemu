#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/misc/lpc4088_sc.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/qdev-core.h"
#include "qapi/error.h"

#ifndef LPC4088_SC_ERROR_DEBUG
#define LPC4088_SC_ERROR_DEBUG 1
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_SC_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_SC,__func__, ##args);}

#define REMOTE_CTRL_EXT_MAGIC 0xDEEDBEE6
#define REMOTE_CTRL_SET_VALUE_EXT_PIN   0x31000100

static const char *lpc4088_sc_register_name(uint32_t offset) {
	
	switch (offset) {
    case LPC4088_SC_FLASHCFG:
		return "SC_FLASHCFG";
	case LPC4088_SC_REG_PLL0CON:
		return "SC_PLL0CON";
	case LPC4088_SC_REG_PLL0CFG:
		return "SC_PLL0CFG";
	case LPC4088_SC_REG_PLL0STAT:
		return "SC_PLL0STAT";
	case LPC4088_SC_REG_PLL0FEED:
		return "SC_PLL0FEED";
	case LPC4088_SC_REG_PLL1CON:
		return "SC_PLL1CON";
	case LPC4088_SC_REG_PLL1CFG:
		return "SC_PLL1CFG";
	case LPC4088_SC_REG_PLL1STAT:
		return "SC_PLL1STAT";
	case LPC4088_SC_REG_PLL1FEED:
		return "SC_PLL1FEED";
	case LPC4088_SC_REG_PCON:
		return "SC_PCON";
	case LPC4088_SC_REG_PCONP:
		return "SC_PCONP";
	case LPC4088_SC_REG_PCONP1:
		return "SC_PCONP1";
	case LPC4088_SC_REG_EMCCLKSEL:
		return "SC_EMCCLKSEL";
	case LPC4088_SC_REG_CCLKSEL:
		return "SC_CCLKSEL";
	case LPC4088_SC_REG_USBCLKSEL:
		return "SC_USBCLKSEL";
	case LPC4088_SC_REG_CLKSRCSEL:
		return "SC_CLKSRCSEL";
	case LPC4088_SC_REG_CANSLEEPCLR:
		return "SC_CANSLEEPCLR";
	case LPC4088_SC_REG_CANWAKEFLAGS:
		return "SC_CANWAKEFLAGS";
	case LPC4088_SC_REG_EXTINT:
		return "SC_EXTINT";
	case LPC4088_SC_REG_EXTMODE:
		return "SC_EXTMODE";
	case LPC4088_SC_REG_EXTPOLAR:
		return "SC_EXTPOLAR";
	case LPC4088_SC_REG_RSID:
		return "SC_RSID";
	case LPC4088_SC_REG_SCS:
		return "SC_SCS";
	case LPC4088_SC_REG_IRCCTRL:
		return "SC_IRCTRIM";
	case LPC4088_SC_REG_PCLKSEL:
		return "SC_PCLKSEL";
	case LPC4088_SC_REG_PBOOST:
		return "SC_PBOOST";
	case LPC4088_SC_REG_SPIFICLKSEL:
		return "SC_SPIFICLKSEL";
	case LPC4088_SC_REG_LCD_CFG:
		return "SC_LCD_CFG";
	case LPC4088_SC_REG_USBIntSt:
		return "SC_USBIntSt";
	case LPC4088_SC_REG_DMAREQSEL:
		return "SC_DMAREQSEL";
	case LPC4088_SC_REG_CLKOUTCFG:
		return "SC_CLKOUTCFG";
	case LPC4088_SC_REG_RSTCON0:
		return "SC_RSTCON0";
	case LPC4088_SC_REG_RSTCON1:
		return "SC_RSTCON1";
	case LPC4088_SC_REG_EMCDLYCTL:
		return "SC_EMCDLYCTL";
	case LPC4088_SC_REG_EMCCALd:
		return "SC_EMCCALd";
    default:
        return "SC_[?]";
    }
}

static void lpc4088_sc_timer_set_alarm(LPC4088SCState *s, int64_t now);

static void lpc4088_sc_timer_interrupt(void *opaque) {	
    LPC4088SCState *s = opaque;
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
	
	if(s->continueEXT0Interrrupt) {
		qemu_irq_pulse(s->ext0_irq);
		s->sc_EXTINT |= 1 << 0;
	}
	if(s->continueEXT1Interrrupt) {
		qemu_irq_pulse(s->ext1_irq);
		s->sc_EXTINT |= 1 << 1;
	}
	if(s->continueEXT2Interrrupt) {
		qemu_irq_pulse(s->ext2_irq);
		s->sc_EXTINT |= 1 << 2;
	}
	if(s->continueEXT3Interrrupt) {
		qemu_irq_pulse(s->ext3_irq);
		s->sc_EXTINT |= 1 << 3;
	}
	
	if(((s->sc_EXTMODE & (1 << 0)) != 0)) {
		if(((s->sc_EXTPOLAR & (1 << 0)) != 0)) {
			if(s->currentEXT0Value == 1 && s->previousEXT0Value == 0) {
				DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","SC_Interrupt", (uint32_t) 1);
				if(!s->continueEXT0Interrrupt) {
					s->continueEXT0Interrrupt = 1;
					s->sc_EXTINT |= 1 << 0;
					qemu_irq_pulse(s->ext0_irq);
				}
			}
		}
		else {
			if(s->currentEXT0Value == 0 && s->previousEXT0Value == 1) {
				if(!s->continueEXT0Interrrupt) {
					s->continueEXT0Interrrupt = 1;
					s->sc_EXTINT |= 1 << 0;
					qemu_irq_pulse(s->ext0_irq);
				}
			}
		}
	}
	else {
		if(((s->sc_EXTPOLAR & (1 << 0)) != 0)) {
			if(s->currentEXT0Value == 1) {
				if(!s->continueEXT0Interrrupt) {
					s->continueEXT0Interrrupt = 1;
					s->sc_EXTINT |= 1 << 0;
					qemu_irq_pulse(s->ext0_irq);
				}
			}
		}
	}
	
	
	if(((s->sc_EXTMODE & (1 << 1)) != 0)) {
		if(((s->sc_EXTPOLAR & (1 << 1)) != 0)) {
			if(s->currentEXT1Value == 1 && s->previousEXT1Value == 0) {
				if(!s->continueEXT1Interrrupt) {
					s->continueEXT1Interrrupt = 1;
					s->sc_EXTINT |= 1 << 1;
					qemu_irq_pulse(s->ext1_irq);
				}
			}
		}
		else {
			if(s->currentEXT1Value == 0 && s->previousEXT1Value == 1) {
				if(!s->continueEXT1Interrrupt) {
					s->continueEXT1Interrrupt = 1;
					s->sc_EXTINT |= 1 << 1;
					qemu_irq_pulse(s->ext1_irq);
				}
			}
		}
	}
	else {
		if(((s->sc_EXTPOLAR & (1 << 1)) != 0)) {
			if(s->currentEXT1Value == 1) {
				if(!s->continueEXT1Interrrupt) {
					s->continueEXT1Interrrupt = 1;
					s->sc_EXTINT |= 1 << 1;
					qemu_irq_pulse(s->ext1_irq);
				}
			}
		}
	}
	
	if(((s->sc_EXTMODE & (1 << 2)) != 0)) {
		if(((s->sc_EXTPOLAR & (1 << 2)) != 0)) {
			if(s->currentEXT2Value == 1 && s->previousEXT2Value == 0) {
				if(!s->continueEXT2Interrrupt) {
					s->continueEXT2Interrrupt = 1;
					s->sc_EXTINT |= 1 << 2;
					qemu_irq_pulse(s->ext2_irq);
				}
			}
		}
		else {
			if(s->currentEXT2Value == 0 && s->previousEXT2Value == 1) {
				if(!s->continueEXT2Interrrupt) {
					s->continueEXT2Interrrupt = 1;
					s->sc_EXTINT |= 1 << 2;
					qemu_irq_pulse(s->ext2_irq);
				}
			}
		}
	}
	else {
		if(((s->sc_EXTPOLAR & (1 << 2)) != 0)) {
			if(s->currentEXT2Value == 1) {
				if(!s->continueEXT2Interrrupt) {
					s->continueEXT2Interrrupt = 1;
					s->sc_EXTINT |= 1 << 2;
					qemu_irq_pulse(s->ext2_irq);
				}
			}
		}
	}
	
	if(((s->sc_EXTMODE & (1 << 3)) != 0)) {
		if(((s->sc_EXTPOLAR & (1 << 3)) != 0)) {
			if(s->currentEXT3Value == 1 && s->previousEXT3Value == 0) {
				if(!s->continueEXT3Interrrupt) {
					s->continueEXT3Interrrupt = 1;
					s->sc_EXTINT |= 1 << 3;
					qemu_irq_pulse(s->ext3_irq);
				}
			}
		}
		else {
			if(s->currentEXT3Value == 0 && s->previousEXT3Value == 1) {
				if(!s->continueEXT3Interrrupt) {
					s->continueEXT3Interrrupt = 1;
					s->sc_EXTINT |= 1 << 3;
					qemu_irq_pulse(s->ext3_irq);
				}
			}
		}
	}
	else {
		if(((s->sc_EXTPOLAR & (1 << 3)) != 0)) {
			if(s->currentEXT3Value == 1) {
				if(!s->continueEXT3Interrrupt) {
					s->continueEXT3Interrrupt = 1;
					s->sc_EXTINT |= 1 << 3;
					qemu_irq_pulse(s->ext3_irq);
				}
			}
		}
	}
	
	s->previousEXT0Value = s->currentEXT0Value;
	s->previousEXT1Value = s->currentEXT1Value;
	s->previousEXT2Value = s->currentEXT2Value;
	s->previousEXT3Value = s->currentEXT3Value;
	
	lpc4088_sc_timer_set_alarm(s, now);
}

static inline int64_t lpc4088_ns_to_ticks(LPC4088SCState *s, int64_t t) {
    return muldiv64(t, s->freq_hz, LPC4088_SC_TIMER_FREQUENCY) / (s->SCPR + 1);
}

static void lpc4088_sc_timer_set_alarm(LPC4088SCState *s, int64_t now) {
    uint64_t ticks;
    int64_t now_ticks;
	//DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","ADC_Set_Alarm", (uint32_t) 0);
	
	now_ticks = lpc4088_ns_to_ticks(s, now);
	
	ticks = 100000 - (now_ticks - s->tick_offset);
	
	s->hit_time = muldiv64((ticks + (uint64_t) now_ticks) * (s->SCPR + 1), LPC4088_SC_TIMER_FREQUENCY, s->freq_hz);
	
	timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->hit_time);
}

static void lpc4088_sc_reset(DeviceState *dev) {
    LPC4088SCState *s = LPC4088_SC(dev);
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
	
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

	if(s->hard_fault_irq) {
		qemu_irq_lower(s->hard_fault_irq);
	}
	
	s->currentEXT0Value = 0x00000000;
	s->currentEXT1Value = 0x00000000;
	s->currentEXT2Value = 0x00000000;
	s->currentEXT3Value = 0x00000000;
	
	s->previousEXT0Value = 0x00000000;
	s->previousEXT1Value = 0x00000000;
	s->previousEXT2Value = 0x00000000;
	s->previousEXT3Value = 0x00000000;
	
	s->continueEXT0Interrrupt = 0x00000000;
	s->continueEXT1Interrrupt = 0x00000000;
	s->continueEXT2Interrrupt = 0x00000000;
	s->continueEXT3Interrrupt = 0x00000000;
	
	s->SCPR = 50;
	s->tick_offset = lpc4088_ns_to_ticks(s, now);
}

static uint64_t lpc4088_sc_read(void *opaque, hwaddr offset, unsigned int size) {
    LPC4088SCState *s = opaque;

    switch (offset) {
    case LPC4088_SC_FLASHCFG:
		return s->sc_FLASHCFG;
	case LPC4088_SC_REG_PLL0CON:
		return s->sc_PLL0CON;
	case LPC4088_SC_REG_PLL0CFG:
		return s->sc_PLL0CFG;
	case LPC4088_SC_REG_PLL0STAT:
		return s->sc_PLL0STAT;
	case LPC4088_SC_REG_PLL1CON:
		return s->sc_PLL1CON;
	case LPC4088_SC_REG_PLL1CFG:
		return s->sc_PLL1CFG;
	case LPC4088_SC_REG_PLL1STAT:
		return s->sc_PLL1STAT;
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
	case LPC4088_SC_REG_IRCCTRL:
		return s->sc_IRCCTRL;
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
	case LPC4088_SC_REG_PLL0FEED:
	case LPC4088_SC_REG_PLL1FEED:
		qemu_irq_pulse(s->hard_fault_irq);
		return 0;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, offset);
    }

    return 0;
}

static void lpc4088_sc_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size) {
    LPC4088SCState *s = LPC4088_SC(opaque);
    uint32_t value = (uint32_t) val64;
			 
	DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_sc_register_name(addr), (uint32_t) value);

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
		// TODO implement feed logic
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
		// TODO implement feed logic
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
		if(((value & (1 << 0)) != 0)) {
			s->continueEXT0Interrrupt = 0;
			qemu_set_irq(s->ext0_irq,0);
		}
		if(((value & (1 << 1)) != 0)) {
			s->continueEXT1Interrrupt = 0;
			qemu_set_irq(s->ext1_irq,0);
		}
		if(((value & (1 << 2)) != 0)) {
			s->continueEXT2Interrrupt = 0;
			qemu_set_irq(s->ext2_irq,0);
		}
		if(((value & (1 << 3)) != 0)) {
			s->continueEXT3Interrrupt = 0;
			qemu_set_irq(s->ext3_irq,0);
		}	
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
	case LPC4088_SC_REG_IRCCTRL:
		s->sc_IRCCTRL = value;
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

static void lpc4088_sc_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg) {
    LPC4088SCState *s = LPC4088_SC(rcs->connected_device);
	
	if(s->enable_rc) {
		if(msg->arg1 == '0' && msg->arg2 < 32) {
			if(msg->cmd == REMOTE_CTRL_SET_VALUE_EXT_PIN) {
				if(msg->arg2 == 0) {
					s->currentEXT0Value = msg->arg3;
					DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","Remote_Change_EXT0", (uint32_t) msg->arg3);
				}
				else if(msg->arg2 == 1) {
					s->currentEXT1Value = msg->arg3;
				}
				else if(msg->arg2 == 2) {
					s->currentEXT2Value = msg->arg3;
				}
				else if(msg->arg2 == 3) {
					s->currentEXT3Value = msg->arg3;
				}
			}
		}
    }
}

static const MemoryRegionOps lpc4088_sc_ops = {
    .read = lpc4088_sc_read,
    .write = lpc4088_sc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    //.impl.min_access_size = 4,
    //.impl.max_access_size = 4,
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

static Property lpc4088_sc_properties[] = {
	DEFINE_PROP_UINT64("clock-frequency", struct LPC4088SCState, freq_hz, LPC4088_SC_TIMER_FREQUENCY),
	DEFINE_PROP_STRING("sc-name", struct LPC4088SCState, sc_name),
    DEFINE_PROP_BOOL("enable-rc", struct LPC4088SCState, enable_rc, false),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_sc_init(Object *obj) {
    LPC4088SCState *s = LPC4088_SC(obj);
	
	DeviceState *ds = DEVICE(obj);
	
	object_initialize_child_with_props(
        obj, "RemoteCtrl", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, &error_abort,
        NULL
    );

    s->rcs.connected_device = ds;

    //sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    //memory_region_init_io(&s->mmio, obj, &lpc4088_sc_ops, s,TYPE_LPC4088_SC, LPC4088_SC_MEM_SIZE);
    //sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void lpc4088_sc_realize(DeviceState *dev, Error **errp) {	
    LPC4088SCState *s = LPC4088_SC(dev);
	int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    uint32_t timer_val = 0;
	
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->hard_fault_irq);
	
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->ext0_irq);
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->ext1_irq);
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->ext2_irq);
	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->ext3_irq);
	
	memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_sc_ops, s, TYPE_LPC4088_SC, LPC4088_SC_MEM_SIZE);

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

	s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_sc_timer_interrupt, s);
	
	timer_val = lpc4088_ns_to_ticks(s, now) - s->tick_offset;
	s->tick_offset = lpc4088_ns_to_ticks(s, now) - timer_val;
	lpc4088_sc_timer_set_alarm(s, now);
	
    s->rcs.callback = lpc4088_sc_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), qdev_get_parent_bus(DEVICE(&s->rcs)), errp);
}

static void lpc4088_sc_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

	device_class_set_props(dc, lpc4088_sc_properties);

	dc->realize = lpc4088_sc_realize;
    dc->reset = lpc4088_sc_reset;
	device_class_set_props(dc, lpc4088_sc_properties);
    dc->vmsd = &vmstate_lpc4088_sc;
	dc->realize = lpc4088_sc_realize;
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
