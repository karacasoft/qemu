#ifndef __LPC4088_USART_H__
#define __LPC4088_USART_H__

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "hw/remotectrl/remotectrl.h"

#define TYPE_LPC4088_USART "lpc4088-usart"

#define LPC4088USART(obj) OBJECT_CHECK(LPC4088USARTState, (obj), TYPE_LPC4088_USART)

#define LPC4088_USART_MEM_SIZE 0x060

#define LPC4088_USART_REG_RBR 0x000
#define LPC4088_USART_REG_THR 0x000
#define LPC4088_USART_REG_DLL 0x000

#define LPC4088_USART_REG_DLM 0x004
#define LPC4088_USART_REG_IER 0x004

#define LPC4088_USART_REG_IIR 0x008
#define LPC4088_USART_REG_FCR 0x008

#define LPC4088_USART_REG_LCR 0x00C
#define LPC4088_USART_REG_LSR 0x014
#define LPC4088_USART_REG_SCR 0x01C
#define LPC4088_USART_REG_ACR 0x020
#define LPC4088_USART_REG_FDR 0x028
#define LPC4088_USART_REG_TER 0x030
#define LPC4088_USART_REG_RS485CTRL	0x04C
#define LPC4088_USART_REG_RS485ADRMATCH 0x50
#define LPC4088_USART_REG_RS485DLY 0x54


// TODO check the real FIFO size
#define LPC4088_USART_FIFO_SIZE 128


typedef struct LPC4088USARTState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion iomem;
	
	char *usart_name;
    bool enable_rc;
	uint32_t enableRemoteInterrupt;

	char fifo[LPC4088_USART_FIFO_SIZE];
	char *fifo_write_ptr;
	char *fifo_read_ptr;

	bool rls_interrupt_active;
	bool rda_interrupt_active;
	bool cti_interrupt_active;
	bool thre_interrupt_active;
	
    uint32_t usart_RBR;
	uint32_t usart_THR;
	uint32_t usart_DLL;

	uint32_t usart_DLM;
	uint32_t usart_IER;

	uint32_t usart_IIR;
	uint32_t usart_FCR;

	uint32_t usart_LCR;
	uint32_t usart_LSR;
	uint32_t usart_SCR;
	uint32_t usart_ACR;
	uint32_t usart_FDR;
	uint32_t usart_TER;
	uint32_t usart_RS485CTRL;
	uint32_t usart_RS485ADRMATCH;
	uint32_t usart_RS485DLY;

    qemu_irq irq;
	
	RemoteCtrlState rcs;
	
} LPC4088USARTState;
#endif
