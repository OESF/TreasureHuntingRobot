 /*
 * hdmi_lib.h
 *
 * HDMI driver definition for TI OMAP processors.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _HDMI_H_
#define _HDMI_H_

#include <linux/string.h>

#define DBG(format, ...) \
		printk(KERN_DEBUG "hdmi: " format, ## __VA_ARGS__)
#define ERR(format, ...) \
	printk(KERN_ERR "hdmi error: " format, ## __VA_ARGS__)

#define BITS_32(in_NbBits) \
		((((u32)1 << in_NbBits) - 1) | ((u32)1 << in_NbBits))

#define BITFIELD(in_UpBit, in_LowBit) \
		(BITS_32(in_UpBit) & ~((BITS_32(in_LowBit)) >> 1))

#define HDMI_BASE	0x58006000
#define HDMI_WP		0x000
#define HDMI_PLLCTRL	0x200
#define HDMI_PHY	0x300
#define HDMI_CORE_SYS	0x400
#define HDMI_CORE_AV	0x900

#define HDMI_WP_REG(idx)			(idx)

#define HDMI_WP_REVISION			HDMI_WP_REG(0x0)
#define HDMI_WP_SYSCONFIG			HDMI_WP_REG(0x10)
#define HDMI_WP_IRQSTATUS_RAW			HDMI_WP_REG(0x24)
#define HDMI_WP_IRQSTATUS			HDMI_WP_REG(0x28)
#define HDMI_WP_PWR_CTRL			HDMI_WP_REG(0x40)
#define HDMI_WP_IRQENABLE_SET			HDMI_WP_REG(0x2C)
#define HDMI_WP_VIDEO_CFG			HDMI_WP_REG(0x50)
#define HDMI_WP_VIDEO_SIZE			HDMI_WP_REG(0x60)
#define HDMI_WP_VIDEO_TIMING_H			HDMI_WP_REG(0x68)
#define HDMI_WP_VIDEO_TIMING_V			HDMI_WP_REG(0x6C)
#define HDMI_WP_WP_CLK				HDMI_WP_REG(0x70)
#define HDMI_WP_AUDIO_CFG			HDMI_WP_REG(0x80)
#define HDMI_WP_AUDIO_CFG2			HDMI_WP_REG(0x84)
#define HDMI_WP_AUDIO_CTRL			HDMI_WP_REG(0x88)
#define HDMI_WP_AUDIO_DATA			HDMI_WP_REG(0x8C)

/* HDMI IP Core System */
#define HDMI_CORE_SYS_REG(idx)			(idx)

#define HDMI_CORE_SYS_VND_IDL			HDMI_CORE_SYS_REG(0x0)
#define HDMI_CORE_SYS_DEV_IDL			HDMI_CORE_SYS_REG(0x8)
#define HDMI_CORE_SYS_DEV_IDH			HDMI_CORE_SYS_REG(0xC)
#define HDMI_CORE_SYS_DEV_REV			HDMI_CORE_SYS_REG(0x10)
#define HDMI_CORE_SYS_SRST			HDMI_CORE_SYS_REG(0x14)
#define HDMI_CORE_CTRL1			HDMI_CORE_SYS_REG(0x20)
#define HDMI_CORE_SYS_SYS_STAT			HDMI_CORE_SYS_REG(0x24)
#define HDMI_CORE_SYS_VID_ACEN			HDMI_CORE_SYS_REG(0x124)
#define HDMI_CORE_SYS_VID_MODE			HDMI_CORE_SYS_REG(0x128)
#define HDMI_CORE_SYS_INTR_STATE		HDMI_CORE_SYS_REG(0x1C0)
#define HDMI_CORE_SYS_INTR1			HDMI_CORE_SYS_REG(0x1C4)
#define HDMI_CORE_SYS_INTR2			HDMI_CORE_SYS_REG(0x1C8)
#define HDMI_CORE_SYS_INTR3			HDMI_CORE_SYS_REG(0x1CC)
#define HDMI_CORE_SYS_INTR4			HDMI_CORE_SYS_REG(0x1D0)
#define HDMI_CORE_SYS_UMASK1			HDMI_CORE_SYS_REG(0x1D4)
#define HDMI_CORE_SYS_TMDS_CTRL		HDMI_CORE_SYS_REG(0x208)
#define HDMI_CORE_SYS_DE_DLY			HDMI_CORE_SYS_REG(0xC8)
#define HDMI_CORE_SYS_DE_CTRL			HDMI_CORE_SYS_REG(0xCC)
#define HDMI_CORE_SYS_DE_TOP			HDMI_CORE_SYS_REG(0xD0)
#define HDMI_CORE_SYS_DE_CNTL			HDMI_CORE_SYS_REG(0xD8)
#define HDMI_CORE_SYS_DE_CNTH			HDMI_CORE_SYS_REG(0xDC)
#define HDMI_CORE_SYS_DE_LINL			HDMI_CORE_SYS_REG(0xE0)
#define HDMI_CORE_SYS_DE_LINH_1		HDMI_CORE_SYS_REG(0xE4)
#define HDMI_CORE_CTRL1_VEN_FOLLOWVSYNC	0x1
#define HDMI_CORE_CTRL1_HEN_FOLLOWHSYNC	0x1
#define HDMI_CORE_CTRL1_BSEL_24BITBUS		0x1
#define HDMI_CORE_CTRL1_EDGE_RISINGEDGE	0x1

/* HDMI DDC E-DID */
#define HDMI_CORE_DDC_CMD			HDMI_CORE_SYS_REG(0x3CC)
#define HDMI_CORE_DDC_STATUS			HDMI_CORE_SYS_REG(0x3C8)
#define HDMI_CORE_DDC_ADDR			HDMI_CORE_SYS_REG(0x3B4)
#define HDMI_CORE_DDC_OFFSET			HDMI_CORE_SYS_REG(0x3BC)
#define HDMI_CORE_DDC_COUNT1			HDMI_CORE_SYS_REG(0x3C0)
#define HDMI_CORE_DDC_COUNT2			HDMI_CORE_SYS_REG(0x3C4)
#define HDMI_CORE_DDC_DATA			HDMI_CORE_SYS_REG(0x3D0)
#define HDMI_CORE_DDC_SEGM			HDMI_CORE_SYS_REG(0x3B8)

/* HDMI IP Core Audio Video */
#define HDMI_CORE_AV_REG(idx)			(idx)

#define HDMI_CORE_AV_HDMI_CTRL			HDMI_CORE_AV_REG(0xBC)
#define HDMI_CORE_AV_DPD			HDMI_CORE_AV_REG(0xF4)
#define HDMI_CORE_AV_PB_CTRL1			HDMI_CORE_AV_REG(0xF8)
#define HDMI_CORE_AV_PB_CTRL2			HDMI_CORE_AV_REG(0xFC)
#define HDMI_CORE_AV_AVI_TYPE			HDMI_CORE_AV_REG(0x100)
#define HDMI_CORE_AV_AVI_VERS			HDMI_CORE_AV_REG(0x104)
#define HDMI_CORE_AV_AVI_LEN			HDMI_CORE_AV_REG(0x108)
#define HDMI_CORE_AV_AVI_CHSUM			HDMI_CORE_AV_REG(0x10C)
#define HDMI_CORE_AV_AVI_DBYTE                0x110ul
#define HDMI_CORE_AV_AVI_DBYTE_NELEMS		HDMI_CORE_AV_REG(15)
#define HDMI_CORE_AV_SPD_DBYTE			HDMI_CORE_AV_REG(0x190)
#define HDMI_CORE_AV_SPD_DBYTE_NELEMS		HDMI_CORE_AV_REG(27)
#define HDMI_CORE_AV_AUD_DBYTE(n)		HDMI_CORE_AV_REG(n * 4 + 0x210)
#define HDMI_CORE_AV_AUD_DBYTE_NELEMS		HDMI_CORE_AV_REG(10)
#define HDMI_CORE_AV_MPEG_DBYTE		HDMI_CORE_AV_REG(0x290)
#define HDMI_CORE_AV_MPEG_DBYTE_NELEMS		HDMI_CORE_AV_REG(27)
#define HDMI_CORE_AV_GEN_DBYTE			HDMI_CORE_AV_REG(0x300)
#define HDMI_CORE_AV_GEN_DBYTE_NELEMS		HDMI_CORE_AV_REG(31)
#define HDMI_CORE_AV_GEN2_DBYTE		HDMI_CORE_AV_REG(0x380)
#define HDMI_CORE_AV_GEN2_DBYTE_NELEMS		HDMI_CORE_AV_REG(31)
#define HDMI_CORE_AV_ACR_CTRL			HDMI_CORE_AV_REG(0x4)
#define HDMI_CORE_AV_FREQ_SVAL			HDMI_CORE_AV_REG(0x8)
#define HDMI_CORE_AV_N_SVAL1			HDMI_CORE_AV_REG(0xC)
#define HDMI_CORE_AV_N_SVAL2			HDMI_CORE_AV_REG(0x10)
#define HDMI_CORE_AV_N_SVAL3			HDMI_CORE_AV_REG(0x14)
#define HDMI_CORE_AV_CTS_SVAL1			HDMI_CORE_AV_REG(0x18)
#define HDMI_CORE_AV_CTS_SVAL2			HDMI_CORE_AV_REG(0x1C)
#define HDMI_CORE_AV_CTS_SVAL3			HDMI_CORE_AV_REG(0x20)
#define HDMI_CORE_AV_CTS_HVAL1			HDMI_CORE_AV_REG(0x24)
#define HDMI_CORE_AV_CTS_HVAL2			HDMI_CORE_AV_REG(0x28)
#define HDMI_CORE_AV_CTS_HVAL3			HDMI_CORE_AV_REG(0x2C)
#define HDMI_CORE_AV_AUD_MODE			HDMI_CORE_AV_REG(0x50)
#define HDMI_CORE_AV_SPDIF_CTRL		HDMI_CORE_AV_REG(0x54)
#define HDMI_CORE_AV_HW_SPDIF_FS		HDMI_CORE_AV_REG(0x60)
#define HDMI_CORE_AV_SWAP_I2S			HDMI_CORE_AV_REG(0x64)
#define HDMI_CORE_AV_SPDIF_ERTH		HDMI_CORE_AV_REG(0x6C)
#define HDMI_CORE_AV_I2S_IN_MAP		HDMI_CORE_AV_REG(0x70)
#define HDMI_CORE_AV_I2S_IN_CTRL		HDMI_CORE_AV_REG(0x74)
#define HDMI_CORE_AV_I2S_CHST0			HDMI_CORE_AV_REG(0x78)
#define HDMI_CORE_AV_I2S_CHST1			HDMI_CORE_AV_REG(0x7C)
#define HDMI_CORE_AV_I2S_CHST2			HDMI_CORE_AV_REG(0x80)
#define HDMI_CORE_AV_I2S_CHST4			HDMI_CORE_AV_REG(0x84)
#define HDMI_CORE_AV_I2S_CHST5			HDMI_CORE_AV_REG(0x88)
#define HDMI_CORE_AV_ASRC			HDMI_CORE_AV_REG(0x8C)
#define HDMI_CORE_AV_I2S_IN_LEN		HDMI_CORE_AV_REG(0x90)
#define HDMI_CORE_AV_HDMI_CTRL			HDMI_CORE_AV_REG(0xBC)
#define HDMI_CORE_AV_AUDO_TXSTAT		HDMI_CORE_AV_REG(0xC0)
#define HDMI_CORE_AV_AUD_PAR_BUSCLK_1		HDMI_CORE_AV_REG(0xCC)
#define HDMI_CORE_AV_AUD_PAR_BUSCLK_2		HDMI_CORE_AV_REG(0xD0)
#define HDMI_CORE_AV_AUD_PAR_BUSCLK_3		HDMI_CORE_AV_REG(0xD4)
#define HDMI_CORE_AV_TEST_TXCTRL		HDMI_CORE_AV_REG(0xF0)
#define HDMI_CORE_AV_DPD			HDMI_CORE_AV_REG(0xF4)
#define HDMI_CORE_AV_PB_CTRL1			HDMI_CORE_AV_REG(0xF8)
#define HDMI_CORE_AV_PB_CTRL2			HDMI_CORE_AV_REG(0xFC)
#define HDMI_CORE_AV_AVI_TYPE			HDMI_CORE_AV_REG(0x100)
#define HDMI_CORE_AV_AVI_VERS			HDMI_CORE_AV_REG(0x104)
#define HDMI_CORE_AV_AVI_LEN			HDMI_CORE_AV_REG(0x108)
#define HDMI_CORE_AV_AVI_CHSUM			HDMI_CORE_AV_REG(0x10C)
#define HDMI_CORE_AV_SPD_TYPE			HDMI_CORE_AV_REG(0x180)
#define HDMI_CORE_AV_SPD_VERS			HDMI_CORE_AV_REG(0x184)
#define HDMI_CORE_AV_SPD_LEN			HDMI_CORE_AV_REG(0x188)
#define HDMI_CORE_AV_SPD_CHSUM			HDMI_CORE_AV_REG(0x18C)
#define HDMI_CORE_AV_AUDIO_TYPE		HDMI_CORE_AV_REG(0x200)
#define HDMI_CORE_AV_AUDIO_VERS		HDMI_CORE_AV_REG(0x204)
#define HDMI_CORE_AV_AUDIO_LEN			HDMI_CORE_AV_REG(0x208)
#define HDMI_CORE_AV_AUDIO_CHSUM		HDMI_CORE_AV_REG(0x20C)
#define HDMI_CORE_AV_MPEG_TYPE			HDMI_CORE_AV_REG(0x280)
#define HDMI_CORE_AV_MPEG_VERS			HDMI_CORE_AV_REG(0x284)
#define HDMI_CORE_AV_MPEG_LEN			HDMI_CORE_AV_REG(0x288)
#define HDMI_CORE_AV_MPEG_CHSUM		HDMI_CORE_AV_REG(0x28C)
#define HDMI_CORE_AV_CP_BYTE1			HDMI_CORE_AV_REG(0x37C)
#define HDMI_CORE_AV_CEC_ADDR_ID		HDMI_CORE_AV_REG(0x3FC)
#define HDMI_CORE_AV_SPD_DBYTE_ELSIZE		0x4
#define HDMI_CORE_AV_GEN2_DBYTE_ELSIZE		0x4
#define HDMI_CORE_AV_MPEG_DBYTE_ELSIZE		0x4
#define HDMI_CORE_AV_GEN_DBYTE_ELSIZE		0x4

#define HDMI_TXPHY_TX_CTRL						0x0ul
#define HDMI_TXPHY_DIGITAL_CTRL					0x4ul
#define HDMI_TXPHY_POWER_CTRL					0x8ul

#define HDMI_CORE_AV_AVI_DBYTE_ELSIZE        0x4ul
#define HDMI_IP_CORE_AV_AVI_DBYTE_NELEMS        15
#define HDMI_CORE_AV_AUDIO_DBYTE              0x210ul
#define HDMI_CORE_AV_AUDIO_DBYTE_ELSIZE      0x4ul
#define HDMI_CORE_AV_AUDIO_DBYTE_NELEMS      10

struct hdmi_irq_vector {
	u8	pllRecal;
	u8	pllUnlock;
	u8	pllLock;
	u8	phyDisconnect;
	u8	phyConnect;
	u8	phyShort5v;
	u8	videoEndFrame;
	u8	videoVsync;
	u8	fifoSampleRequest;
	u8	fifoOverflow;
	u8	fifoUnderflow;
	u8	ocpTimeOut;
	u8	core;
};

typedef enum HDMI_PhyPwr_label {
	HDMI_PHYPWRCMD_OFF = 0,
	HDMI_PHYPWRCMD_LDOON = 1,
	HDMI_PHYPWRCMD_TXON = 2
} HDMI_PhyPwr_t, *pHDMI_PhyPwr_t;

typedef enum HDMI_PllPwr_label {
	HDMI_PLLPWRCMD_ALLOFF = 0,
	HDMI_PLLPWRCMD_PLLONLY = 1,
	HDMI_PLLPWRCMD_BOTHON_ALLCLKS = 2,
	HDMI_PLLPWRCMD_BOTHON_NOPHYCLK = 3
} HDMI_PllPwr_t, *pHDMI_PllPwr_t;

enum hdmi_core_inputbus_width {
	HDMI_INPUT_8BIT = 0,
	HDMI_INPUT_10BIT = 1,
	HDMI_INPUT_12BIT = 2
};

enum hdmi_core_dither_trunc {
	HDMI_OUTPUTTRUNCATION_8BIT = 0,
	HDMI_OUTPUTTRUNCATION_10BIT = 1,
	HDMI_OUTPUTTRUNCATION_12BIT = 2,
	HDMI_OUTPUTDITHER_8BIT = 3,
	HDMI_OUTPUTDITHER_10BIT = 4,
	HDMI_OUTPUTDITHER_12BIT = 5
};

enum hdmi_core_deepcolor_ed {
	HDMI_DEEPCOLORPACKECTDISABLE = 0,
	HDMI_DEEPCOLORPACKECTENABLE = 1
};

enum hdmi_core_packet_mode {
	HDMI_PACKETMODERESERVEDVALUE = 0,
	HDMI_PACKETMODE24BITPERPIXEL = 4,
	HDMI_PACKETMODE30BITPERPIXEL = 5,
	HDMI_PACKETMODE36BITPERPIXEL = 6,
	HDMI_PACKETMODE48BITPERPIXEL = 7
};

enum hdmi_core_hdmi_dvi {
	HDMI_DVI	= 0,
	HDMI_HDMI	= 1
};

enum hdmi_core_tclkselclkmult {
	FPLL05IDCK  = 0,
	FPLL10IDCK  = 1,
	FPLL20IDCK  = 2,
	FPLL40IDCK  = 3
};

struct hdmi_core_video_config_t {
	enum hdmi_core_inputbus_width	CoreInputBusWide;
	enum hdmi_core_dither_trunc 	CoreOutputDitherTruncation;
	enum hdmi_core_deepcolor_ed		CoreDeepColorPacketED;
	enum hdmi_core_packet_mode		CorePacketMode;
	enum hdmi_core_hdmi_dvi			CoreHdmiDvi;
	enum hdmi_core_tclkselclkmult	CoreTclkSelClkMult;
};

enum hdmi_core_packet_ctrl {
	PACKETENABLE = 1,
	PACKETDISABLE = 0,
	PACKETREPEATON = 1,
	PACKETREPEATOFF = 0
};

/* INFOFRAME_AVI_ definations */
enum hdmi_core_infoframe {
	INFOFRAME_AVI_DB1Y_RGB = 0,
	INFOFRAME_AVI_DB1Y_YUV422 = 1,
	INFOFRAME_AVI_DB1Y_YUV444 = 2,
	INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_OFF = 0,
	INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_ON =  1,
	INFOFRAME_AVI_DB1B_NO = 0,
	INFOFRAME_AVI_DB1B_VERT = 1,
	INFOFRAME_AVI_DB1B_HORI = 2,
	INFOFRAME_AVI_DB1B_VERTHORI = 3,
	INFOFRAME_AVI_DB1S_0 = 0,
	INFOFRAME_AVI_DB1S_1 = 1,
	INFOFRAME_AVI_DB1S_2 = 2,
	INFOFRAME_AVI_DB2C_NO = 0,
	INFOFRAME_AVI_DB2C_ITU601 = 1,
	INFOFRAME_AVI_DB2C_ITU709 = 2,
	INFOFRAME_AVI_DB2C_EC_EXTENDED = 3,
	INFOFRAME_AVI_DB2M_NO = 0,
	INFOFRAME_AVI_DB2M_43 = 1,
	INFOFRAME_AVI_DB2M_169 = 2,
	INFOFRAME_AVI_DB2R_SAME = 8,
	INFOFRAME_AVI_DB2R_43 = 9,
	INFOFRAME_AVI_DB2R_169 = 10,
	INFOFRAME_AVI_DB2R_149 = 11,
	INFOFRAME_AVI_DB3ITC_NO = 0,
	INFOFRAME_AVI_DB3ITC_YES = 1,
	INFOFRAME_AVI_DB3EC_XVYUV601 = 0,
	INFOFRAME_AVI_DB3EC_XVYUV709 = 1,
	INFOFRAME_AVI_DB3Q_DEFAULT = 0,
	INFOFRAME_AVI_DB3Q_LR = 1,
	INFOFRAME_AVI_DB3Q_FR = 2,
	INFOFRAME_AVI_DB3SC_NO = 0,
	INFOFRAME_AVI_DB3SC_HORI = 1,
	INFOFRAME_AVI_DB3SC_VERT = 2,
	INFOFRAME_AVI_DB3SC_HORIVERT = 3,
	INFOFRAME_AVI_DB5PR_NO = 0,
	INFOFRAME_AVI_DB5PR_2 = 1,
	INFOFRAME_AVI_DB5PR_3 = 2,
	INFOFRAME_AVI_DB5PR_4 = 3,
	INFOFRAME_AVI_DB5PR_5 = 4,
	INFOFRAME_AVI_DB5PR_6 = 5,
	INFOFRAME_AVI_DB5PR_7 = 6,
	INFOFRAME_AVI_DB5PR_8 = 7,
	INFOFRAME_AVI_DB5PR_9 = 8,
	INFOFRAME_AVI_DB5PR_10 = 9,
	HDMI_INFOFRAME_AUDIO_DB1CT_FROM_STREAM = 0,
	HDMI_INFOFRAME_AUDIO_DB1CT_IEC60958 = 1,
	HDMI_INFOFRAME_AUDIO_DB1CT_AC3 = 2,
	HDMI_INFOFRAME_AUDIO_DB1CT_MPEG1 = 3,
	HDMI_INFOFRAME_AUDIO_DB1CT_MP3 = 4,
	HDMI_INFOFRAME_AUDIO_DB1CT_MPEG2_MULTICH = 5,
	HDMI_INFOFRAME_AUDIO_DB1CT_AAC = 6,
	HDMI_INFOFRAME_AUDIO_DB1CT_DTS = 7,
	HDMI_INFOFRAME_AUDIO_DB1CT_ATRAC = 8,
	HDMI_INFOFRAME_AUDIO_DB1CT_ONEBIT = 9,
	HDMI_INFOFRAME_AUDIO_DB1CT_DOLBY_DIGITAL_PLUS = 10,
	HDMI_INFOFRAME_AUDIO_DB1CT_DTS_HD = 11,
	HDMI_INFOFRAME_AUDIO_DB1CT_MAT = 12,
	HDMI_INFOFRAME_AUDIO_DB1CT_DST = 13,
	HDMI_INFOFRAME_AUDIO_DB1CT_WMA_PRO = 14,
	HDMI_INFOFRAME_AUDIO_DB2SF_FROM_STREAM = 0,
	HDMI_INFOFRAME_AUDIO_DB2SF_32000 = 1,
	HDMI_INFOFRAME_AUDIO_DB2SF_44100 = 2,
	HDMI_INFOFRAME_AUDIO_DB2SF_48000 = 3,
	HDMI_INFOFRAME_AUDIO_DB2SF_88200 = 4,
	HDMI_INFOFRAME_AUDIO_DB2SF_96000 = 5,
	HDMI_INFOFRAME_AUDIO_DB2SF_176400 = 6,
	HDMI_INFOFRAME_AUDIO_DB2SF_192000 = 7,
	HDMI_INFOFRAME_AUDIO_DB2SS_FROM_STREAM = 0,
	HDMI_INFOFRAME_AUDIO_DB2SS_16BIT = 1,
	HDMI_INFOFRAME_AUDIO_DB2SS_20BIT = 2,
	HDMI_INFOFRAME_AUDIO_DB2SS_24BIT = 3,
	HDMI_INFOFRAME_AUDIO_DB5_DM_INH_PERMITTED = 0,
	HDMI_INFOFRAME_AUDIO_DB5_DM_INH_PROHIBITED = 1
};

struct hdmi_core_infoframe_avi {
	u8		db1y_rgb_yuv422_yuv444;
	u8		db1a_active_format_off_on;
	u8		db1b_no_vert_hori_verthori;
	u8		db1s_0_1_2;
	u8		db2c_no_itu601_itu709_extented;
	u8		db2m_no_43_169;
	u8		db2r_same_43_169_149;
	u8		db3itc_no_yes;
	u8		db3ec_xvyuv601_xvyuv709;
	u8		db3q_default_lr_fr;
	u8		db3sc_no_hori_vert_horivert;
	u8		db4vic_videocode;
	u8		db5pr_no_2_3_4_5_6_7_8_9_10;
	u16		db6_7_lineendoftop;
	u16		db8_9_linestartofbottom;
	u16		db10_11_pixelendofleft;
	u16		db12_13_pixelstartofright;
};

struct hdmi_core_infoframe_audio {
	u8 db1_coding_type;
		/* Audio coding type */
	u8 db1_channel_count;
		/* Number of channels */
	u8 db2_sample_freq;
		/* Sample frequency */
	u8 db2_sample_size;
		/* Sample size */
	u8 db4_channel_alloc;
		/* Channel allocation code */
	bool db5_downmix_inh;
		/* Downmix inhibit flag */
	u8 db5_lsv;
		/* Level shift values for downmix */
};

struct hdmi_core_packet_enable_repeat {
	u32		MPEGInfoFrameED;
	u32		MPEGInfoFrameRepeat;
	u32		AudioPacketED;
	u32		AudioPacketRepeat;
	u32		SPDInfoFrameED;
	u32		SPDInfoFrameRepeat;
	u32		AVIInfoFrameED;
	u32		AVIInfoFrameRepeat;
	u32		GeneralcontrolPacketED;
	u32		GeneralcontrolPacketRepeat;
	u32		GenericPacketED;
	u32		GenericPacketRepeat;
};

enum hdmi_stereo_channel {
	HDMI_STEREO_NOCHANNEL = 0,
	HDMI_STEREO_ONECHANNELS = 1,
	HDMI_STEREO_TWOCHANNELS = 2,
	HDMI_STEREO_THREECHANNELS = 3,
	HDMI_STEREO_FOURCHANNELS = 4
};

enum hdmi_iec_format {
	HDMI_AUDIO_FORMAT_LPCM = 0,
	HDMI_AUDIO_FORMAT_IEC = 1
};

enum hdmi_sample_order {
	HDMI_SAMPLE_RIGHT_FIRST = 0,
	HDMI_SAMPLE_LEFT_FIRST = 1
};

enum hdmi_sample_perword {
	HDMI_ONEWORD_ONE_SAMPLE = 0,
	HDMI_ONEWORD_TWO_SAMPLES = 1
};

enum hdmi_sample_size {
	HDMI_SAMPLE_16BITS = 0,
	HDMI_SAMPLE_24BITS = 1
};

enum hdmi_stereo_channels {
	HDMI_AUDIO_STEREO_NOCHANNELS = 0,
	HDMI_AUDIO_STEREO_ONECHANNEL = 1,
	HDMI_AUDIO_STEREO_TWOCHANNELS = 2,
	HDMI_AUDIO_STEREO_THREECHANNELS = 3,
	HDMI_AUDIO_STEREO_FOURCHANNELS = 4
};

enum hdmi_audio_type {
	HDMI_AUDIO_TYPE_LPCM = 0,
	HDMI_AUDIO_TYPE_IEC = 1
};

enum hdmi_audio_justify {
	HDMI_AUDIO_JUSTIFY_LEFT = 0,
	HDMI_AUDIO_JUSTIFY_RIGHT = 1
};

enum hdmi_audio_sample_order {
	HDMI_AUDIO_SAMPLE_RIGHT_FIRST = 0,
	HDMI_AUDIO_SAMPLE_LEFT_FIRST = 1
};

enum hdmi_audio_samples_perword {
	HDMI_AUDIO_ONEWORD_ONESAMPLE = 0,
	HDMI_AUDIO_ONEWORD_TWOSAMPLES = 1
};

enum hdmi_audio_sample_size {
	HDMI_AUDIO_SAMPLE_16BITS = 0,
	HDMI_AUDIO_SAMPLE_24BITS = 1
};

enum hdmi_audio_blk_strt_end_sig {
	HDMI_AUDIO_BLOCK_SIG_STARTEND_ON = 0,
	HDMI_AUDIO_BLOCK_SIG_STARTEND_OFF = 1
};

struct hdmi_audio_format {
	enum hdmi_stereo_channels		stereo_channels;
	u8					active_chnnls_msk;
	enum hdmi_audio_type			type;
	enum hdmi_audio_justify			justify;
	enum hdmi_audio_sample_order		sample_order;
	enum hdmi_audio_samples_perword		samples_p_word;
	enum hdmi_audio_sample_size		sample_size;
	enum hdmi_audio_blk_strt_end_sig	sig_blk_strt_end;
};

enum hdmi_audio_transf_mode {
	HDMI_AUDIO_TRANSF_DMA = 0,
	HDMI_AUDIO_TRANSF_IRQ = 1
};

struct hdmi_audio_dma {
	u8				transfer_size;
	u8				block_size;
	enum hdmi_audio_transf_mode	mode;
	u16				threshold;
};

enum hdmi_packing_mode {
	HDMI_PACK_10b_RGB_YUV444 = 0,
	HDMI_PACK_24b_RGB_YUV444_YUV422 = 1,
	HDMI_PACK_ALREADYPACKED = 7
};

enum hdmi_timing_mode {
	HDMI_TIMING_MASTER_24BIT = 0x1,
	HDMI_TIMING_MASTER_30BIT = 0x2,
	HDMI_TIMING_MASTER_36BIT = 0x3
};

enum hdmi_deep_mode {
	HDMI_DEEP_COLOR_24BIT = 0,
	HDMI_DEEP_COLOR_30BIT = 1,
	HDMI_DEEP_COLOR_36BIT = 2
};

enum hdmi_range {
	HDMI_LIMITED_RANGE = 0,
	HDMI_FULL_RANGE = 1,
};

struct hdmi_video_format {
	enum hdmi_packing_mode	packingMode;
	u32	linePerPanel;
	u32	pixelPerLine;
};

struct hdmi_video_interface {
	int	vSyncPolarity;
	int	hSyncPolarity;
	int	interlacing;
	int	timingMode;
};

struct hdmi_video_timing {
	u32	horizontalBackPorch;
	u32	horizontalFrontPorch;
	u32	horizontalSyncPulse;
	u32	verticalBackPorch;
	u32	verticalFrontPorch;
	u32	verticalSyncPulse;
};

typedef struct HDMI_Timing_label {
	u32	pixelPerLine;
	u32	linePerPanel;
	u32	horizontalBackPorch;
	u32	horizontalFrontPorch;
	u32	horizontalSyncPulse;
	u32	verticalBackPorch;
	u32	verticalFrontPorch;
	u32	verticalSyncPulse;
	u32	pplclk;
} HDMI_Timing_t, *pHDMI_Timing_t;

struct hdmi_config {
	u16 ppl;	/* pixel per line */
	u16 lpp;	/* line per panel */
	u32 pixel_clock;
	u16 hsw;	/* Horizontal synchronization pulse width */
	u16 hfp;	/* Horizontal front porch */
	u16 hbp;	/* Horizontal back porch */
	u16 vsw;	/* Vertical synchronization pulse width */
	u16 vfp;	/* Vertical front porch */
	u16 vbp;	/* Vertical back porch */
	u16 interlace;
	u16 h_pol;
	u16 v_pol;
	u16 hdmi_dvi;
	u16 video_format;
	u16 deep_color;
};

enum hdmi_core_av_csc{
	RGB	= 0x0,
	RGB_TO_YUV = 0x1,
	YUV_TO_RGB = 0x2
};

struct hdmi_core_audio_i2s_config {
	u8 word_max_length;
	u8 word_length;
	u8 in_length_bits;
	u8 justify;
	u8 en_high_br_aud;
	u8 sck_edge_mode;
	u8 cbit_order;
	u8 vbit;
	u8 ws_polarity;
	u8 direction;
	u8 shift;
};

enum hdmi_audio_i2s_config {
	HDMI_AUDIO_I2S_WS_POLARITY_LOW_IS_LEFT = 0,
	HDMI_AUDIO_I2S_WS_POLARIT_YLOW_IS_RIGHT = 1,
	HDMI_AUDIO_I2S_MSB_SHIFTED_FIRST = 0,
	HDMI_AUDIO_I2S_LSB_SHIFTED_FIRST = 1,
	HDMI_AUDIO_I2S_MAX_WORD_20BITS = 0,
	HDMI_AUDIO_I2S_MAX_WORD_24BITS = 1,
	HDMI_AUDIO_I2S_CHST_WORD_NOT_SPECIFIED = 0,
	HDMI_AUDIO_I2S_CHST_WORD_16_BITS = 1,
	HDMI_AUDIO_I2S_CHST_WORD_17_BITS = 6,
	HDMI_AUDIO_I2S_CHST_WORD_18_BITS = 2,
	HDMI_AUDIO_I2S_CHST_WORD_19_BITS = 4,
	HDMI_AUDIO_I2S_CHST_WORD_20_BITS_20MAX = 5,
	HDMI_AUDIO_I2S_CHST_WORD_20_BITS_24MAX = 1,
	HDMI_AUDIO_I2S_CHST_WORD_21_BITS = 6,
	HDMI_AUDIO_I2S_CHST_WORD_22_BITS = 2,
	HDMI_AUDIO_I2S_CHST_WORD_23_BITS = 4,
	HDMI_AUDIO_I2S_CHST_WORD_24_BITS = 5,
	HDMI_AUDIO_I2S_SCK_SAMPLE_EDGE_FALLING = 0,
	HDMI_AUDIO_I2S_SCK_SAMPLE_EDGE_RISING = 1,
	HDMI_AUDIO_I2S_VBIT_PCM = 0,
	HDMI_AUDIO_I2S_VBIT_COMPRESSED = 1,
	HDMI_AUDIO_I2S_INPUT_LENGTH_NA = 0,
	HDMI_AUDIO_I2S_INPUT_LENGTH_16 = 2,
	HDMI_AUDIO_I2S_INPUT_LENGTH_17 = 12,
	HDMI_AUDIO_I2S_INPUT_LENGTH_18 = 4,
	HDMI_AUDIO_I2S_INPUT_LENGTH_19 = 8,
	HDMI_AUDIO_I2S_INPUT_LENGTH_20 = 10,
	HDMI_AUDIO_I2S_INPUT_LENGTH_21 = 13,
	HDMI_AUDIO_I2S_INPUT_LENGTH_22 = 5,
	HDMI_AUDIO_I2S_INPUT_LENGTH_23 = 9,
	HDMI_AUDIO_I2S_INPUT_LENGTH_24 = 11,
	HDMI_AUDIO_I2S_FIRST_BIT_SHIFT = 0,
	HDMI_AUDIO_I2S_FIRST_BIT_NO_SHIFT = 1
};

enum hdmi_core_audio_sample_freq {
	HDMI_AUDIO_FS_32000 = 0x3,
	HDMI_AUDIO_FS_44100 = 0x0,
	HDMI_AUDIO_FS_48000 = 0x2,
	HDMI_AUDIO_FS_88200 = 0x8,
	HDMI_AUDIO_FS_96000 = 0xA,
	HDMI_AUDIO_FS_176400 = 0xC,
	HDMI_AUDIO_FS_192000 = 0xE,
	HDMI_AUDIO_FS_NOT_INDICATED = 0x1
};

enum hdmi_core_audio_layout {
	HDMI_AUDIO_LAYOUT_2CH = 0,
	HDMI_AUDIO_LAYOUT_8CH = 1
};

enum hdmi_core_cts_mode {
	HDMI_AUDIO_CTS_MODE_HW = 0,
	HDMI_AUDIO_CTS_MODE_SW = 1
};

enum hdmi_audio_mclk_mode {
	HDMI_AUDIO_MCLK_128FS = 0,
	HDMI_AUDIO_MCLK_256FS = 1,
	HDMI_AUDIO_MCLK_384FS = 2,
	HDMI_AUDIO_MCLK_512FS = 3,
	HDMI_AUDIO_MCLK_768FS = 4,
	HDMI_AUDIO_MCLK_1024FS = 5,
	HDMI_AUDIO_MCLK_1152FS = 6,
	HDMI_AUDIO_MCLK_192FS = 7
};

struct hdmi_core_audio_config {
	struct hdmi_core_audio_i2s_config	i2s_cfg;
	enum hdmi_core_audio_sample_freq	freq_sample;
	bool					fs_override;
	u32					n;
	u32					cts;
	u32					aud_par_busclk;
	enum hdmi_core_audio_layout		layout;
	enum hdmi_core_cts_mode			cts_mode;
	bool					use_mclk;
	enum hdmi_audio_mclk_mode		mclk_mode;
	bool					en_acr_pkt;
	bool					en_direct_strm_dig_aud;
	bool					en_parallel_aud;
	bool					en_spdif;
};

struct hdmi_notifier {
	void (*hpd_notifier)(int state, void *data);
	void *private_data;
	struct list_head list;
};

#define HDMI_CONNECT      0x01
#define HDMI_DISCONNECT   0x02
#define HDMI_HPD          0x04
#define HDMI_FIRST_HPD    0x08

/* Function prototype */
int HDMI_W1_StopVideoFrame(u32);
int HDMI_W1_StartVideoFrame(u32);
int HDMI_W1_SetWaitPhyPwrState(u32 name, HDMI_PhyPwr_t param);
int HDMI_W1_SetWaitPllPwrState(u32 name, HDMI_PllPwr_t param);
int HDMI_W1_SetWaitSoftReset(void);
int hdmi_w1_wrapper_disable(u32);
int hdmi_w1_wrapper_enable(u32);
int hdmi_w1_stop_audio_transfer(u32);
int hdmi_w1_start_audio_transfer(u32);
int HDMI_CORE_DDC_READEDID(u32 Core, u8 *data, u16 max_length);
int hdmi_lib_enable(struct hdmi_config *cfg);
void HDMI_W1_HPD_handler(int *r);
int hdmi_lib_init(void);
void hdmi_lib_exit(void);
int hdmi_configure_csc(enum hdmi_core_av_csc csc);
int hdmi_configure_lrfr(enum hdmi_range, int force_set);
void hdmi_add_notifier(struct hdmi_notifier *notifier);
void hdmi_remove_notifier(struct hdmi_notifier *notifier);
void hdmi_notify_hpd(int state);

#endif

