/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#ifndef __LPC4088FET208_H__
#define __LPC4088FET208_H__

#include "hw/arm/armv7m.h"

#define TYPE_LPC4088FET208 "lpc4088fet208"
#define LPC4088FET208(obj) \
    OBJECT_CHECK(LPC4088FET208State, (obj), TYPE_LPC4088FET208)

#define LPC4088_FLASH_BASE_ADDRESS 0x0
#define LPC4088FET208_FLASH_SIZE (512*1024)

#define LPC4088FET208_SRAM_BASE_ADDRESS 0x20000000
#define LPC4088FET208_SRAM_SIZE (16*1024)


typedef struct LPC4088FET208State {
    /*<private>*/
    SysBusDevice parent_obj;

    /*<public>*/
    char *cpu_type;
    ARMv7MState armv7m;

} LPC4088FET208State;

#endif // __LPC4088FET208_H__