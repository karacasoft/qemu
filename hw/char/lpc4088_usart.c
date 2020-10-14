#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/char/lpc4088_usart.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "qemu/main-loop.h"

#include "glib.h"

#ifndef LPC4088_USART_ERROR_DEBUG
#define LPC4088_USART_ERROR_DEBUG 1
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_USART_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_USART,__func__, ##args);}

#define REMOTE_CTRL_USART_MAGIC 0xBADB0001

#define REMOTE_CTRL_USART_CMD_SEND_CHARS 0x20100


static const char *lpc4088_usart_register_name(uint32_t offset) {
	switch (offset) {
    case LPC4088_USART_REG_RBR:
        return "USART_RBR_THR_DLL";
		
    case LPC4088_USART_REG_DLM:
        return "USART_DLM_IER";
		
    case LPC4088_USART_REG_IIR:
        return "USART_IIR_FCR";
		
	case LPC4088_USART_REG_LCR:
        return "USART_LCR";
	case LPC4088_USART_REG_LSR:
        return "USART_LSR";
	case LPC4088_USART_REG_SCR:
        return "USART_SCR";
	case LPC4088_USART_REG_ACR:
        return "USART_ACR";
	case LPC4088_USART_REG_FDR:
        return "USART_FDR";
	case LPC4088_USART_REG_TER:
        return "USART_TER";
		
	case LPC4088_USART_REG_RS485CTRL:
        return "USART_RS485CTRL";
	case LPC4088_USART_REG_RS485ADRMATCH:
        return "USART_RS485ADRMATCH";
	case LPC4088_USART_REG_RS485DLY:
        return "USART_RS485DLY";
    default:
        return "USART_[?]";
    }
}

static void lpc4088_usart_interrupt(LPC4088USARTState *s) {
    if(s->rls_interrupt_active) {
        s->usart_IIR &= ~0xF;
        s->usart_IIR |= (3 << 1);
        qemu_irq_pulse(s->irq);
    } else if(s->rda_interrupt_active) {
        s->usart_IIR &= ~0xF;
        s->usart_IIR |= (2 << 1);
        qemu_irq_pulse(s->irq);
    } else if(s->cti_interrupt_active) {
        s->usart_IIR &= ~0xF;
        s->usart_IIR |= (6 << 1);
        qemu_irq_pulse(s->irq);
    } else if(s->thre_interrupt_active) {
        s->usart_IIR &= ~0xF;
        s->usart_IIR |= (1 << 1);
        qemu_irq_pulse(s->irq);
    } else {
        s->usart_IIR &= ~0xF;
        s->usart_IIR |= (1 << 0);
    }
}

static void lpc4088_usart_send_char_interrupt(void *opaque) {
    LPC4088USARTState *s = LPC4088USART(opaque);

    if(s->usart_IER & (1 << 1)) {
        s->thre_interrupt_active = true;
        lpc4088_usart_interrupt(s);
    }
}

static void lpc4088_usart_receive(LPC4088USARTState *opaque, const char *buf) {
    LPC4088USARTState *s = LPC4088USART(opaque);

    DEBUG_PRINT("USART RECEIVED: %s\n", buf);

    size_t size = 20;

    while(size-- && *buf != 0) {
        *(s->fifo_write_ptr++) = *buf++;
        if(s->fifo_write_ptr - s->fifo > LPC4088_USART_FIFO_SIZE) {
            s->fifo_write_ptr = s->fifo;
        }
        if(s->fifo_write_ptr == s->fifo_read_ptr) {
            // Overrun Error
            s->usart_LSR |= 0x2;
            if(s->usart_IER & 0x4) {
                s->rls_interrupt_active = true;
            }
            break;
        }
    }
    // set RDR
    s->usart_LSR |= 0x1;

    if(s->usart_IER & 1) {
        s->rda_interrupt_active = true;
    }

    if(s->rda_interrupt_active || s->rls_interrupt_active) {
        qemu_mutex_lock_iothread();
        lpc4088_usart_interrupt(s);
        qemu_mutex_unlock_iothread();
    }
}

static void lpc4088_usart_send(LPC4088USARTState *s, const char ch) {
    RemoteCtrlClass *rcc = REMOTE_CTRL_GET_CLASS(&s->rcs);
    
    RemoteCtrlMessage msg = {
        .magic = REMOTE_CTRL_USART_MAGIC,
        .cmd = 0,
        .arg1 = s->usart_name[0],
        .arg2 = ch,
        .arg3 = 0,
        .arg4 = 0,
        .arg5 = 0,
        .arg6 = 0
    };

    rcc->send_message(&s->rcs, &msg, sizeof(RemoteCtrlMessage));
}

static void lpc4088_usart_reset(DeviceState *dev) {
    LPC4088USARTState *s = LPC4088USART(dev);
	
	s->usart_RBR = 0x00000000;
	s->usart_THR = 0x00000000;
	s->usart_DLL = 0x1;

	s->usart_DLM = 0x00000000;
	s->usart_IER = 0x00000000;

	s->usart_IIR = 0x1;
	s->usart_FCR = 0x00000000;

	s->usart_LCR = 0x00000000;
	s->usart_LSR = 0x60;
	s->usart_SCR = 0x00000000;
	s->usart_ACR = 0x00000000;
	s->usart_FDR = 0x10;
	s->usart_TER = 0x80;
	s->usart_RS485CTRL = 0x00000000;
	s->usart_RS485ADRMATCH = 0x00000000;
	s->usart_RS485DLY = 0x00000000;
	
	s->enableRemoteInterrupt = 0;
}

static uint64_t lpc4088_usart_read(void *opaque, hwaddr offset, unsigned int size) {
    LPC4088USARTState *s = opaque;
    uint64_t retvalue;

    switch (offset) {
	case LPC4088_USART_REG_RBR:
        if(s->usart_LCR & (1 << 7)) {
            return s->usart_DLL;
        } else {
            if(s->fifo_read_ptr + 1 == s->fifo_write_ptr ||
                (s->fifo_read_ptr + 1 == s->fifo + LPC4088_USART_FIFO_SIZE && s->fifo_write_ptr == s->fifo)) {
                s->usart_LSR &= ~1;
                s->rda_interrupt_active = false;
                lpc4088_usart_interrupt(s);
            } else {
                if(++s->fifo_read_ptr == s->fifo + LPC4088_USART_FIFO_SIZE) {
                    s->fifo_read_ptr = s->fifo;
                }
                s->usart_RBR = *s->fifo_read_ptr;
            }
            retvalue = s->usart_RBR;
            return retvalue;
        }
    case LPC4088_USART_REG_DLM:
        if(s->usart_LCR & (1 << 7)) {
            return s->usart_DLM;
        } else {
            return s->usart_IER;
        }
    case LPC4088_USART_REG_IIR:
        return s->usart_IIR;
	case LPC4088_USART_REG_LCR:
        return s->usart_LCR;
	case LPC4088_USART_REG_LSR:
        s->rls_interrupt_active = false;
        lpc4088_usart_interrupt(s);
        return s->usart_LSR;
	case LPC4088_USART_REG_SCR:
        return s->usart_SCR;
	case LPC4088_USART_REG_ACR:
        return s->usart_ACR;
	case LPC4088_USART_REG_FDR:
        return s->usart_FDR;
	case LPC4088_USART_REG_TER:
        return s->usart_TER;
	case LPC4088_USART_REG_RS485CTRL:
        return s->usart_RS485CTRL;
	case LPC4088_USART_REG_RS485ADRMATCH:
        return s->usart_RS485ADRMATCH;
	case LPC4088_USART_REG_RS485DLY:
        return s->usart_RS485DLY;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
    }

    return 0;
}

static void lpc4088_usart_write(void *opaque, hwaddr offset, uint64_t val64, unsigned int size) {
    LPC4088USARTState *s = opaque;
    uint32_t value = val64;
    
	DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n",lpc4088_usart_register_name(offset), (uint32_t) offset);
	
    switch (offset) {
    case LPC4088_USART_REG_THR:
        if(s->usart_LCR & (1 << 7)) {
            s->usart_DLL = value;
        } else {
            char ch = value & 0xFF;
            lpc4088_usart_send(s, ch);
            // 10 chars per ms
            // equivalent to almost 9600 baud
            timer_mod_ns(s->send_char_timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + (1000 * 100));
        }
        return;
	case LPC4088_USART_REG_DLM:
        if(s->usart_LCR & (1 << 7)) {
            s->usart_DLM = value;
        } else {
            s->usart_IER = value;
        }
        return;
		
	case LPC4088_USART_REG_FCR:
        s->usart_FCR = value;
        if(value & 0x1) {
            s->usart_IIR |= (3 << 6);
        } else {
            s->usart_IIR &= ~(3 << 6);
        }
        return;
	case LPC4088_USART_REG_LCR:
        s->usart_LCR = value;
        return;
	case LPC4088_USART_REG_LSR:
        // Read only
        return;
	case LPC4088_USART_REG_SCR:
        s->usart_SCR = value;
        return;
	case LPC4088_USART_REG_ACR:
        s->usart_ACR = value;
        return;
	case LPC4088_USART_REG_FDR:
        s->usart_FDR = value;
        return;
	case LPC4088_USART_REG_TER:
        s->usart_TER = value;
        return;
	case LPC4088_USART_REG_RS485CTRL:
        s->usart_RS485CTRL = value;
        return;
	case LPC4088_USART_REG_RS485ADRMATCH:
        s->usart_RS485ADRMATCH = value;
        return;
	case LPC4088_USART_REG_RS485DLY:
        s->usart_RS485DLY = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, offset);
    }
}

static void lpc4088_usart_remote_ctrl_callback(RemoteCtrlState *rcs, RemoteCtrlMessage *msg) {
    LPC4088USARTState *s = LPC4088USART(rcs->connected_device);
    
    if(s->enable_rc) {
        if(msg->cmd == REMOTE_CTRL_USART_CMD_SEND_CHARS) {
            if(msg->arg1 == s->usart_name[0]) {
                const char *msg_bytes = (const char *) msg;
                DEBUG_PRINT("RemoteCTRL received arg2: %d\n", msg->arg2);
                lpc4088_usart_receive(s, &msg_bytes[12]);
            }
        }
    }
}

static const MemoryRegionOps lpc4088_usart_ops = {
    .read = lpc4088_usart_read,
    .write = lpc4088_usart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_lpc4088_usart = {
    .name = TYPE_LPC4088_USART,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(usart_RBR, LPC4088USARTState),
        VMSTATE_UINT32(usart_DLM, LPC4088USARTState),
        VMSTATE_UINT32(usart_IIR, LPC4088USARTState),
        VMSTATE_UINT32(usart_LCR, LPC4088USARTState),
        VMSTATE_UINT32(usart_LSR, LPC4088USARTState),
		VMSTATE_UINT32(usart_SCR, LPC4088USARTState),
		VMSTATE_UINT32(usart_ACR, LPC4088USARTState),
		VMSTATE_UINT32(usart_FDR, LPC4088USARTState),
		VMSTATE_UINT32(usart_TER, LPC4088USARTState),
		VMSTATE_UINT32(usart_RS485CTRL, LPC4088USARTState),
		VMSTATE_UINT32(usart_RS485ADRMATCH, LPC4088USARTState),
		VMSTATE_UINT32(usart_RS485DLY, LPC4088USARTState),
        VMSTATE_END_OF_LIST()
    }
};

static Property lpc4088_usart_properties[] = {
	DEFINE_PROP_STRING("usart-name", struct LPC4088USARTState, usart_name),
    DEFINE_PROP_BOOL("enable-rc", struct LPC4088USARTState, enable_rc, false),
    DEFINE_PROP_END_OF_LIST(),
};

static void lpc4088_usart_init(Object *obj) {
    LPC4088USARTState *s = LPC4088USART(obj);
	
	DeviceState *ds = DEVICE(obj);
	
	object_initialize_child_with_props(
        obj, "RemoteCtrl", &s->rcs,
        sizeof(RemoteCtrlState), TYPE_REMOTE_CTRL, &error_abort,
        NULL
    );
	
	s->rcs.connected_device = ds;

    s->fifo_read_ptr = s->fifo;
    s->fifo_write_ptr = s->fifo + 1;

    /*sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &lpc4088_usart_ops, s,
                          TYPE_LPC4088_USART, LPC4088_USART_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);*/
}

static void lpc4088_usart_realize(DeviceState *dev, Error **errp) {
    LPC4088USARTState *s = LPC4088USART(dev);
	
    s->send_char_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, lpc4088_usart_send_char_interrupt, s);

	sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);

    memory_region_init_io(&s->iomem, OBJECT(s), &lpc4088_usart_ops, s, TYPE_LPC4088_USART, LPC4088_USART_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

	s->rcs.callback = lpc4088_usart_remote_ctrl_callback;
    qdev_realize(DEVICE(&s->rcs), NULL, NULL);
}

static void lpc4088_usart_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_usart_reset;
    device_class_set_props(dc, lpc4088_usart_properties);
	dc->vmsd = &vmstate_lpc4088_usart;
    dc->realize = lpc4088_usart_realize;
}

static const TypeInfo lpc4088_usart_info = {
    .name          = TYPE_LPC4088_USART,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088USARTState),
    .instance_init = lpc4088_usart_init,
    .class_init    = lpc4088_usart_class_init,
};

static void lpc4088_usart_register_types(void) {
    type_register_static(&lpc4088_usart_info);
}

type_init(lpc4088_usart_register_types)
