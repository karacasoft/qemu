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

static const uint32_t timer_addresses[LPC4088_NR_TIMERS] = { 0x40004000, 0x40008000,
    0x40090000, 0x40094000 };
	
static const uint32_t pwm_addresses[LPC4088_NR_PWMS] = { 0x40014000, 0x40018000};
static const uint32_t adc_addresses[LPC4088_NR_ADCS] = { 0x40034000};
static const uint32_t usart_addresses[LPC4088_NR_USARTS] = { 0x4000C000, 0x40010000, 0x40088000, 0x4009C000,  0x400A4000};
	
	
static const int timer_irq[LPC4088_NR_TIMERS] = {1, 2, 3, 4};
static const int pwm_irq[LPC4088_NR_PWMS] = {55, 25};
static const int adc_irq[LPC4088_NR_ADCS] = {38};
static const int usart_irq[LPC4088_NR_USARTS] = {21, 22, 23, 24, 51};

#define NAME_SIZE 20

static void lpc4088fet208_trigger_hard_fault(void *opaque, int n, int level)
{
    LPC4088FET208State *s = LPC4088FET208(opaque);
    if(level)
    {
        cpu_interrupt(CPU(s->armv7m.cpu), CPU_INTERRUPT_HARD);
    }
}

static void lpc4088fet208_init(Object *obj)
{
    LPC4088FET208State *s = LPC4088FET208(obj);
    int i;
    char name[NAME_SIZE];

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);
    object_initialize_child(obj, "syscon", &s->syscon, TYPE_LPC4088_SC);

    for (i = 0; i < LPC4088_NR_GPIO_PORTS; i++)
    {
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
	
	for (i = 0; i < LPC4088_NR_USARTS; i++) {
		snprintf(name, NAME_SIZE, "usart%d", i);
        object_initialize_child(obj, name, &s->usart[i],
                                TYPE_LPC4088_USART);
    }

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->hardfault_input_irq);
}

static void lpc4088fet208_realize(DeviceState *dev_soc, Error **errp)
{
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

    qdev_init_gpio_in(dev_soc, lpc4088fet208_trigger_hard_fault, 1);

    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq", 56);//LPC4088 56
    qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    object_property_set_link(OBJECT(&s->armv7m), OBJECT(get_system_memory()),
            "memory", &error_abort);
    

	sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), &err);
    if(err != NULL)
    {
        error_propagate(errp, err);
        return;
    }


    //////////////////////// SYSCON //////////////////////////////
    sysbus_realize(SYS_BUS_DEVICE(&s->syscon), &err);
    if(err != NULL)
    {
        error_propagate(errp, err);
        return;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->syscon), 0, 0x400FC000);

    qdev_connect_gpio_out_named(DEVICE(&s->syscon), "HardFaultIRQ", 1, s->hardfault_input_irq);
    ///////////////////////// SYSCON /////////////////////////////
	
	for (i = 0; i < LPC4088_NR_USARTS; i++) {
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
		
		
		/*qdev_prop_set_chr(SYS_BUS_DEVICE(&s->usart[i]), "chardev", serial_hd(i));
        sysbus_realize(SYS_BUS_DEVICE(&s->usart[i]), &err);
		
        if (err) {
            error_propagate(errp, err);
            return;
        }
		
		sysbus_mmio_map(SYS_BUS_DEVICE(&s->usart[i]), 0, usart_addresses[i]);
		
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->usart[i]), 0, qdev_get_gpio_in(SYS_BUS_DEVICE(&s->armv7m), usart_irq[i]));*/
    }
	
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
	
	for (i = 0; i < LPC4088_NR_TIMERS; i++) {
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
		
		/*dev = DEVICE(&(s->usart[i]));
        qdev_prop_set_uint64(SYS_BUS_DEVICE(&s->timer[i]), "clock-frequency", 1000000000);
        sysbus_realize(SYS_BUS_DEVICE(&s->timer[i]), &err);
		
        if (err) {
            error_propagate(errp, err);
            return;
        }
		
		sysbus_mmio_map(SYS_BUS_DEVICE(&s->timer[i]), 0, timer_addresses[i]);
		
        sysbus_connect_irq(SYS_BUS_DEVICE(DEVICE(&s->timer[i])), 0, qdev_get_gpio_in(DEVICE(&s->armv7m), timer_irq[i]));*/
    }
	
	for (i = 0; i < LPC4088_NR_PWMS; i++) {
		dev = DEVICE(&(s->pwm[i]));
        qdev_prop_set_uint64(dev, "clock-frequency", 1000000000);
        sysbus_realize(SYS_BUS_DEVICE(&s->pwm[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, pwm_addresses[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, pwm_irq[i]));
		
        /*qdev_prop_set_uint64(SYS_BUS_DEVICE(&s->pwm[i]), "clock-frequency", 1000000000);
        sysbus_realize(SYS_BUS_DEVICE(&s->pwm[i]), &err);
		
        if (err) {
            error_propagate(errp, err);
            return;
        }
		
		sysbus_mmio_map(SYS_BUS_DEVICE(&s->pwm[i]), 0, pwm_addresses[i]);
		
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->pwm[i]), 0, qdev_get_gpio_in(SYS_BUS_DEVICE(&s->armv7m), pwm_irq[i]));*/
    }
	
	for (i = 0; i < LPC4088_NR_ADCS; i++) {
        sysbus_realize(SYS_BUS_DEVICE(&s->adc[i]), &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, adc_addresses[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, adc_irq[i]));
		
        /*sysbus_realize(SYS_BUS_DEVICE(&s->adc[i]), &err);
		
        if (err) {
            error_propagate(errp, err);
            return;
        }
		
		sysbus_mmio_map(SYS_BUS_DEVICE(&s->adc[i]), 0, adc_addresses[i]);
		
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->adc[i]), 0, qdev_get_gpio_in(SYS_BUS_DEVICE(&s->armv7m), adc_irq[i]));*/
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