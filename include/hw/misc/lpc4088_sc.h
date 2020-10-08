#ifndef __LPC4088_SC_H__
#define __LPC4088_SC_H__

#include "hw/sysbus.h"

#define TYPE_LPC4088_SC "lpc4088-sc"

#define LPC4088_SC(obj) OBJECT_CHECK(LPC4088SCState, (obj), TYPE_LPC4088_SC)

#define LPC4088_SC_MEM_SIZE 0x200

#define LPC4088_SC_FLASHCFG 0x00

#define LPC4088_SC_REG_PLL0CON 0x080
#define LPC4088_SC_REG_PLL0CFG 0x084
#define LPC4088_SC_REG_PLL0STAT 0x088
#define LPC4088_SC_REG_PLL0FEED 0x08C

#define LPC4088_SC_REG_PLL1CON 0x0A0
#define LPC4088_SC_REG_PLL1CFG 0x0A4
#define LPC4088_SC_REG_PLL1STAT 0x0A8
#define LPC4088_SC_REG_PLL1FEED 0x0AC

#define LPC4088_SC_REG_PCON 0x0C0
#define LPC4088_SC_REG_PCONP 0x0C4
#define LPC4088_SC_REG_PCONP1 0x0C8

#define LPC4088_SC_REG_EMCCLKSEL 0x100
#define LPC4088_SC_REG_CCLKSEL 0x104
#define LPC4088_SC_REG_USBCLKSEL 0x108
#define LPC4088_SC_REG_CLKSRCSEL 0x10C
#define LPC4088_SC_REG_CANSLEEPCLR 0x110
#define LPC4088_SC_REG_CANWAKEFLAGS 0x114

#define LPC4088_SC_REG_EXTINT 0x140

#define LPC4088_SC_REG_EXTMODE 0x148
#define LPC4088_SC_REG_EXTPOLAR 0x14C

#define LPC4088_SC_REG_RSID 0x180

#define LPC4088_SC_REG_SCS 0x1A0
#define LPC4088_SC_REG_IRCTRIM 0x1A4
#define LPC4088_SC_REG_PCLKSEL 0x1A8

#define LPC4088_SC_REG_PBOOST 0x1B0
#define LPC4088_SC_REG_SPIFICLKSEL 0x1B4
#define LPC4088_SC_REG_LCD_CFG 0x1B8

#define LPC4088_SC_REG_USBIntSt 0x1C0
#define LPC4088_SC_REG_DMAREQSEL 0x1C4
#define LPC4088_SC_REG_CLKOUTCFG 0x1C8
#define LPC4088_SC_REG_RSTCON0 0x1CC
#define LPC4088_SC_REG_RSTCON1 0x1D0

#define LPC4088_SC_REG_EMCDLYCTL 0x1DC
#define LPC4088_SC_REG_EMCCALd 0x1E0

#define PLL_FEED_STATE_DEFAULT 0
#define PLL_FEED_STATE_FEED1 1
#define PLL_FEED_STATE_FEED2 2

typedef struct LPC4088SCState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

	uint8_t pll0_feed_state;
	uint8_t pll1_feed_state;

	uint32_t sc_FLASHCFG;
	
	uint32_t sc_PLL0CON;
	uint32_t sc_PLL0CFG;
	uint32_t sc_PLL0STAT;

	
	uint32_t sc_PLL1CON;
	uint32_t sc_PLL1CFG;
	uint32_t sc_PLL1STAT;
	uint32_t sc_PLL1FEED;
	
	uint32_t sc_PCON;
	uint32_t sc_PCONP;
	uint32_t sc_PCONP1;
	
	uint32_t sc_EMCCLKSEL;
	uint32_t sc_CCLKSEL;
	uint32_t sc_USBCLKSEL;
	uint32_t sc_CLKSRCSEL;
	uint32_t sc_CANSLEEPCLR;
	uint32_t sc_CANWAKEFLAGS;
	
	uint32_t sc_EXTINT;
	uint32_t sc_EXTMODE;
	uint32_t sc_EXTPOLAR;
	
	uint32_t sc_RSID;
	
	uint32_t sc_SCS;
	uint32_t sc_IRCCTRL;
	uint32_t sc_IRCTRIM;
	uint32_t sc_PCLKSEL;
	
	uint32_t sc_PBOOST;
	uint32_t sc_SPIFICLKSEL;
	uint32_t sc_LCD_CFG;
	
	uint32_t sc_USBIntSt;
	uint32_t sc_DMAREQSEL;
	uint32_t sc_CLKOUTCFG;
	uint32_t sc_RSTCON0;
	uint32_t sc_RSTCON1;

	uint32_t sc_EMCDLYCTL;
	uint32_t sc_EMCCALd;
	
    qemu_irq irq;
	
	qemu_irq hard_fault_irq;

} LPC4088SCState;

#endif /* __LPC4088_SC_H__ */
