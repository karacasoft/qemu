#ifndef __LPC4088_ADC_H__
#define __LPC4088_ADC_H__

#include "hw/sysbus.h"
#include "hw/remotectrl/remotectrl.h"

#define TYPE_LPC4088_ADC "lpc4088-adc"

#define LPC4088ADC(obj) OBJECT_CHECK(LPC4088ADCState, (obj), TYPE_LPC4088_ADC)

#define LPC4088_ADC_MEM_SIZE 0x040

#define LPC4088_ADC_REG_CR 0x000
#define LPC4088_ADC_REG_GDR 0x004
#define LPC4088_ADC_REG_INTEN 0x00C
#define LPC4088_ADC_REG_DR0 0x010
#define LPC4088_ADC_REG_DR1 0x014
#define LPC4088_ADC_REG_DR2 0x018
#define LPC4088_ADC_REG_DR3 0x01C
#define LPC4088_ADC_REG_DR4 0x020
#define LPC4088_ADC_REG_DR5 0x024
#define LPC4088_ADC_REG_DR6 0x028
#define LPC4088_ADC_REG_DR7 0x02C
#define LPC4088_ADC_REG_STAT 0x030
#define LPC4088_ADC_REG_ADTRM 0x034

typedef struct LPC4088ADCState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion iomem;
	
	char *adc_name;
    bool enable_rc;
	uint32_t enableRemoteInterrupt;

    uint32_t adc_CR;
    uint32_t adc_GDR;
    uint32_t adc_INTEN;
    uint32_t adc_DR0;
    uint32_t adc_DR1;
	uint32_t adc_DR2;
	uint32_t adc_DR3;
	uint32_t adc_DR4;
	uint32_t adc_DR5;
	uint32_t adc_DR6;
	uint32_t adc_DR7;
	uint32_t adc_STAT;
	uint32_t adc_ADTRM;
	
    qemu_irq irq;
	
	RemoteCtrlState rcs;
	uint32_t lastData;
	
} LPC4088ADCState;

#endif /* __LPC4088_ADC_H__ */
