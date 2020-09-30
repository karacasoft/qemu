#ifndef __LPC4088_DAC_H__
#define __LPC4088_DAC_H__

#include "hw/sysbus.h"

#define TYPE_LPC4088_DAC "lpc4088-dac"

#define LPC4088DAC(obj) OBJECT_CHECK(LPC4088DACState, (obj), TYPE_LPC4088_DAC)

#define LPC4088_DAC_MEM_SIZE 0x010

#define LPC4088_DAC_REG_CR 0x000
#define LPC4088_DAC_REG_CTRL 0x004
#define LPC4088_DAC_REG_CNTVAL 0x008

typedef struct LPC4088DACState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t dac_CR;
    uint32_t dac_CTRL;
    uint32_t dac_CNTVAL;
	
    qemu_irq irq;
} LPC4088DACState;

#endif /* __LPC4088_DAC_H__ */
