/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "exec/address-spaces.h"
#include "hw/arm/lpc4088fet208.h"
#include "hw/qdev-properties.h"
#include "sysemu/sysemu.h"

#define NAME_SIZE 20

static void lpc4088fet208_init(Object *obj)
{
    LPC4088FET208State *s = LPC4088FET208(obj);
    int i;
    char name[NAME_SIZE];

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    for (i = 0; i < LPC4088_NR_GPIO_PORTS; i++)
    {
        snprintf(name, NAME_SIZE, "gpio%d", i);
        object_initialize_child(obj, name, &s->gpio[i],
                                TYPE_LPC4088_GPIO_PORT);
    }
}

static void lpc4088fet208_realize(DeviceState *dev_soc, Error **errp)
{
    int i;
    LPC4088FET208State *s = LPC4088FET208(dev_soc);
    DeviceState *armv7m;
    //SysBusDevice *busdev;
    Error *err = NULL;

    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *sram = g_new(MemoryRegion, 1);
    MemoryRegion *flash = g_new(MemoryRegion, 1);

    memory_region_init_rom(flash, OBJECT(dev_soc),
            "LPC4088FET208.flash", LPC4088FET208_FLASH_SIZE, &error_fatal);
    memory_region_add_subregion(system_memory, LPC4088_FLASH_BASE_ADDRESS, flash);

    memory_region_init_ram(sram, NULL, "LPC4088FET208.sram",
            LPC4088FET208_SRAM_SIZE, &error_fatal);
    memory_region_add_subregion(system_memory, LPC4088FET208_SRAM_BASE_ADDRESS, sram);

    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq", 96);
    qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    object_property_set_link(OBJECT(&s->armv7m), OBJECT(get_system_memory()),
            "memory", &error_abort);
    
    for (i = 0; i < LPC4088_NR_GPIO_PORTS; i++)
    {
        sysbus_realize(SYS_BUS_DEVICE(&s->gpio[i]), &err);
        if (err)
        {
            error_propagate(errp, err);
            return;
        }

        sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio[i]), 0,
                LPC4088_GPIO_BASE_ADDR + 0x20 * i);
    }

    sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), &err);
    if(err != NULL)
    {
        error_propagate(errp, err);
        return;
    }
}

static Property lpc4088fet208_soc_properties[] = {
    DEFINE_PROP_STRING("cpu-type", LPC4088FET208State, cpu_type),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088fet208_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->realize = lpc4088fet208_realize;
    
    device_class_set_props(dc, lpc4088fet208_soc_properties);
}

static const TypeInfo lpc4088fet208_info = {
    .name          = TYPE_LPC4088FET208,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088FET208State),
    .instance_init = lpc4088fet208_init,
    .class_init    = lpc4088fet208_class_init,
};

static void lpc4088fet208_types(void)
{
    type_register_static(&lpc4088fet208_info);
}

type_init(lpc4088fet208_types);