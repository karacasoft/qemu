/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/arm/lpc4088fet208.h"
#include "hw/qdev-properties.h"
#include "hw/arm/boot.h"

static void lpc4088_devboard_init(MachineState *machine)
{
    DeviceState *dev;

    dev = qdev_new(TYPE_LPC4088FET208);
    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m4"));
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    armv7m_load_kernel(ARM_CPU(first_cpu),
            machine->kernel_filename, LPC4088FET208_FLASH_SIZE);
}

static void lpc4088_devboard_machine_init(MachineClass *mc)
{
    mc->desc = "LPC4088 Development Board";
    mc->init = lpc4088_devboard_init;
    mc->ignore_memory_transaction_failures = true;
}

DEFINE_MACHINE("LPC4088", lpc4088_devboard_machine_init)
