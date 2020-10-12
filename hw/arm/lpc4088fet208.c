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
#include "hw/qdev-core.h"
#include "hw/qdev-properties.h"

static const uint32_t timer_addresses[LPC4088_NR_TIMERS] = {0x40004000, 0x40008000, 0x40090000, 0x40094000 };
static const uint32_t pwm_addresses[LPC4088_NR_PWMS] = {0x40014000, 0x40018000};
static const uint32_t adc_addresses[LPC4088_NR_ADCS] = {0x40034000};
static const uint32_t dac_addresses[LPC4088_NR_DACS] = {0x4008C000};
static const uint32_t usart_addresses[LPC4088_NR_USARTS] = {0x4000C000, 0x40010000, 0x40088000, 0x4009C000,  0x400A4000};
	
static const int timer_irq[LPC4088_NR_TIMERS] = {1, 2, 3, 4};
static const int pwm_irq[LPC4088_NR_PWMS] = {39, 9};
static const int adc_irq[LPC4088_NR_ADCS] = {22};
static const int usart_irq[LPC4088_NR_USARTS] = {5, 6, 7, 8, 35};

static const char *gpio_names[] = {"0", "1", "2", "3", "4", "5" };
static const char *timer_names[] = {"0", "1", "2", "3", "4" };
static const char *pwm_names[] = {"0", "1"};
static const char *adc_names[] = {"0"};
static const char *dac_names[] = {"0"};
static const char *usart_names[] = {"0", "1", "2", "3", "4"};

#define NAME_SIZE 20

static void lpc4088fet208_init(Object *obj) {
    LPC4088FET208State *s = LPC4088FET208(obj);
    int i;
    char name[NAME_SIZE];

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    for (i = 0; i < LPC4088_NR_GPIO_PORTS; i++) {
        snprintf(name, NAME_SIZE, "gpio%d", i);
        object_initialize_child(obj, name, &s->gpio[i],
                                TYPE_LPC4088_GPIO_PORT);
    }
	
	for (i = 0; i < LPC4088_NR_TIMERS; i++) {
		snprintf(name, NAME_SIZE, "timer%d", i);
        object_initialize_child(obj, name, &s->timer[i],
                                TYPE_LPC4088_TIMER);
    }
	
	for (i = 0; i < LPC4088_NR_PWMS; i++) {
		snprintf(name, NAME_SIZE, "pwm%d", i);
        object_initialize_child(obj, name, &s->pwm[i],
                                TYPE_LPC4088_PWM);
    }
	
	for (i = 0; i < LPC4088_NR_ADCS; i++) {
		snprintf(name, NAME_SIZE, "adc%d", i);
        object_initialize_child(obj, name, &s->adc[i],
                                TYPE_LPC4088_ADC);
    }
	
	for (i = 0; i < LPC4088_NR_ADCS; i++) {
		snprintf(name, NAME_SIZE, "dac%d", i);
        object_initialize_child(obj, name, &s->dac[i],
                                TYPE_LPC4088_DAC);
    }
	
	for (i = 0; i < LPC4088_NR_USARTS; i++) {
		snprintf(name, NAME_SIZE, "usart%d", i);
        object_initialize_child(obj, name, &s->usart[i],
                                TYPE_LPC4088_USART);
    }
}

static void lpc4088fet208_realize(DeviceState *dev_soc, Error **errp) {
    int i;
    LPC4088FET208State *s = LPC4088FET208(dev_soc);
    DeviceState *dev, *armv7m;
    SysBusDevice *busdev;
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
    qdev_prop_set_uint32(armv7m, "num-irq", 56);
    qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    object_property_set_link(OBJECT(&s->armv7m), OBJECT(get_system_memory()),
            "memory", &error_abort);
    
	sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), &err);
    if(err != NULL) {
        error_propagate(errp, err);
        return;
    }
	
    for (i = 0; i < LPC4088_NR_GPIO_PORTS; i++) {
		qdev_prop_set_string(DEVICE(&s->gpio[i]), "port-name", gpio_names[i]);
        qdev_prop_set_bit(DEVICE(&s->gpio[i]), "enable-rc", true);

        sysbus_realize(SYS_BUS_DEVICE(&s->gpio[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio[i]), 0, LPC4088_GPIO_BASE_ADDR + 0x20 * i);
    }
	
	for (i = 0; i < LPC4088_NR_TIMERS; i++) {
		qdev_prop_set_string(DEVICE(&s->timer[i]), "timer-name", timer_names[i]);
        qdev_prop_set_bit(DEVICE(&s->timer[i]), "enable-rc", true);
		
		dev = DEVICE(&(s->timer[i]));
        qdev_prop_set_uint64(dev, "clock-frequency", 60000000);
        sysbus_realize(SYS_BUS_DEVICE(&s->timer[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, timer_addresses[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, timer_irq[i]));
    }
	
	for (i = 0; i < LPC4088_NR_PWMS; i++) {
		qdev_prop_set_string(DEVICE(&s->pwm[i]), "pwm-name", pwm_names[i]);
        qdev_prop_set_bit(DEVICE(&s->pwm[i]), "enable-rc", true);
		
		dev = DEVICE(&(s->pwm[i]));
        qdev_prop_set_uint64(dev, "clock-frequency", 60000000);
        sysbus_realize(SYS_BUS_DEVICE(&s->pwm[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, pwm_addresses[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, pwm_irq[i]));
    }
	
	for (i = 0; i < LPC4088_NR_ADCS; i++) {
		qdev_prop_set_string(DEVICE(&s->adc[i]), "adc-name", adc_names[i]);
        qdev_prop_set_bit(DEVICE(&s->adc[i]), "enable-rc", true);
		
		dev = DEVICE(&(s->adc[i]));
        qdev_prop_set_uint64(dev, "clock-frequency", 0xF4240);
        sysbus_realize(SYS_BUS_DEVICE(&s->adc[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, adc_addresses[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, adc_irq[i]));
    }
	
	for (i = 0; i < LPC4088_NR_DACS; i++) {
		qdev_prop_set_string(DEVICE(&s->dac[i]), "dac-name", dac_names[i]);
        qdev_prop_set_bit(DEVICE(&s->dac[i]), "enable-rc", true);
		
		dev = DEVICE(&(s->dac[i]));
        sysbus_realize(SYS_BUS_DEVICE(&s->dac[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, dac_addresses[i]);
        //sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, dac_irq[i]));
    }
	
	for (i = 0; i < LPC4088_NR_USARTS; i++) {
		qdev_prop_set_string(DEVICE(&s->usart[i]), "usart-name", usart_names[i]);
        qdev_prop_set_bit(DEVICE(&s->usart[i]), "enable-rc", true);
		
		dev = DEVICE(&(s->usart[i]));
        qdev_prop_set_chr(dev, "chardev", serial_hd(i));
        sysbus_realize(SYS_BUS_DEVICE(&s->usart[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, usart_addresses[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, usart_irq[i]));
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
