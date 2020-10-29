#ifndef __LPC4088_IOCON_H__
#define __LPC4088_IOCON_H__

#include "hw/sysbus.h"
#include "hw/remotectrl/remotectrl.h"

#define TYPE_LPC4088_IOCON "lpc4088-iocon"

#define LPC4088_IOCON(obj) OBJECT_CHECK(LPC4088IOCONState, (obj), TYPE_LPC4088_IOCON)

#define LPC4088_IOCON_BASE_ADDR 0x4002C000

#define LPC4088_IOCON_MEM_SIZE 0x300

#define LPC4088_IOCON_REG_P0_0  0x000
#define LPC4088_IOCON_REG_P0_1  0x004
#define LPC4088_IOCON_REG_P0_2  0x008
#define LPC4088_IOCON_REG_P0_3  0x00C
#define LPC4088_IOCON_REG_P0_4  0x010
#define LPC4088_IOCON_REG_P0_5  0x014
#define LPC4088_IOCON_REG_P0_6  0x018
#define LPC4088_IOCON_REG_P0_7  0x01C
#define LPC4088_IOCON_REG_P0_8  0x020
#define LPC4088_IOCON_REG_P0_9  0x024
#define LPC4088_IOCON_REG_P0_10  0x028
#define LPC4088_IOCON_REG_P0_11  0x02C
#define LPC4088_IOCON_REG_P0_12  0x030
#define LPC4088_IOCON_REG_P0_13  0x034
#define LPC4088_IOCON_REG_P0_14  0x038
#define LPC4088_IOCON_REG_P0_15  0x03C
#define LPC4088_IOCON_REG_P0_16  0x040
#define LPC4088_IOCON_REG_P0_17  0x044
#define LPC4088_IOCON_REG_P0_18  0x048
#define LPC4088_IOCON_REG_P0_19  0x04C
#define LPC4088_IOCON_REG_P0_20  0x050
#define LPC4088_IOCON_REG_P0_21  0x054
#define LPC4088_IOCON_REG_P0_22  0x058
#define LPC4088_IOCON_REG_P0_23  0x05C
#define LPC4088_IOCON_REG_P0_24  0x060
#define LPC4088_IOCON_REG_P0_25  0x064
#define LPC4088_IOCON_REG_P0_26  0x068
#define LPC4088_IOCON_REG_P0_27  0x06C
#define LPC4088_IOCON_REG_P0_28  0x070
#define LPC4088_IOCON_REG_P0_29  0x074
#define LPC4088_IOCON_REG_P0_30  0x078
#define LPC4088_IOCON_REG_P0_31  0x07C
#define LPC4088_IOCON_REG_P1_0  0x080
#define LPC4088_IOCON_REG_P1_1  0x084
#define LPC4088_IOCON_REG_P1_2  0x088
#define LPC4088_IOCON_REG_P1_3  0x08C
#define LPC4088_IOCON_REG_P1_4  0x090
#define LPC4088_IOCON_REG_P1_5  0x094
#define LPC4088_IOCON_REG_P1_6  0x098
#define LPC4088_IOCON_REG_P1_7  0x09C
#define LPC4088_IOCON_REG_P1_8  0x0A0
#define LPC4088_IOCON_REG_P1_9  0x0A4
#define LPC4088_IOCON_REG_P1_10  0x0A8
#define LPC4088_IOCON_REG_P1_11  0x0AC
#define LPC4088_IOCON_REG_P1_12  0x0B0
#define LPC4088_IOCON_REG_P1_13  0x0B4
#define LPC4088_IOCON_REG_P1_14  0x0B8
#define LPC4088_IOCON_REG_P1_15  0x0BC
#define LPC4088_IOCON_REG_P1_16  0x0C0
#define LPC4088_IOCON_REG_P1_17  0x0C4
#define LPC4088_IOCON_REG_P1_18  0x0C8
#define LPC4088_IOCON_REG_P1_19  0x0CC
#define LPC4088_IOCON_REG_P1_20  0x0D0
#define LPC4088_IOCON_REG_P1_21  0x0D4
#define LPC4088_IOCON_REG_P1_22  0x0D8
#define LPC4088_IOCON_REG_P1_23  0x0DC
#define LPC4088_IOCON_REG_P1_24  0x0E0
#define LPC4088_IOCON_REG_P1_25  0x0E4
#define LPC4088_IOCON_REG_P1_26  0x0E8
#define LPC4088_IOCON_REG_P1_27  0x0EC
#define LPC4088_IOCON_REG_P1_28  0x0F0
#define LPC4088_IOCON_REG_P1_29  0x0F4
#define LPC4088_IOCON_REG_P1_30  0x0F8
#define LPC4088_IOCON_REG_P1_31  0x0FC
#define LPC4088_IOCON_REG_P2_0  0x100
#define LPC4088_IOCON_REG_P2_1  0x104
#define LPC4088_IOCON_REG_P2_2  0x108
#define LPC4088_IOCON_REG_P2_3  0x10C
#define LPC4088_IOCON_REG_P2_4  0x110
#define LPC4088_IOCON_REG_P2_5  0x114
#define LPC4088_IOCON_REG_P2_6  0x118
#define LPC4088_IOCON_REG_P2_7  0x11C
#define LPC4088_IOCON_REG_P2_8  0x120
#define LPC4088_IOCON_REG_P2_9  0x124
#define LPC4088_IOCON_REG_P2_10  0x128
#define LPC4088_IOCON_REG_P2_11  0x12C
#define LPC4088_IOCON_REG_P2_12  0x130
#define LPC4088_IOCON_REG_P2_13  0x134
#define LPC4088_IOCON_REG_P2_14  0x138
#define LPC4088_IOCON_REG_P2_15  0x13C
#define LPC4088_IOCON_REG_P2_16  0x140
#define LPC4088_IOCON_REG_P2_17  0x144
#define LPC4088_IOCON_REG_P2_18  0x148
#define LPC4088_IOCON_REG_P2_19  0x14C
#define LPC4088_IOCON_REG_P2_20  0x150
#define LPC4088_IOCON_REG_P2_21  0x154
#define LPC4088_IOCON_REG_P2_22  0x158
#define LPC4088_IOCON_REG_P2_23  0x15C
#define LPC4088_IOCON_REG_P2_24  0x160
#define LPC4088_IOCON_REG_P2_25  0x164
#define LPC4088_IOCON_REG_P2_26  0x168
#define LPC4088_IOCON_REG_P2_27  0x16C
#define LPC4088_IOCON_REG_P2_28  0x170
#define LPC4088_IOCON_REG_P2_29  0x174
#define LPC4088_IOCON_REG_P2_30  0x178
#define LPC4088_IOCON_REG_P2_31  0x17C
#define LPC4088_IOCON_REG_P3_0  0x180
#define LPC4088_IOCON_REG_P3_1  0x184
#define LPC4088_IOCON_REG_P3_2  0x188
#define LPC4088_IOCON_REG_P3_3  0x18C
#define LPC4088_IOCON_REG_P3_4  0x190
#define LPC4088_IOCON_REG_P3_5  0x194
#define LPC4088_IOCON_REG_P3_6  0x198
#define LPC4088_IOCON_REG_P3_7  0x19C
#define LPC4088_IOCON_REG_P3_8  0x1A0
#define LPC4088_IOCON_REG_P3_9  0x1A4
#define LPC4088_IOCON_REG_P3_10  0x1A8
#define LPC4088_IOCON_REG_P3_11  0x1AC
#define LPC4088_IOCON_REG_P3_12  0x1B0
#define LPC4088_IOCON_REG_P3_13  0x1B4
#define LPC4088_IOCON_REG_P3_14  0x1B8
#define LPC4088_IOCON_REG_P3_15  0x1BC
#define LPC4088_IOCON_REG_P3_16  0x1C0
#define LPC4088_IOCON_REG_P3_17  0x1C4
#define LPC4088_IOCON_REG_P3_18  0x1C8
#define LPC4088_IOCON_REG_P3_19  0x1CC
#define LPC4088_IOCON_REG_P3_20  0x1D0
#define LPC4088_IOCON_REG_P3_21  0x1D4
#define LPC4088_IOCON_REG_P3_22  0x1D8
#define LPC4088_IOCON_REG_P3_23  0x1DC
#define LPC4088_IOCON_REG_P3_24  0x1E0
#define LPC4088_IOCON_REG_P3_25  0x1E4
#define LPC4088_IOCON_REG_P3_26  0x1E8
#define LPC4088_IOCON_REG_P3_27  0x1EC
#define LPC4088_IOCON_REG_P3_28  0x1F0
#define LPC4088_IOCON_REG_P3_29  0x1F4
#define LPC4088_IOCON_REG_P3_30  0x1F8
#define LPC4088_IOCON_REG_P3_31  0x1FC
#define LPC4088_IOCON_REG_P4_0  0x200
#define LPC4088_IOCON_REG_P4_1  0x204
#define LPC4088_IOCON_REG_P4_2  0x208
#define LPC4088_IOCON_REG_P4_3  0x20C
#define LPC4088_IOCON_REG_P4_4  0x210
#define LPC4088_IOCON_REG_P4_5  0x214
#define LPC4088_IOCON_REG_P4_6  0x218
#define LPC4088_IOCON_REG_P4_7  0x21C
#define LPC4088_IOCON_REG_P4_8  0x220
#define LPC4088_IOCON_REG_P4_9  0x224
#define LPC4088_IOCON_REG_P4_10  0x228
#define LPC4088_IOCON_REG_P4_11  0x22C
#define LPC4088_IOCON_REG_P4_12  0x230
#define LPC4088_IOCON_REG_P4_13  0x234
#define LPC4088_IOCON_REG_P4_14  0x238
#define LPC4088_IOCON_REG_P4_15  0x23C
#define LPC4088_IOCON_REG_P4_16  0x240
#define LPC4088_IOCON_REG_P4_17  0x244
#define LPC4088_IOCON_REG_P4_18  0x248
#define LPC4088_IOCON_REG_P4_19  0x24C
#define LPC4088_IOCON_REG_P4_20  0x250
#define LPC4088_IOCON_REG_P4_21  0x254
#define LPC4088_IOCON_REG_P4_22  0x258
#define LPC4088_IOCON_REG_P4_23  0x25C
#define LPC4088_IOCON_REG_P4_24  0x260
#define LPC4088_IOCON_REG_P4_25  0x264
#define LPC4088_IOCON_REG_P4_26  0x268
#define LPC4088_IOCON_REG_P4_27  0x26C
#define LPC4088_IOCON_REG_P4_28  0x270
#define LPC4088_IOCON_REG_P4_29  0x274
#define LPC4088_IOCON_REG_P4_30  0x278
#define LPC4088_IOCON_REG_P4_31  0x27C
#define LPC4088_IOCON_REG_P5_0  0x280
#define LPC4088_IOCON_REG_P5_1  0x284
#define LPC4088_IOCON_REG_P5_2  0x288
#define LPC4088_IOCON_REG_P5_3  0x28C
#define LPC4088_IOCON_REG_P5_4  0x290

typedef struct LPC4088IOCONState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;
	
	uint32_t iocon_P0_0;
	uint32_t iocon_P0_1;
	uint32_t iocon_P0_2;
	uint32_t iocon_P0_3;
	uint32_t iocon_P0_4;
	uint32_t iocon_P0_5;
	uint32_t iocon_P0_6;
	uint32_t iocon_P0_7;

	uint32_t iocon_P0_8;				/* 0x020 */
	uint32_t iocon_P0_9;
	uint32_t iocon_P0_10;
	uint32_t iocon_P0_11;
	uint32_t iocon_P0_12;
	uint32_t iocon_P0_13;
	uint32_t iocon_P0_14;
	uint32_t iocon_P0_15;

	uint32_t iocon_P0_16;				/* 0x040 */
	uint32_t iocon_P0_17;
	uint32_t iocon_P0_18;
	uint32_t iocon_P0_19;
	uint32_t iocon_P0_20;
	uint32_t iocon_P0_21;
	uint32_t iocon_P0_22;
	uint32_t iocon_P0_23;

	uint32_t iocon_P0_24;				/* 0x060 */
	uint32_t iocon_P0_25;
	uint32_t iocon_P0_26;
	uint32_t iocon_P0_27;
	uint32_t iocon_P0_28;
	uint32_t iocon_P0_29;
	uint32_t iocon_P0_30;
	uint32_t iocon_P0_31;

	uint32_t iocon_P1_0;				/* 0x080 */
	uint32_t iocon_P1_1;
	uint32_t iocon_P1_2;
	uint32_t iocon_P1_3;
	uint32_t iocon_P1_4;
	uint32_t iocon_P1_5;
	uint32_t iocon_P1_6;
	uint32_t iocon_P1_7;

	uint32_t iocon_P1_8;				/* 0x0A0 */
	uint32_t iocon_P1_9;
	uint32_t iocon_P1_10;
	uint32_t iocon_P1_11;
	uint32_t iocon_P1_12;
	uint32_t iocon_P1_13;
	uint32_t iocon_P1_14;
	uint32_t iocon_P1_15;

	uint32_t iocon_P1_16;				/* 0x0C0 */
	uint32_t iocon_P1_17;
	uint32_t iocon_P1_18;
	uint32_t iocon_P1_19;
	uint32_t iocon_P1_20;
	uint32_t iocon_P1_21;
	uint32_t iocon_P1_22;
	uint32_t iocon_P1_23;

	uint32_t iocon_P1_24;				/* 0x0E0 */
	uint32_t iocon_P1_25;
	uint32_t iocon_P1_26;
	uint32_t iocon_P1_27;
	uint32_t iocon_P1_28;
	uint32_t iocon_P1_29;
	uint32_t iocon_P1_30;
	uint32_t iocon_P1_31;

	uint32_t iocon_P2_0;				/* 0x100 */
	uint32_t iocon_P2_1;
	uint32_t iocon_P2_2;
	uint32_t iocon_P2_3;
	uint32_t iocon_P2_4;
	uint32_t iocon_P2_5;
	uint32_t iocon_P2_6;
	uint32_t iocon_P2_7;

	uint32_t iocon_P2_8;				/* 0x120 */
	uint32_t iocon_P2_9;
	uint32_t iocon_P2_10;
	uint32_t iocon_P2_11;
	uint32_t iocon_P2_12;
	uint32_t iocon_P2_13;
	uint32_t iocon_P2_14;
	uint32_t iocon_P2_15;

	uint32_t iocon_P2_16;				/* 0x140 */
	uint32_t iocon_P2_17;
	uint32_t iocon_P2_18;
	uint32_t iocon_P2_19;
	uint32_t iocon_P2_20;
	uint32_t iocon_P2_21;
	uint32_t iocon_P2_22;
	uint32_t iocon_P2_23;

	uint32_t iocon_P2_24;				/* 0x160 */
	uint32_t iocon_P2_25;
	uint32_t iocon_P2_26;
	uint32_t iocon_P2_27;
	uint32_t iocon_P2_28;
	uint32_t iocon_P2_29;
	uint32_t iocon_P2_30;
	uint32_t iocon_P2_31;

	uint32_t iocon_P3_0;				/* 0x180 */
	uint32_t iocon_P3_1;
	uint32_t iocon_P3_2;
	uint32_t iocon_P3_3;
	uint32_t iocon_P3_4;
	uint32_t iocon_P3_5;
	uint32_t iocon_P3_6;
	uint32_t iocon_P3_7;

	uint32_t iocon_P3_8;				/* 0x1A0 */
	uint32_t iocon_P3_9;
	uint32_t iocon_P3_10;
	uint32_t iocon_P3_11;
	uint32_t iocon_P3_12;
	uint32_t iocon_P3_13;
	uint32_t iocon_P3_14;
	uint32_t iocon_P3_15;

	uint32_t iocon_P3_16;				/* 0x1C0 */
	uint32_t iocon_P3_17;
	uint32_t iocon_P3_18;
	uint32_t iocon_P3_19;
	uint32_t iocon_P3_20;
	uint32_t iocon_P3_21;
	uint32_t iocon_P3_22;
	uint32_t iocon_P3_23;

	uint32_t iocon_P3_24;				/* 0x1E0 */
	uint32_t iocon_P3_25;
	uint32_t iocon_P3_26;
	uint32_t iocon_P3_27;
	uint32_t iocon_P3_28;
	uint32_t iocon_P3_29;
	uint32_t iocon_P3_30;
	uint32_t iocon_P3_31;

	uint32_t iocon_P4_0;				/* 0x200 */
	uint32_t iocon_P4_1;
	uint32_t iocon_P4_2;
	uint32_t iocon_P4_3;
	uint32_t iocon_P4_4;
	uint32_t iocon_P4_5;
	uint32_t iocon_P4_6;
	uint32_t iocon_P4_7;

	uint32_t iocon_P4_8;				/* 0x220 */
	uint32_t iocon_P4_9;
	uint32_t iocon_P4_10;
	uint32_t iocon_P4_11;
	uint32_t iocon_P4_12;
	uint32_t iocon_P4_13;
	uint32_t iocon_P4_14;
	uint32_t iocon_P4_15;

	uint32_t iocon_P4_16;				/* 0x240 */
	uint32_t iocon_P4_17;
	uint32_t iocon_P4_18;
	uint32_t iocon_P4_19;
	uint32_t iocon_P4_20;
	uint32_t iocon_P4_21;
	uint32_t iocon_P4_22;
	uint32_t iocon_P4_23;

	uint32_t iocon_P4_24;				/* 0x260 */
	uint32_t iocon_P4_25;
	uint32_t iocon_P4_26;
	uint32_t iocon_P4_27;
	uint32_t iocon_P4_28;
	uint32_t iocon_P4_29;
	uint32_t iocon_P4_30;
	uint32_t iocon_P4_31;

	uint32_t iocon_P5_0;				/* 0x280 */
	uint32_t iocon_P5_1;
	uint32_t iocon_P5_2;
	uint32_t iocon_P5_3;
	uint32_t iocon_P5_4;				/* 0x290 */

	RemoteCtrlState rcs;

} LPC4088IOCONState;

#endif /* __LPC4088_IOCON_H__ */
