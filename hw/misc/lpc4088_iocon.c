#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/misc/lpc4088_iocon.h"

#ifndef LPC4088_IOCON_ERROR_DEBUG
#define LPC4088_IOCON_ERROR_DEBUG 0
#endif

#define DEBUG_PRINT(fmt, args...) if(LPC4088_IOCON_ERROR_DEBUG) {fprintf(stderr, "[%s->%s]:" fmt, TYPE_LPC4088_IOCON,__func__, ##args);}

static void lpc4088_iocon_reset(DeviceState *dev) {
    LPC4088IOCONState *s = LPC4088IOCON(dev);
	
	s->iocon_P0_0 = 0x30;
	s->iocon_P0_1 = 0x30;
	s->iocon_P0_2 = 0x30;
	s->iocon_P0_3 = 0x30;
	s->iocon_P0_4 = 0x30;
	s->iocon_P0_5 = 0x30;
	s->iocon_P0_6 = 0x30;
	s->iocon_P0_7 = 0xA0;

	s->iocon_P0_8 = 0xA0;				/* 0x020 */
	s->iocon_P0_9 = 0xA0;
	s->iocon_P0_10 = 0x30;
	s->iocon_P0_11 = 0x30;
	s->iocon_P0_12 = 0x1B0;
	s->iocon_P0_13 = 0x1B0;
	s->iocon_P0_14 = 0x30;
	s->iocon_P0_15 = 0x30;

	s->iocon_P0_16 = 0x30;				/* 0x040 */
	s->iocon_P0_17 = 0x30;
	s->iocon_P0_18 = 0x30;
	s->iocon_P0_19 = 0x30;
	s->iocon_P0_20 = 0x30;
	s->iocon_P0_21 = 0x30;
	s->iocon_P0_22 = 0x30;
	s->iocon_P0_23 = 0x1B0;

	s->iocon_P0_24 = 0x1B0;				/* 0x060 */
	s->iocon_P0_25 = 0x1B0;
	s->iocon_P0_26 = 0x1B0;
	s->iocon_P0_27 = 0x00000000;
	s->iocon_P0_28 = 0x00000000;
	s->iocon_P0_29 = 0x00000000;
	s->iocon_P0_30 = 0x00000000;
	s->iocon_P0_31 = 0x00000000;

	s->iocon_P1_0 = 0x30;				/* 0x080 */
	s->iocon_P1_1 = 0x30;
	s->iocon_P1_2 = 0x30;
	s->iocon_P1_3 = 0x30;
	s->iocon_P1_4 = 0x30;
	s->iocon_P1_5 = 0xB0;
	s->iocon_P1_6 = 0xB0;
	s->iocon_P1_7 = 0xB0;

	s->iocon_P1_8 = 0x30;				/* 0x0A0 */
	s->iocon_P1_9 = 0x30;
	s->iocon_P1_10 = 0x30;
	s->iocon_P1_11 = 0x30;
	s->iocon_P1_12 = 0x30;
	s->iocon_P1_13 = 0x30;
	s->iocon_P1_14 = 0xB0;
	s->iocon_P1_15 = 0x30;

	s->iocon_P1_16 = 0xB0;				/* 0x0C0 */
	s->iocon_P1_17 = 0xB0;
	s->iocon_P1_18 = 0x30;
	s->iocon_P1_19 = 0x30;
	s->iocon_P1_20 = 0x30;
	s->iocon_P1_21 = 0x30;
	s->iocon_P1_22 = 0x30;
	s->iocon_P1_23 = 0x30;

	s->iocon_P1_24 = 0x30;				/* 0x0E0 */
	s->iocon_P1_25 = 0x30;
	s->iocon_P1_26 = 0x30;
	s->iocon_P1_27 = 0x30;
	s->iocon_P1_28 = 0x30;
	s->iocon_P1_29 = 0x30;
	s->iocon_P1_30 = 0x1B0;
	s->iocon_P1_31 = 0x1B0;

	s->iocon_P2_0 = 0x30;				/* 0x100 */
	s->iocon_P2_1 = 0x30;
	s->iocon_P2_2 = 0x30;
	s->iocon_P2_3 = 0x30;
	s->iocon_P2_4 = 0x30;
	s->iocon_P2_5 = 0x30;
	s->iocon_P2_6 = 0x30;
	s->iocon_P2_7 = 0x30;

	s->iocon_P2_8 = 0x30;				/* 0x120 */
	s->iocon_P2_9 = 0x30;
	s->iocon_P2_10 = 0x30;
	s->iocon_P2_11 = 0x30;
	s->iocon_P2_12 = 0x30;
	s->iocon_P2_13 = 0x30;
	s->iocon_P2_14 = 0x30;
	s->iocon_P2_15 = 0x30;

	s->iocon_P2_16 = 0x30;				/* 0x140 */
	s->iocon_P2_17 = 0x30;
	s->iocon_P2_18 = 0x30;
	s->iocon_P2_19 = 0x30;
	s->iocon_P2_20 = 0x30;
	s->iocon_P2_21 = 0x30;
	s->iocon_P2_22 = 0x30;
	s->iocon_P2_23 = 0x30;

	s->iocon_P2_24 = 0x30;				/* 0x160 */
	s->iocon_P2_25 = 0x30;
	s->iocon_P2_26 = 0x30;
	s->iocon_P2_27 = 0x30;
	s->iocon_P2_28 = 0x30;
	s->iocon_P2_29 = 0x30;
	s->iocon_P2_30 = 0x30;
	s->iocon_P2_31 = 0x30;

	s->iocon_P3_0 = 0x30;				/* 0x180 */
	s->iocon_P3_1 = 0x30;
	s->iocon_P3_2 = 0x30;
	s->iocon_P3_3 = 0x30;
	s->iocon_P3_4 = 0x30;
	s->iocon_P3_5 = 0x30;
	s->iocon_P3_6 = 0x30;
	s->iocon_P3_7 = 0x30;

	s->iocon_P3_8 = 0x30;				/* 0x1A0 */
	s->iocon_P3_9 = 0x30;
	s->iocon_P3_10 = 0x30;
	s->iocon_P3_11 = 0x30;
	s->iocon_P3_12 = 0x30;
	s->iocon_P3_13 = 0x30;
	s->iocon_P3_14 = 0x30;
	s->iocon_P3_15 = 0x30;

	s->iocon_P3_16 = 0x30;				/* 0x1C0 */
	s->iocon_P3_17 = 0x30;
	s->iocon_P3_18 = 0x30;
	s->iocon_P3_19 = 0x30;
	s->iocon_P3_20 = 0x30;
	s->iocon_P3_21 = 0x30;
	s->iocon_P3_22 = 0x30;
	s->iocon_P3_23 = 0x30;

	s->iocon_P3_24 = 0x30;				/* 0x1E0 */
	s->iocon_P3_25 = 0x30;
	s->iocon_P3_26 = 0x30;
	s->iocon_P3_27 = 0x30;
	s->iocon_P3_28 = 0x30;
	s->iocon_P3_29 = 0x30;
	s->iocon_P3_30 = 0x30;
	s->iocon_P3_31 = 0x30;

	s->iocon_P4_0 = 0x30;				/* 0x200 */
	s->iocon_P4_1 = 0x30;
	s->iocon_P4_2 = 0x30;
	s->iocon_P4_3 = 0x30;
	s->iocon_P4_4 = 0x30;
	s->iocon_P4_5 = 0x30;
	s->iocon_P4_6 = 0x30;
	s->iocon_P4_7 = 0x30;

	s->iocon_P4_8 = 0x30;				/* 0x220 */
	s->iocon_P4_9 = 0x30;
	s->iocon_P4_10 = 0x30;
	s->iocon_P4_11 = 0x30;
	s->iocon_P4_12 = 0x30;
	s->iocon_P4_13 = 0x30;
	s->iocon_P4_14 = 0x30;
	s->iocon_P4_15 = 0x30;

	s->iocon_P4_16 = 0x30;				/* 0x240 */
	s->iocon_P4_17 = 0x30;
	s->iocon_P4_18 = 0x30;
	s->iocon_P4_19 = 0x30;
	s->iocon_P4_20 = 0x30;
	s->iocon_P4_21 = 0x30;
	s->iocon_P4_22 = 0x30;
	s->iocon_P4_23 = 0x30;

	s->iocon_P4_24 = 0x30;				/* 0x260 */
	s->iocon_P4_25 = 0x30;
	s->iocon_P4_26 = 0x30;
	s->iocon_P4_27 = 0x30;
	s->iocon_P4_28 = 0x30;
	s->iocon_P4_29 = 0x30;
	s->iocon_P4_30 = 0x30;
	s->iocon_P4_31 = 0x30;

	s->iocon_P5_0 = 0x30;				/* 0x280 */
	s->iocon_P5_1 = 0x30;
	s->iocon_P5_2 = 0x00000000;
	s->iocon_P5_3 = 0x00000000;
	s->iocon_P5_4 = 0x30;				/* 0x290 */
}

static uint64_t lpc4088_iocon_read(void *opaque, hwaddr addr, unsigned int size) {
    LPC4088IOCONState *s = opaque;

    switch (addr) {
    case LPC4088_IOCON_REG_P0_0:
		return s->iocon_P0_0;
	case LPC4088_IOCON_REG_P0_1:
		return s->iocon_P0_1;
	case LPC4088_IOCON_REG_P0_2:
		return s->iocon_P0_2;
	case LPC4088_IOCON_REG_P0_3:
		return s->iocon_P0_3;
	case LPC4088_IOCON_REG_P0_4:
		return s->iocon_P0_4;
	case LPC4088_IOCON_REG_P0_5:
		return s->iocon_P0_5;
	case LPC4088_IOCON_REG_P0_6:
		return s->iocon_P0_6;
	case LPC4088_IOCON_REG_P0_7:
		return s->iocon_P0_7;
	case LPC4088_IOCON_REG_P0_8:
		return s->iocon_P0_8;
	case LPC4088_IOCON_REG_P0_9:
		return s->iocon_P0_9;
	case LPC4088_IOCON_REG_P0_10:
		return s->iocon_P0_10;
	case LPC4088_IOCON_REG_P0_11:
		return s->iocon_P0_11;
	case LPC4088_IOCON_REG_P0_12:
		return s->iocon_P0_12;
	case LPC4088_IOCON_REG_P0_13:
		return s->iocon_P0_13;
	case LPC4088_IOCON_REG_P0_14:
		return s->iocon_P0_14;
	case LPC4088_IOCON_REG_P0_15:
		return s->iocon_P0_15;
	case LPC4088_IOCON_REG_P0_16:
		return s->iocon_P0_16;
	case LPC4088_IOCON_REG_P0_17:
		return s->iocon_P0_17;
	case LPC4088_IOCON_REG_P0_18:
		return s->iocon_P0_18;
	case LPC4088_IOCON_REG_P0_19:
		return s->iocon_P0_19;
	case LPC4088_IOCON_REG_P0_20:
		return s->iocon_P0_20;
	case LPC4088_IOCON_REG_P0_21:
		return s->iocon_P0_21;
	case LPC4088_IOCON_REG_P0_22:
		return s->iocon_P0_22;
	case LPC4088_IOCON_REG_P0_23:
		return s->iocon_P0_23;
	case LPC4088_IOCON_REG_P0_24:
		return s->iocon_P0_24;
	case LPC4088_IOCON_REG_P0_25:
		return s->iocon_P0_25;
	case LPC4088_IOCON_REG_P0_26:
		return s->iocon_P0_26;
	case LPC4088_IOCON_REG_P0_27:
		return s->iocon_P0_27;
	case LPC4088_IOCON_REG_P0_28:
		return s->iocon_P0_28;
	case LPC4088_IOCON_REG_P0_29:
		return s->iocon_P0_29;
	case LPC4088_IOCON_REG_P0_30:
		return s->iocon_P0_30;
	case LPC4088_IOCON_REG_P0_31:
		return s->iocon_P0_31;
	case LPC4088_IOCON_REG_P1_0:
		return s->iocon_P1_0;
	case LPC4088_IOCON_REG_P1_1:
		return s->iocon_P1_1;
	case LPC4088_IOCON_REG_P1_2:
		return s->iocon_P1_2;
	case LPC4088_IOCON_REG_P1_3:
		return s->iocon_P1_3;
	case LPC4088_IOCON_REG_P1_4:
		return s->iocon_P1_4;
	case LPC4088_IOCON_REG_P1_5:
		return s->iocon_P1_5;
	case LPC4088_IOCON_REG_P1_6:
		return s->iocon_P1_6;
	case LPC4088_IOCON_REG_P1_7:
		return s->iocon_P1_7;
	case LPC4088_IOCON_REG_P1_8:
		return s->iocon_P1_8;
	case LPC4088_IOCON_REG_P1_9:
		return s->iocon_P1_9;
	case LPC4088_IOCON_REG_P1_10:
		return s->iocon_P1_10;
	case LPC4088_IOCON_REG_P1_11:
		return s->iocon_P1_11;
	case LPC4088_IOCON_REG_P1_12:
		return s->iocon_P1_12;
	case LPC4088_IOCON_REG_P1_13:
		return s->iocon_P1_13;
	case LPC4088_IOCON_REG_P1_14:
		return s->iocon_P1_14;
	case LPC4088_IOCON_REG_P1_15:
		return s->iocon_P1_15;
	case LPC4088_IOCON_REG_P1_16:
		return s->iocon_P1_16;
	case LPC4088_IOCON_REG_P1_17:
		return s->iocon_P1_17;
	case LPC4088_IOCON_REG_P1_18:
		return s->iocon_P1_18;
	case LPC4088_IOCON_REG_P1_19:
		return s->iocon_P1_19;
	case LPC4088_IOCON_REG_P1_20:
		return s->iocon_P1_20;
	case LPC4088_IOCON_REG_P1_21:
		return s->iocon_P1_21;
	case LPC4088_IOCON_REG_P1_22:
		return s->iocon_P1_22;
	case LPC4088_IOCON_REG_P1_23:
		return s->iocon_P1_23;
	case LPC4088_IOCON_REG_P1_24:
		return s->iocon_P1_24;
	case LPC4088_IOCON_REG_P1_25:
		return s->iocon_P1_25;
	case LPC4088_IOCON_REG_P1_26:
		return s->iocon_P1_26;
	case LPC4088_IOCON_REG_P1_27:
		return s->iocon_P1_27;
	case LPC4088_IOCON_REG_P1_28:
		return s->iocon_P1_28;
	case LPC4088_IOCON_REG_P1_29:
		return s->iocon_P1_29;
	case LPC4088_IOCON_REG_P1_30:
		return s->iocon_P1_30;
	case LPC4088_IOCON_REG_P1_31:
		return s->iocon_P1_31;
	case LPC4088_IOCON_REG_P2_0:
		return s->iocon_P2_0;
	case LPC4088_IOCON_REG_P2_1:
		return s->iocon_P2_1;
	case LPC4088_IOCON_REG_P2_2:
		return s->iocon_P2_2;
	case LPC4088_IOCON_REG_P2_3:
		return s->iocon_P2_3;
	case LPC4088_IOCON_REG_P2_4:
		return s->iocon_P2_4;
	case LPC4088_IOCON_REG_P2_5:
		return s->iocon_P2_5;
	case LPC4088_IOCON_REG_P2_6:
		return s->iocon_P2_6;
	case LPC4088_IOCON_REG_P2_7:
		return s->iocon_P2_7;
	case LPC4088_IOCON_REG_P2_8:
		return s->iocon_P2_8;
	case LPC4088_IOCON_REG_P2_9:
		return s->iocon_P2_9;
	case LPC4088_IOCON_REG_P2_10:
		return s->iocon_P2_10;
	case LPC4088_IOCON_REG_P2_11:
		return s->iocon_P2_11;
	case LPC4088_IOCON_REG_P2_12:
		return s->iocon_P2_12;
	case LPC4088_IOCON_REG_P2_13:
		return s->iocon_P2_13;
	case LPC4088_IOCON_REG_P2_14:
		return s->iocon_P2_14;
	case LPC4088_IOCON_REG_P2_15:
		return s->iocon_P2_15;
	case LPC4088_IOCON_REG_P2_16:
		return s->iocon_P2_16;
	case LPC4088_IOCON_REG_P2_17:
		return s->iocon_P2_17;
	case LPC4088_IOCON_REG_P2_18:
		return s->iocon_P2_18;
	case LPC4088_IOCON_REG_P2_19:
		return s->iocon_P2_19;
	case LPC4088_IOCON_REG_P2_20:
		return s->iocon_P2_20;
	case LPC4088_IOCON_REG_P2_21:
		return s->iocon_P2_21;
	case LPC4088_IOCON_REG_P2_22:
		return s->iocon_P2_22;
	case LPC4088_IOCON_REG_P2_23:
		return s->iocon_P2_23;
	case LPC4088_IOCON_REG_P2_24:
		return s->iocon_P2_24;
	case LPC4088_IOCON_REG_P2_25:
		return s->iocon_P2_25;
	case LPC4088_IOCON_REG_P2_26:
		return s->iocon_P2_26;
	case LPC4088_IOCON_REG_P2_27:
		return s->iocon_P2_27;
	case LPC4088_IOCON_REG_P2_28:
		return s->iocon_P2_28;
	case LPC4088_IOCON_REG_P2_29:
		return s->iocon_P2_29;
	case LPC4088_IOCON_REG_P2_30:
		return s->iocon_P2_30;
	case LPC4088_IOCON_REG_P2_31:
		return s->iocon_P2_31;
	case LPC4088_IOCON_REG_P3_0:
		return s->iocon_P3_0;
	case LPC4088_IOCON_REG_P3_1:
		return s->iocon_P3_1;
	case LPC4088_IOCON_REG_P3_2:
		return s->iocon_P3_2;
	case LPC4088_IOCON_REG_P3_3:
		return s->iocon_P3_3;
	case LPC4088_IOCON_REG_P3_4:
		return s->iocon_P3_4;
	case LPC4088_IOCON_REG_P3_5:
		return s->iocon_P3_5;
	case LPC4088_IOCON_REG_P3_6:
		return s->iocon_P3_6;
	case LPC4088_IOCON_REG_P3_7:
		return s->iocon_P3_7;
	case LPC4088_IOCON_REG_P3_8:
		return s->iocon_P3_8;
	case LPC4088_IOCON_REG_P3_9:
		return s->iocon_P3_9;
	case LPC4088_IOCON_REG_P3_10:
		return s->iocon_P3_10;
	case LPC4088_IOCON_REG_P3_11:
		return s->iocon_P3_11;
	case LPC4088_IOCON_REG_P3_12:
		return s->iocon_P3_12;
	case LPC4088_IOCON_REG_P3_13:
		return s->iocon_P3_13;
	case LPC4088_IOCON_REG_P3_14:
		return s->iocon_P3_14;
	case LPC4088_IOCON_REG_P3_15:
		return s->iocon_P3_15;
	case LPC4088_IOCON_REG_P3_16:
		return s->iocon_P3_16;
	case LPC4088_IOCON_REG_P3_17:
		return s->iocon_P3_17;
	case LPC4088_IOCON_REG_P3_18:
		return s->iocon_P3_18;
	case LPC4088_IOCON_REG_P3_19:
		return s->iocon_P3_19;
	case LPC4088_IOCON_REG_P3_20:
		return s->iocon_P3_20;
	case LPC4088_IOCON_REG_P3_21:
		return s->iocon_P3_21;
	case LPC4088_IOCON_REG_P3_22:
		return s->iocon_P3_22;
	case LPC4088_IOCON_REG_P3_23:
		return s->iocon_P3_23;
	case LPC4088_IOCON_REG_P3_24:
		return s->iocon_P3_24;
	case LPC4088_IOCON_REG_P3_25:
		return s->iocon_P3_25;
	case LPC4088_IOCON_REG_P3_26:
		return s->iocon_P3_26;
	case LPC4088_IOCON_REG_P3_27:
		return s->iocon_P3_27;
	case LPC4088_IOCON_REG_P3_28:
		return s->iocon_P3_28;
	case LPC4088_IOCON_REG_P3_29:
		return s->iocon_P3_29;
	case LPC4088_IOCON_REG_P3_30:
		return s->iocon_P3_30;
	case LPC4088_IOCON_REG_P3_31:
		return s->iocon_P3_31;
	case LPC4088_IOCON_REG_P4_0:
		return s->iocon_P4_0;
	case LPC4088_IOCON_REG_P4_1:
		return s->iocon_P4_1;
	case LPC4088_IOCON_REG_P4_2:
		return s->iocon_P4_2;
	case LPC4088_IOCON_REG_P4_3:
		return s->iocon_P4_3;
	case LPC4088_IOCON_REG_P4_4:
		return s->iocon_P4_4;
	case LPC4088_IOCON_REG_P4_5:
		return s->iocon_P4_5;
	case LPC4088_IOCON_REG_P4_6:
		return s->iocon_P4_6;
	case LPC4088_IOCON_REG_P4_7:
		return s->iocon_P4_7;
	case LPC4088_IOCON_REG_P4_8:
		return s->iocon_P4_8;
	case LPC4088_IOCON_REG_P4_9:
		return s->iocon_P4_9;
	case LPC4088_IOCON_REG_P4_10:
		return s->iocon_P4_10;
	case LPC4088_IOCON_REG_P4_11:
		return s->iocon_P4_11;
	case LPC4088_IOCON_REG_P4_12:
		return s->iocon_P4_12;
	case LPC4088_IOCON_REG_P4_13:
		return s->iocon_P4_13;
	case LPC4088_IOCON_REG_P4_14:
		return s->iocon_P4_14;
	case LPC4088_IOCON_REG_P4_15:
		return s->iocon_P4_15;
	case LPC4088_IOCON_REG_P4_16:
		return s->iocon_P4_16;
	case LPC4088_IOCON_REG_P4_17:
		return s->iocon_P4_17;
	case LPC4088_IOCON_REG_P4_18:
		return s->iocon_P4_18;
	case LPC4088_IOCON_REG_P4_19:
		return s->iocon_P4_19;
	case LPC4088_IOCON_REG_P4_20:
		return s->iocon_P4_20;
	case LPC4088_IOCON_REG_P4_21:
		return s->iocon_P4_21;
	case LPC4088_IOCON_REG_P4_22:
		return s->iocon_P4_22;
	case LPC4088_IOCON_REG_P4_23:
		return s->iocon_P4_23;
	case LPC4088_IOCON_REG_P4_24:
		return s->iocon_P4_24;
	case LPC4088_IOCON_REG_P4_25:
		return s->iocon_P4_25;
	case LPC4088_IOCON_REG_P4_26:
		return s->iocon_P4_26;
	case LPC4088_IOCON_REG_P4_27:
		return s->iocon_P4_27;
	case LPC4088_IOCON_REG_P4_28:
		return s->iocon_P4_28;
	case LPC4088_IOCON_REG_P4_29:
		return s->iocon_P4_29;
	case LPC4088_IOCON_REG_P4_30:
		return s->iocon_P4_30;
	case LPC4088_IOCON_REG_P4_31:
		return s->iocon_P4_31;
	case LPC4088_IOCON_REG_P5_0:
		return s->iocon_P5_0;
	case LPC4088_IOCON_REG_P5_1:
		return s->iocon_P5_1;
	case LPC4088_IOCON_REG_P5_2:
		return s->iocon_P5_2;
	case LPC4088_IOCON_REG_P5_3:
		return s->iocon_P5_3;
	case LPC4088_IOCON_REG_P5_4:
		return s->iocon_P5_4;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }

    return 0;
}

static void lpc4088_iocon_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size) {
    LPC4088IOCONState *s = opaque;
    uint32_t value = (uint32_t) val64;
			 
	DEBUG_PRINT("(%s, value = 0x%" PRIx32 ")\n","IOCON_Write",(uint32_t) value);

    switch (addr) {
    case LPC4088_IOCON_REG_P0_0:
		s->iocon_P0_0 = value;
		break;
	case LPC4088_IOCON_REG_P0_1:
		s->iocon_P0_1 = value;
		break;
	case LPC4088_IOCON_REG_P0_2:
		s->iocon_P0_2 = value;
		break;
	case LPC4088_IOCON_REG_P0_3:
		s->iocon_P0_3 = value;
		break;
	case LPC4088_IOCON_REG_P0_4:
		s->iocon_P0_4 = value;
		break;
	case LPC4088_IOCON_REG_P0_5:
		s->iocon_P0_5 = value;
		break;
	case LPC4088_IOCON_REG_P0_6:
		s->iocon_P0_6 = value;
		break;
	case LPC4088_IOCON_REG_P0_7:
		s->iocon_P0_7 = value;
		break;
	case LPC4088_IOCON_REG_P0_8:
		s->iocon_P0_8 = value;
		break;
	case LPC4088_IOCON_REG_P0_9:
		s->iocon_P0_9 = value;
		break;
	case LPC4088_IOCON_REG_P0_10:
		s->iocon_P0_10 = value;
		break;
	case LPC4088_IOCON_REG_P0_11:
		s->iocon_P0_11 = value;
		break;
	case LPC4088_IOCON_REG_P0_12:
		s->iocon_P0_12 = value;
		break;
	case LPC4088_IOCON_REG_P0_13:
		s->iocon_P0_13 = value;
		break;
	case LPC4088_IOCON_REG_P0_14:
		s->iocon_P0_14 = value;
		break;
	case LPC4088_IOCON_REG_P0_15:
		s->iocon_P0_15 = value;
		break;
	case LPC4088_IOCON_REG_P0_16:
		s->iocon_P0_16 = value;
		break;
	case LPC4088_IOCON_REG_P0_17:
		s->iocon_P0_17 = value;
		break;
	case LPC4088_IOCON_REG_P0_18:
		s->iocon_P0_18 = value;
		break;
	case LPC4088_IOCON_REG_P0_19:
		s->iocon_P0_19 = value;
		break;
	case LPC4088_IOCON_REG_P0_20:
		s->iocon_P0_20 = value;
		break;
	case LPC4088_IOCON_REG_P0_21:
		s->iocon_P0_21 = value;
		break;
	case LPC4088_IOCON_REG_P0_22:
		s->iocon_P0_22 = value;
		break;
	case LPC4088_IOCON_REG_P0_23:
		s->iocon_P0_23 = value;
		break;
	case LPC4088_IOCON_REG_P0_24:
		s->iocon_P0_24 = value;
		break;
	case LPC4088_IOCON_REG_P0_25:
		s->iocon_P0_25 = value;
		break;
	case LPC4088_IOCON_REG_P0_26:
		s->iocon_P0_26 = value;
		break;
	case LPC4088_IOCON_REG_P0_27:
		s->iocon_P0_27 = value;
		break;
	case LPC4088_IOCON_REG_P0_28:
		s->iocon_P0_28 = value;
		break;
	case LPC4088_IOCON_REG_P0_29:
		s->iocon_P0_29 = value;
		break;
	case LPC4088_IOCON_REG_P0_30:
		s->iocon_P0_30 = value;
		break;
	case LPC4088_IOCON_REG_P0_31:
		s->iocon_P0_31 = value;
		break;
	case LPC4088_IOCON_REG_P1_0:
		s->iocon_P1_0 = value;
		break;
	case LPC4088_IOCON_REG_P1_1:
		s->iocon_P1_1 = value;
		break;
	case LPC4088_IOCON_REG_P1_2:
		s->iocon_P1_2 = value;
		break;
	case LPC4088_IOCON_REG_P1_3:
		s->iocon_P1_3 = value;
		break;
	case LPC4088_IOCON_REG_P1_4:
		s->iocon_P1_4 = value;
		break;
	case LPC4088_IOCON_REG_P1_5:
		s->iocon_P1_5 = value;
		break;
	case LPC4088_IOCON_REG_P1_6:
		s->iocon_P1_6 = value;
		break;
	case LPC4088_IOCON_REG_P1_7:
		s->iocon_P1_7 = value;
		break;
	case LPC4088_IOCON_REG_P1_8:
		s->iocon_P1_8 = value;
		break;
	case LPC4088_IOCON_REG_P1_9:
		s->iocon_P1_9 = value;
		break;
	case LPC4088_IOCON_REG_P1_10:
		s->iocon_P1_10 = value;
		break;
	case LPC4088_IOCON_REG_P1_11:
		s->iocon_P1_11 = value;
		break;
	case LPC4088_IOCON_REG_P1_12:
		s->iocon_P1_12 = value;
		break;
	case LPC4088_IOCON_REG_P1_13:
		s->iocon_P1_13 = value;
		break;
	case LPC4088_IOCON_REG_P1_14:
		s->iocon_P1_14 = value;
		break;
	case LPC4088_IOCON_REG_P1_15:
		s->iocon_P1_15 = value;
		break;
	case LPC4088_IOCON_REG_P1_16:
		s->iocon_P1_16 = value;
		break;
	case LPC4088_IOCON_REG_P1_17:
		s->iocon_P1_17 = value;
		break;
	case LPC4088_IOCON_REG_P1_18:
		s->iocon_P1_18 = value;
		break;
	case LPC4088_IOCON_REG_P1_19:
		s->iocon_P1_19 = value;
		break;
	case LPC4088_IOCON_REG_P1_20:
		s->iocon_P1_20 = value;
		break;
	case LPC4088_IOCON_REG_P1_21:
		s->iocon_P1_21 = value;
		break;
	case LPC4088_IOCON_REG_P1_22:
		s->iocon_P1_22 = value;
		break;
	case LPC4088_IOCON_REG_P1_23:
		s->iocon_P1_23 = value;
		break;
	case LPC4088_IOCON_REG_P1_24:
		s->iocon_P1_24 = value;
		break;
	case LPC4088_IOCON_REG_P1_25:
		s->iocon_P1_25 = value;
		break;
	case LPC4088_IOCON_REG_P1_26:
		s->iocon_P1_26 = value;
		break;
	case LPC4088_IOCON_REG_P1_27:
		s->iocon_P1_27 = value;
		break;
	case LPC4088_IOCON_REG_P1_28:
		s->iocon_P1_28 = value;
		break;
	case LPC4088_IOCON_REG_P1_29:
		s->iocon_P1_29 = value;
		break;
	case LPC4088_IOCON_REG_P1_30:
		s->iocon_P1_30 = value;
		break;
	case LPC4088_IOCON_REG_P1_31:
		s->iocon_P1_31 = value;
		break;
	case LPC4088_IOCON_REG_P2_0:
		s->iocon_P2_0 = value;
		break;
	case LPC4088_IOCON_REG_P2_1:
		s->iocon_P2_1 = value;
		break;
	case LPC4088_IOCON_REG_P2_2:
		s->iocon_P2_2 = value;
		break;
	case LPC4088_IOCON_REG_P2_3:
		s->iocon_P2_3 = value;
		break;
	case LPC4088_IOCON_REG_P2_4:
		s->iocon_P2_4 = value;
		break;
	case LPC4088_IOCON_REG_P2_5:
		s->iocon_P2_5 = value;
		break;
	case LPC4088_IOCON_REG_P2_6:
		s->iocon_P2_6 = value;
		break;
	case LPC4088_IOCON_REG_P2_7:
		s->iocon_P2_7 = value;
		break;
	case LPC4088_IOCON_REG_P2_8:
		s->iocon_P2_8 = value;
		break;
	case LPC4088_IOCON_REG_P2_9:
		s->iocon_P2_9 = value;
		break;
	case LPC4088_IOCON_REG_P2_10:
		s->iocon_P2_10 = value;
		break;
	case LPC4088_IOCON_REG_P2_11:
		s->iocon_P2_11 = value;
		break;
	case LPC4088_IOCON_REG_P2_12:
		s->iocon_P2_12 = value;
		break;
	case LPC4088_IOCON_REG_P2_13:
		s->iocon_P2_13 = value;
		break;
	case LPC4088_IOCON_REG_P2_14:
		s->iocon_P2_14 = value;
		break;
	case LPC4088_IOCON_REG_P2_15:
		s->iocon_P2_15 = value;
		break;
	case LPC4088_IOCON_REG_P2_16:
		s->iocon_P2_16 = value;
		break;
	case LPC4088_IOCON_REG_P2_17:
		s->iocon_P2_17 = value;
		break;
	case LPC4088_IOCON_REG_P2_18:
		s->iocon_P2_18 = value;
		break;
	case LPC4088_IOCON_REG_P2_19:
		s->iocon_P2_19 = value;
		break;
	case LPC4088_IOCON_REG_P2_20:
		s->iocon_P2_20 = value;
		break;
	case LPC4088_IOCON_REG_P2_21:
		s->iocon_P2_21 = value;
		break;
	case LPC4088_IOCON_REG_P2_22:
		s->iocon_P2_22 = value;
		break;
	case LPC4088_IOCON_REG_P2_23:
		s->iocon_P2_23 = value;
		break;
	case LPC4088_IOCON_REG_P2_24:
		s->iocon_P2_24 = value;
		break;
	case LPC4088_IOCON_REG_P2_25:
		s->iocon_P2_25 = value;
		break;
	case LPC4088_IOCON_REG_P2_26:
		s->iocon_P2_26 = value;
		break;
	case LPC4088_IOCON_REG_P2_27:
		s->iocon_P2_27 = value;
		break;
	case LPC4088_IOCON_REG_P2_28:
		s->iocon_P2_28 = value;
		break;
	case LPC4088_IOCON_REG_P2_29:
		s->iocon_P2_29 = value;
		break;
	case LPC4088_IOCON_REG_P2_30:
		s->iocon_P2_30 = value;
		break;
	case LPC4088_IOCON_REG_P2_31:
		s->iocon_P2_31 = value;
		break;
	case LPC4088_IOCON_REG_P3_0:
		s->iocon_P3_0 = value;
		break;
	case LPC4088_IOCON_REG_P3_1:
		s->iocon_P3_1 = value;
		break;
	case LPC4088_IOCON_REG_P3_2:
		s->iocon_P3_2 = value;
		break;
	case LPC4088_IOCON_REG_P3_3:
		s->iocon_P3_3 = value;
		break;
	case LPC4088_IOCON_REG_P3_4:
		s->iocon_P3_4 = value;
		break;
	case LPC4088_IOCON_REG_P3_5:
		s->iocon_P3_5 = value;
		break;
	case LPC4088_IOCON_REG_P3_6:
		s->iocon_P3_6 = value;
		break;
	case LPC4088_IOCON_REG_P3_7:
		s->iocon_P3_7 = value;
		break;
	case LPC4088_IOCON_REG_P3_8:
		s->iocon_P3_8 = value;
		break;
	case LPC4088_IOCON_REG_P3_9:
		s->iocon_P3_9 = value;
		break;
	case LPC4088_IOCON_REG_P3_10:
		s->iocon_P3_10 = value;
		break;
	case LPC4088_IOCON_REG_P3_11:
		s->iocon_P3_11 = value;
		break;
	case LPC4088_IOCON_REG_P3_12:
		s->iocon_P3_12 = value;
		break;
	case LPC4088_IOCON_REG_P3_13:
		s->iocon_P3_13 = value;
		break;
	case LPC4088_IOCON_REG_P3_14:
		s->iocon_P3_14 = value;
		break;
	case LPC4088_IOCON_REG_P3_15:
		s->iocon_P3_15 = value;
		break;
	case LPC4088_IOCON_REG_P3_16:
		s->iocon_P3_16 = value;
		break;
	case LPC4088_IOCON_REG_P3_17:
		s->iocon_P3_17 = value;
		break;
	case LPC4088_IOCON_REG_P3_18:
		s->iocon_P3_18 = value;
		break;
	case LPC4088_IOCON_REG_P3_19:
		s->iocon_P3_19 = value;
		break;
	case LPC4088_IOCON_REG_P3_20:
		s->iocon_P3_20 = value;
		break;
	case LPC4088_IOCON_REG_P3_21:
		s->iocon_P3_21 = value;
		break;
	case LPC4088_IOCON_REG_P3_22:
		s->iocon_P3_22 = value;
		break;
	case LPC4088_IOCON_REG_P3_23:
		s->iocon_P3_23 = value;
		break;
	case LPC4088_IOCON_REG_P3_24:
		s->iocon_P3_24 = value;
		break;
	case LPC4088_IOCON_REG_P3_25:
		s->iocon_P3_25 = value;
		break;
	case LPC4088_IOCON_REG_P3_26:
		s->iocon_P3_26 = value;
		break;
	case LPC4088_IOCON_REG_P3_27:
		s->iocon_P3_27 = value;
		break;
	case LPC4088_IOCON_REG_P3_28:
		s->iocon_P3_28 = value;
		break;
	case LPC4088_IOCON_REG_P3_29:
		s->iocon_P3_29 = value;
		break;
	case LPC4088_IOCON_REG_P3_30:
		s->iocon_P3_30 = value;
		break;
	case LPC4088_IOCON_REG_P3_31:
		s->iocon_P3_31 = value;
		break;
	case LPC4088_IOCON_REG_P4_0:
		s->iocon_P4_0 = value;
		break;
	case LPC4088_IOCON_REG_P4_1:
		s->iocon_P4_1 = value;
		break;
	case LPC4088_IOCON_REG_P4_2:
		s->iocon_P4_2 = value;
		break;
	case LPC4088_IOCON_REG_P4_3:
		s->iocon_P4_3 = value;
		break;
	case LPC4088_IOCON_REG_P4_4:
		s->iocon_P4_4 = value;
		break;
	case LPC4088_IOCON_REG_P4_5:
		s->iocon_P4_5 = value;
		break;
	case LPC4088_IOCON_REG_P4_6:
		s->iocon_P4_6 = value;
		break;
	case LPC4088_IOCON_REG_P4_7:
		s->iocon_P4_7 = value;
		break;
	case LPC4088_IOCON_REG_P4_8:
		s->iocon_P4_8 = value;
		break;
	case LPC4088_IOCON_REG_P4_9:
		s->iocon_P4_9 = value;
		break;
	case LPC4088_IOCON_REG_P4_10:
		s->iocon_P4_10 = value;
		break;
	case LPC4088_IOCON_REG_P4_11:
		s->iocon_P4_11 = value;
		break;
	case LPC4088_IOCON_REG_P4_12:
		s->iocon_P4_12 = value;
		break;
	case LPC4088_IOCON_REG_P4_13:
		s->iocon_P4_13 = value;
		break;
	case LPC4088_IOCON_REG_P4_14:
		s->iocon_P4_14 = value;
		break;
	case LPC4088_IOCON_REG_P4_15:
		s->iocon_P4_15 = value;
		break;
	case LPC4088_IOCON_REG_P4_16:
		s->iocon_P4_16 = value;
		break;
	case LPC4088_IOCON_REG_P4_17:
		s->iocon_P4_17 = value;
		break;
	case LPC4088_IOCON_REG_P4_18:
		s->iocon_P4_18 = value;
		break;
	case LPC4088_IOCON_REG_P4_19:
		s->iocon_P4_19 = value;
		break;
	case LPC4088_IOCON_REG_P4_20:
		s->iocon_P4_20 = value;
		break;
	case LPC4088_IOCON_REG_P4_21:
		s->iocon_P4_21 = value;
		break;
	case LPC4088_IOCON_REG_P4_22:
		s->iocon_P4_22 = value;
		break;
	case LPC4088_IOCON_REG_P4_23:
		s->iocon_P4_23 = value;
		break;
	case LPC4088_IOCON_REG_P4_24:
		s->iocon_P4_24 = value;
		break;
	case LPC4088_IOCON_REG_P4_25:
		s->iocon_P4_25 = value;
		break;
	case LPC4088_IOCON_REG_P4_26:
		s->iocon_P4_26 = value;
		break;
	case LPC4088_IOCON_REG_P4_27:
		s->iocon_P4_27 = value;
		break;
	case LPC4088_IOCON_REG_P4_28:
		s->iocon_P4_28 = value;
		break;
	case LPC4088_IOCON_REG_P4_29:
		s->iocon_P4_29 = value;
		break;
	case LPC4088_IOCON_REG_P4_30:
		s->iocon_P4_30 = value;
		break;
	case LPC4088_IOCON_REG_P4_31:
		s->iocon_P4_31 = value;
		break;
	case LPC4088_IOCON_REG_P5_0:
		s->iocon_P5_0 = value;
		break;
	case LPC4088_IOCON_REG_P5_1:
		s->iocon_P5_1 = value;
		break;
	case LPC4088_IOCON_REG_P5_2:
		s->iocon_P5_2 = value;
		break;
	case LPC4088_IOCON_REG_P5_3:
		s->iocon_P5_3 = value;
		break;
	case LPC4088_IOCON_REG_P5_4:
		s->iocon_P5_4 = value;
		break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps lpc4088_iocon_ops = {
    .read = lpc4088_iocon_read,
    .write = lpc4088_iocon_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .impl.min_access_size = 4,
    .impl.max_access_size = 4,
};

static const VMStateDescription vmstate_lpc4088_iocon = {
    .name = TYPE_LPC4088_IOCON,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(iocon_P0_0, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_1, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_2, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_3, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_4, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_5, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_6, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_7, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P0_8, LPC4088IOCONState),				/* 0x020 */
		VMSTATE_UINT32(iocon_P0_9, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_10, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_11, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_12, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_13, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_14, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_15, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P0_16, LPC4088IOCONState),				/* 0x040 */
		VMSTATE_UINT32(iocon_P0_17, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_18, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_19, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_20, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_21, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_22, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_23, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P0_24, LPC4088IOCONState),				/* 0x060 */
		VMSTATE_UINT32(iocon_P0_25, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_26, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_27, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_28, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_29, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_30, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P0_31, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P1_0, LPC4088IOCONState),				/* 0x080 */
		VMSTATE_UINT32(iocon_P1_1, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_2, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_3, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_4, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_5, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_6, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_7, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P1_8, LPC4088IOCONState),				/* 0x0A0 */
		VMSTATE_UINT32(iocon_P1_9, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_10, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_11, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_12, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_13, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_14, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_15, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P1_16, LPC4088IOCONState),				/* 0x0C0 */
		VMSTATE_UINT32(iocon_P1_17, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_18, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_19, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_20, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_21, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_22, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_23, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P1_24, LPC4088IOCONState),				/* 0x0E0 */
		VMSTATE_UINT32(iocon_P1_25, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_26, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_27, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_28, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_29, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_30, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P1_31, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P2_0, LPC4088IOCONState),				/* 0x100 */
		VMSTATE_UINT32(iocon_P2_1, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_2, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_3, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_4, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_5, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_6, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_7, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P2_8, LPC4088IOCONState),				/* 0x120 */
		VMSTATE_UINT32(iocon_P2_9, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_10, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_11, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_12, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_13, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_14, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_15, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P2_16, LPC4088IOCONState),				/* 0x140 */
		VMSTATE_UINT32(iocon_P2_17, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_18, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_19, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_20, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_21, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_22, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_23, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P2_24, LPC4088IOCONState),				/* 0x160 */
		VMSTATE_UINT32(iocon_P2_25, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_26, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_27, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_28, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_29, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_30, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P2_31, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P3_0, LPC4088IOCONState),				/* 0x180 */
		VMSTATE_UINT32(iocon_P3_1, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_2, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_3, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_4, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_5, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_6, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_7, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P3_8, LPC4088IOCONState),				/* 0x1A0 */
		VMSTATE_UINT32(iocon_P3_9, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_10, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_11, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_12, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_13, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_14, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_15, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P3_16, LPC4088IOCONState),				/* 0x1C0 */
		VMSTATE_UINT32(iocon_P3_17, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_18, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_19, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_20, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_21, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_22, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_23, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P3_24, LPC4088IOCONState),				/* 0x1E0 */
		VMSTATE_UINT32(iocon_P3_25, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_26, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_27, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_28, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_29, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_30, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P3_31, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P4_0, LPC4088IOCONState),				/* 0x200 */
		VMSTATE_UINT32(iocon_P4_1, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_2, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_3, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_4, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_5, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_6, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_7, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P4_8, LPC4088IOCONState),				/* 0x220 */
		VMSTATE_UINT32(iocon_P4_9, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_10, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_11, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_12, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_13, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_14, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_15, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P4_16, LPC4088IOCONState),				/* 0x240 */
		VMSTATE_UINT32(iocon_P4_17, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_18, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_19, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_20, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_21, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_22, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_23, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P4_24, LPC4088IOCONState),				/* 0x260 */
		VMSTATE_UINT32(iocon_P4_25, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_26, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_27, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_28, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_29, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_30, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P4_31, LPC4088IOCONState),

		VMSTATE_UINT32(iocon_P5_0, LPC4088IOCONState),				/* 0x280 */
		VMSTATE_UINT32(iocon_P5_1, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P5_2, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P5_3, LPC4088IOCONState),
		VMSTATE_UINT32(iocon_P5_4, LPC4088IOCONState),				/* 0x290 */
        VMSTATE_END_OF_LIST()
    }
};

static void lpc4088_iocon_init(Object *obj) {
    LPC4088IOCONState *s = LPC4088IOCON(obj);

    memory_region_init_io(&s->mmio, obj, &lpc4088_iocon_ops, s,TYPE_LPC4088_IOCON, LPC4088_IOCON_MEM_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void lpc4088_iocon_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lpc4088_iocon_reset;
    dc->vmsd = &vmstate_lpc4088_iocon;
}

static const TypeInfo lpc4088_iocon_info = {
    .name          = TYPE_LPC4088_IOCON,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LPC4088IOCONState),
    .instance_init = lpc4088_iocon_init,
    .class_init    = lpc4088_iocon_class_init,
};

static void lpc4088_iocon_register_types(void) {
    type_register_static(&lpc4088_iocon_info);
}

type_init(lpc4088_iocon_register_types)
