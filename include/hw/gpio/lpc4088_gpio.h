/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#ifndef __LPC4088_GPIO_H__
#define __LPC4088_GPIO_H__

#include <stdint.h>
#include "hw/sysbus.h"
#include "hw/remotectrl/remotectrl.h"
#include "hw/misc/lpc4088_sc.h"

#define LPC4088_GPIO_PORT_MEM_SIZE 0x20

#define LPC4088_GPIO_PORT_PIN_COUNT 32

#define TYPE_LPC4088_GPIO_PORT "lpc4088.gpioport"
#define LPC4088_GPIO_PORT(obj) OBJECT_CHECK(LPC4088GPIOPortState, (obj), TYPE_LPC4088_GPIO_PORT)

// TODO interupt registers must be implemented
#define DIR_ADDR  0x00
#define MASK_ADDR 0x10
#define PIN_ADDR  0x14
#define SET_ADDR  0x18
#define CLR_ADDR  0x1C

typedef struct __LPC4088GPIOPortState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion iomem;

    char *port_name;
    bool enable_rc;

    uint32_t dir;
    uint32_t mask;
    uint32_t pin;

    qemu_irq output[LPC4088_GPIO_PORT_PIN_COUNT];

    LPC4088SCState *sc;

    RemoteCtrlState rcs;

} LPC4088GPIOPortState;



#endif // __LPC4088_GPIO_H__