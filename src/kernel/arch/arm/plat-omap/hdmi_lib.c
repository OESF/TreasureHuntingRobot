/*
 * hdmi_lib.c
 *
 * HDMI library support functions for TI OMAP processors.
 *
 * Copyright (C) 2010 Texas Instruments
 * Author: Yong Zhi <y-zhi@ti.com>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* Rev history:
 * Yong Zhi <y-zhi@ti.com>	changed SiVal macros
 * 				added PLL/PHY code
 *				added EDID code
 * 				moved PLL/PHY code to hdmi panel driver
 * 				cleanup 2/08/10
 * MythriPk <mythripk@ti.com>	Apr 2010 Modified to read extended EDID partition
 *				and handle checksum with and without extension
 *				May 2010 Added support for Hot Plug Detect.
 *
 */

#define DSS_SUBSYS_NAME "HDMI"

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <plat/hdmi_lib.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/module.h>
#include <linux/seq_file.h>

static struct {
	void __iomem *base_wp;       /*2*/
	struct hdmi_core_infoframe_avi avi_param;
	struct mutex mutex;
	struct list_head notifier_head;
} hdmi;

int count = 0, count_hpd = 0;

void hdmi_write_reg(u32 base, u16 idx, u32 val)
{
	__raw_writel(val, hdmi.base_wp + base + idx);
}

u32 hdmi_read_reg(u32 base, u16 idx)
{
	return __raw_readl(hdmi.base_wp + base + idx);
}

void hdmi_dump_regs(struct seq_file *s)
{
	#define DUMPREG(g, r) seq_printf(s, "%-35s %08x\n", #r, hdmi_read_reg(g, r))

	/*wrapper registers*/
	DUMPREG(HDMI_WP, HDMI_WP_REVISION);
	DUMPREG(HDMI_WP, HDMI_WP_SYSCONFIG);
	DUMPREG(HDMI_WP, HDMI_WP_IRQSTATUS_RAW);
	DUMPREG(HDMI_WP, HDMI_WP_IRQSTATUS);
	DUMPREG(HDMI_WP, HDMI_WP_PWR_CTRL);
	DUMPREG(HDMI_WP, HDMI_WP_IRQENABLE_SET);
	DUMPREG(HDMI_WP, HDMI_WP_VIDEO_SIZE);
	DUMPREG(HDMI_WP, HDMI_WP_VIDEO_TIMING_H);
	DUMPREG(HDMI_WP, HDMI_WP_VIDEO_TIMING_V);
	DUMPREG(HDMI_WP, HDMI_WP_WP_CLK);

	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_VND_IDL);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DEV_IDL);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DEV_IDH);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DEV_REV);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_SRST);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_SYS_STAT);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_MODE);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR_STATE);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR1);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR2);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR3);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR4);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_UMASK1);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_TMDS_CTRL);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_CTRL1_VEN_FOLLOWVSYNC);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_CTRL1_HEN_FOLLOWHSYNC);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_CTRL1_BSEL_24BITBUS);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_CTRL1_EDGE_RISINGEDGE);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DE_CTRL);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DE_TOP);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DE_CNTH);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DE_LINL);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DE_LINH_1);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_SYS_DE_DLY);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_CMD);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_STATUS);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_ADDR);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_OFFSET);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_COUNT1);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_COUNT2);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_DATA);
	DUMPREG(HDMI_CORE_SYS, HDMI_CORE_DDC_SEGM);

	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_DBYTE);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_ACR_CTRL);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_TYPE);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_VERS);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_LEN);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_CHSUM);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_HDMI_CTRL);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_DBYTE);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_DBYTE);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_DBYTE);
	DUMPREG(HDMI_CORE_AV, HDMI_CORE_AV_AVI_DBYTE);

}

#define FLD_MASK(start, end)	(((1 << (start - end + 1)) - 1) << (end))
#define FLD_VAL(val, start, end) (((val) << end) & FLD_MASK(start, end))
#define FLD_GET(val, start, end) (((val) & FLD_MASK(start, end)) >> (end))
#define FLD_MOD(orig, val, start, end) \
	(((orig) & ~FLD_MASK(start, end)) | FLD_VAL(val, start, end))

#define REG_FLD_MOD(base, idx, val, start, end) \
	hdmi_write_reg(base, idx, FLD_MOD(hdmi_read_reg(base, idx), val, start, end))

#define RD_REG_32(COMP, REG)            hdmi_read_reg(COMP, REG)
#define WR_REG_32(COMP, REG, VAL)       hdmi_write_reg(COMP, REG, (u32)(VAL))

int hdmi_get_pixel_append_position(void)
{
	printk("This is yet to be implemented");
	return 0;
}
EXPORT_SYMBOL(hdmi_get_pixel_append_position);

int hdmi_core_ddc_edid(u8 *pEDID, int ext)
{
	u32 i, j, l;
	char checksum = 0;
	u32 sts = HDMI_CORE_DDC_STATUS;
	u32 ins = HDMI_CORE_SYS;
	u32 offset = 0;

	/* Turn on CLK for DDC */
	REG_FLD_MOD(HDMI_CORE_AV, HDMI_CORE_AV_DPD, 0x7, 2, 0);

	/* Wait */
	mdelay(10);

	if (!ext) {
		/* Clk SCL Devices */
		REG_FLD_MOD(ins, HDMI_CORE_DDC_CMD, 0xA, 3, 0);

		/* HDMI_CORE_DDC_STATUS_IN_PROG */
		while (FLD_GET(hdmi_read_reg(ins, sts), 4, 4) == 1)
			;

		/* Clear FIFO */
		REG_FLD_MOD(ins, HDMI_CORE_DDC_CMD, 0x9, 3, 0);

		/* HDMI_CORE_DDC_STATUS_IN_PROG */
		while (FLD_GET(hdmi_read_reg(ins, sts), 4, 4) == 1)
			;
	} else {
		if (ext%2 != 0)
			offset = 0x80;
	}

	/* Load Segment Address Register */
	REG_FLD_MOD(ins, HDMI_CORE_DDC_SEGM, ext/2, 7, 0);

	/* Load Slave Address Register */
	REG_FLD_MOD(ins, HDMI_CORE_DDC_ADDR, 0xA0 >> 1, 7, 1);

	/* Load Offset Address Register */
	REG_FLD_MOD(ins, HDMI_CORE_DDC_OFFSET, offset, 7, 0);
	/* Load Byte Count */
	REG_FLD_MOD(ins, HDMI_CORE_DDC_COUNT1, 0x80, 7, 0);
	REG_FLD_MOD(ins, HDMI_CORE_DDC_COUNT2, 0x0, 1, 0);
	/* Set DDC_CMD */

	if (ext)
		REG_FLD_MOD(ins, HDMI_CORE_DDC_CMD, 0x4, 3, 0);
	else
	REG_FLD_MOD(ins, HDMI_CORE_DDC_CMD, 0x2, 3, 0);

	/* Yong: do not optimize this part of the code, seems
	DDC bus needs some time to get stabilized
	*/
	l = hdmi_read_reg(ins, sts);

	/* HDMI_CORE_DDC_STATUS_BUS_LOW */
	if (FLD_GET(l, 6, 6) == 1) {
		printk("I2C Bus Low?\n\r");
		return -1;
	}
	/* HDMI_CORE_DDC_STATUS_NO_ACK */
	if (FLD_GET(l, 5, 5) == 1) {
		printk("I2C No Ack\n\r");
		return -1;
	}

	i = ext * 128;
	j = 0;
	while (((FLD_GET(hdmi_read_reg(ins, sts), 4, 4) == 1)
			| (FLD_GET(hdmi_read_reg(ins, sts), 2, 2) == 0)) && j < 128) {
		if (FLD_GET(hdmi_read_reg(ins,
			sts), 2, 2) == 0) {
			/* FIFO not empty */
			pEDID[i++] = FLD_GET(hdmi_read_reg(ins, HDMI_CORE_DDC_DATA), 7, 0);
			j++;
		}
	}

	for (j = 0; j < 128; j++)
		checksum += pEDID[j];

	if (checksum != 0) {
		printk("E-EDID checksum failed!!");
		return -1;
	}
	return 0;
}

int read_edid(u8 *pEDID, u16 max_length)
{
	int r = 0, n = 0, i = 0;
	int max_ext_blocks = (max_length / 128) - 1;

	r = hdmi_core_ddc_edid(pEDID, 0);
	if (r) {
		return -1;
	} else {
		n = pEDID[0x7e];
		/* FIXME: need to comply with max_length set by the caller. Better implementation should */
		/* be to allocate necessary memory to store EDID according to nb_block field found       */
		/* in first block */
		if (n > max_ext_blocks)
			n = max_ext_blocks;

		for (i = 1; i <= n; i++) {
			r = hdmi_core_ddc_edid(pEDID, i);
			if (r)
				return -1;
		}
	}
	return 0;
}

static void hdmi_core_init(enum hdmi_deep_mode deep_color,
	struct hdmi_core_video_config_t *v_cfg,
	struct hdmi_core_audio_config *audio_cfg,
	struct hdmi_core_infoframe_avi *avi,
	struct hdmi_core_packet_enable_repeat *r_p)
{
	DBG("Enter HDMI_Core_GlobalInitVars()\n");

	/*video core*/
	switch (deep_color) {
	case HDMI_DEEP_COLOR_24BIT:
				v_cfg->CoreInputBusWide = HDMI_INPUT_8BIT;
				v_cfg->CoreOutputDitherTruncation = HDMI_OUTPUTTRUNCATION_8BIT;
				v_cfg->CoreDeepColorPacketED = HDMI_DEEPCOLORPACKECTDISABLE;
				v_cfg->CorePacketMode = HDMI_PACKETMODERESERVEDVALUE;
				break;
	case HDMI_DEEP_COLOR_30BIT:
				v_cfg->CoreInputBusWide = HDMI_INPUT_10BIT;
				v_cfg->CoreOutputDitherTruncation = HDMI_OUTPUTTRUNCATION_10BIT;
				v_cfg->CoreDeepColorPacketED = HDMI_DEEPCOLORPACKECTENABLE;
				v_cfg->CorePacketMode = HDMI_PACKETMODE30BITPERPIXEL;
				break;
	case HDMI_DEEP_COLOR_36BIT:
				v_cfg->CoreInputBusWide = HDMI_INPUT_12BIT;
				v_cfg->CoreOutputDitherTruncation = HDMI_OUTPUTTRUNCATION_12BIT;
				v_cfg->CoreDeepColorPacketED = HDMI_DEEPCOLORPACKECTENABLE;
				v_cfg->CorePacketMode = HDMI_PACKETMODE36BITPERPIXEL;
				break;
	default:
				v_cfg->CoreInputBusWide = HDMI_INPUT_8BIT;
				v_cfg->CoreOutputDitherTruncation = HDMI_OUTPUTTRUNCATION_8BIT;
				v_cfg->CoreDeepColorPacketED = HDMI_DEEPCOLORPACKECTDISABLE;
				v_cfg->CorePacketMode = HDMI_PACKETMODERESERVEDVALUE;
				break;
	}

	v_cfg->CoreHdmiDvi = HDMI_DVI;
	v_cfg->CoreTclkSelClkMult = FPLL10IDCK;
	/*audio core*/
	audio_cfg->freq_sample = HDMI_AUDIO_FS_44100;
	audio_cfg->n = 0;
	audio_cfg->cts = 0;
	audio_cfg->layout = HDMI_AUDIO_LAYOUT_2CH; /*2channel audio*/
	audio_cfg->aud_par_busclk = 0;
	audio_cfg->cts_mode = HDMI_AUDIO_CTS_MODE_HW;

	/*info frame*/
	avi->db1y_rgb_yuv422_yuv444 = 0;
	avi->db1a_active_format_off_on = 0;
	avi->db1b_no_vert_hori_verthori = 0;
	avi->db1s_0_1_2 = 0;
	avi->db2c_no_itu601_itu709_extented = 0;
	avi->db2m_no_43_169 = 0;
	avi->db2r_same_43_169_149 = 0;
	avi->db3itc_no_yes = 0;
	avi->db3ec_xvyuv601_xvyuv709 = 0;
	avi->db3q_default_lr_fr = 0;
	avi->db3sc_no_hori_vert_horivert = 0;
	avi->db4vic_videocode = 0;
	avi->db5pr_no_2_3_4_5_6_7_8_9_10 = 0;
	avi->db6_7_lineendoftop = 0 ;
	avi->db8_9_linestartofbottom = 0;
	avi->db10_11_pixelendofleft = 0;
	avi->db12_13_pixelstartofright = 0;

	/*packet enable and repeat*/
	r_p->AudioPacketED = 0;
	r_p->AudioPacketRepeat = 0;
	r_p->AVIInfoFrameED = 0;
	r_p->AVIInfoFrameRepeat = 0;
	r_p->GeneralcontrolPacketED = 0;
	r_p->GeneralcontrolPacketRepeat = 0;
	r_p->GenericPacketED = 0;
	r_p->GenericPacketRepeat = 0;
        r_p->MPEGInfoFrameED = 0;
        r_p->MPEGInfoFrameRepeat = 0;
        r_p->SPDInfoFrameED = 0;
        r_p->SPDInfoFrameRepeat = 0;
}

static void hdmi_core_powerdown_disable(void)
{
	DBG("Enter DSS_HDMI_CORE_POWER_DOWN_DISABLE()\n");
	REG_FLD_MOD(HDMI_CORE_SYS, HDMI_CORE_CTRL1, 0x0, 0, 0);
}

/* todo: power off the core */
static __attribute__ ((unused)) void hdmi_core_powerdown_enable(void)
{
	REG_FLD_MOD(HDMI_CORE_SYS, HDMI_CORE_CTRL1, 0x1, 0, 0);
}

static void hdmi_core_swreset_release(void)
{
	DBG("Enter DSS_HDMI_CORE_SW_RESET_RELEASE()\n");
	REG_FLD_MOD(HDMI_CORE_SYS, HDMI_CORE_SYS_SRST, 0x0, 0, 0);
}

static void hdmi_core_swreset_assert(void)
{
	DBG("Enter DSS_HDMI_CORE_SW_RESET_ASSERT ()\n");
	REG_FLD_MOD(HDMI_CORE_SYS, HDMI_CORE_SYS_SRST, 0x1, 0, 0);
}

/* DSS_HDMI_CORE_VIDEO_CONFIG */
static int hdmi_core_video_config(
	struct hdmi_core_video_config_t *cfg)
{
	u32 name = HDMI_CORE_SYS;
	u32 av_name = HDMI_CORE_AV;
	u32 r = 0;

	/*sys_ctrl1 default configuration not tunable*/
	u32 ven;
	u32 hen;
	u32 bsel;
	u32 edge;

	/*sys_ctrl1 default configuration not tunable*/
	ven = HDMI_CORE_CTRL1_VEN_FOLLOWVSYNC;
	hen = HDMI_CORE_CTRL1_HEN_FOLLOWHSYNC;
	bsel = HDMI_CORE_CTRL1_BSEL_24BITBUS;
	edge = HDMI_CORE_CTRL1_EDGE_RISINGEDGE;

	/*sys_ctrl1 default configuration not tunable*/
	r = hdmi_read_reg(name, HDMI_CORE_CTRL1);
	r = FLD_MOD(r, ven, 5, 5);
	r = FLD_MOD(r, hen, 4, 4);
	r = FLD_MOD(r, bsel, 2, 2);
	r = FLD_MOD(r, edge, 1, 1);
	hdmi_write_reg(name, HDMI_CORE_CTRL1, r);

	REG_FLD_MOD(name, HDMI_CORE_SYS_VID_ACEN, cfg->CoreInputBusWide, 7, 6);

	/*Vid_Mode */
	r = hdmi_read_reg(name, HDMI_CORE_SYS_VID_MODE);
	/*dither truncation configuration*/
	if (cfg->CoreOutputDitherTruncation >
				HDMI_OUTPUTTRUNCATION_12BIT) {
		r = FLD_MOD(r, cfg->CoreOutputDitherTruncation - 3, 7, 6);
		r = FLD_MOD(r, 1, 5, 5);
	} else {
		r = FLD_MOD(r, cfg->CoreOutputDitherTruncation, 7, 6);
		r = FLD_MOD(r, 0, 5, 5);
	}
	hdmi_write_reg(name, HDMI_CORE_SYS_VID_MODE, r);

	/*HDMI_Ctrl*/
	r = hdmi_read_reg(av_name, HDMI_CORE_AV_HDMI_CTRL);
	r = FLD_MOD(r, cfg->CoreDeepColorPacketED, 6, 6);
	r = FLD_MOD(r, cfg->CorePacketMode, 5, 3);
	r = FLD_MOD(r, cfg->CoreHdmiDvi, 0, 0);
	hdmi_write_reg(av_name, HDMI_CORE_AV_HDMI_CTRL, r);

	/*TMDS_CTRL*/
	REG_FLD_MOD(name, HDMI_CORE_SYS_TMDS_CTRL,
		cfg->CoreTclkSelClkMult, 6, 5);

	return 0;
}

int hdmi_core_read_avi_infoframe(struct hdmi_core_infoframe_avi *info_avi)
{
	info_avi->db1y_rgb_yuv422_yuv444 = hdmi.avi_param.db1y_rgb_yuv422_yuv444;
	info_avi->db1a_active_format_off_on = hdmi.avi_param.db1a_active_format_off_on;
	info_avi->db1b_no_vert_hori_verthori = hdmi.avi_param.db1b_no_vert_hori_verthori;
	info_avi->db1s_0_1_2 = hdmi.avi_param.db1s_0_1_2;
	info_avi->db2c_no_itu601_itu709_extented = 							hdmi.avi_param.db2c_no_itu601_itu709_extented;
	info_avi->db2m_no_43_169 = hdmi.avi_param.db2m_no_43_169;
	info_avi->db2r_same_43_169_149 = hdmi.avi_param.db2r_same_43_169_149;
	info_avi->db3itc_no_yes = hdmi.avi_param.db3itc_no_yes;
	info_avi->db3ec_xvyuv601_xvyuv709 = hdmi.avi_param.db3ec_xvyuv601_xvyuv709;
	info_avi->db3q_default_lr_fr = hdmi.avi_param.db3q_default_lr_fr;
	info_avi->db3sc_no_hori_vert_horivert = hdmi.avi_param.db3sc_no_hori_vert_horivert;
	info_avi->db4vic_videocode = hdmi.avi_param.db4vic_videocode;
	info_avi->db5pr_no_2_3_4_5_6_7_8_9_10 = hdmi.avi_param.db5pr_no_2_3_4_5_6_7_8_9_10;
	info_avi->db6_7_lineendoftop = hdmi.avi_param.db6_7_lineendoftop;
	info_avi->db8_9_linestartofbottom = hdmi.avi_param.db8_9_linestartofbottom;
	info_avi->db10_11_pixelendofleft = hdmi.avi_param.db10_11_pixelendofleft;
	info_avi->db12_13_pixelstartofright = hdmi.avi_param.db12_13_pixelstartofright;
	return 0;
}

static int hdmi_core_audio_infoframe_avi(struct hdmi_core_infoframe_avi info_avi)
{
	u16 offset;
	int dbyte, dbyte_size;
	u32 val;
	char sum = 0, checksum = 0;

	dbyte = HDMI_CORE_AV_AVI_DBYTE;
	dbyte_size = HDMI_CORE_AV_AVI_DBYTE_ELSIZE;
	/*info frame video*/
	sum += 0x82 + 0x002 + 0x00D;
	hdmi_write_reg(HDMI_CORE_AV, HDMI_CORE_AV_AVI_TYPE, 0x082);
	hdmi_write_reg(HDMI_CORE_AV, HDMI_CORE_AV_AVI_VERS, 0x002);
	hdmi_write_reg(HDMI_CORE_AV, HDMI_CORE_AV_AVI_LEN, 0x00D);

	offset = dbyte + (0 * dbyte_size);
	val = (info_avi.db1y_rgb_yuv422_yuv444 << 5) |
		(info_avi.db1a_active_format_off_on << 4) |
		(info_avi.db1b_no_vert_hori_verthori << 2) |
		(info_avi.db1s_0_1_2);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (1 * dbyte_size);
	val = (info_avi.db2c_no_itu601_itu709_extented << 6) |
		(info_avi.db2m_no_43_169 << 4) |
		(info_avi.db2r_same_43_169_149);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (2 * dbyte_size);
	val = (info_avi.db3itc_no_yes << 7) |
		(info_avi.db3ec_xvyuv601_xvyuv709 << 4) |
		(info_avi.db3q_default_lr_fr << 2) |
		(info_avi.db3sc_no_hori_vert_horivert);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (3 * dbyte_size);
	hdmi_write_reg(HDMI_CORE_AV, offset, info_avi.db4vic_videocode);
	sum += info_avi.db4vic_videocode;

	offset = dbyte + (4 * dbyte_size);
	val = info_avi.db5pr_no_2_3_4_5_6_7_8_9_10;
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (5 * dbyte_size);
	val = info_avi.db6_7_lineendoftop & 0x00FF;
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (6 * dbyte_size);
	val = ((info_avi.db6_7_lineendoftop >> 8) & 0x00FF);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (7 * dbyte_size);
	val = info_avi.db8_9_linestartofbottom & 0x00FF;
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (8 * dbyte_size);
	val = ((info_avi.db8_9_linestartofbottom >> 8) & 0x00FF);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (9 * dbyte_size);
	val = info_avi.db10_11_pixelendofleft & 0x00FF;
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (10 * dbyte_size);
	val = ((info_avi.db10_11_pixelendofleft >> 8) & 0x00FF);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	offset = dbyte + (11 * dbyte_size);
	val = info_avi.db12_13_pixelstartofright & 0x00FF;
	hdmi_write_reg(HDMI_CORE_AV, offset , val);
	sum += val;

	offset = dbyte + (12 * dbyte_size);
	val = ((info_avi.db12_13_pixelstartofright >> 8) & 0x00FF);
	hdmi_write_reg(HDMI_CORE_AV, offset, val);
	sum += val;

	checksum = 0x100 - sum;
	hdmi_write_reg(HDMI_CORE_AV, HDMI_CORE_AV_AVI_CHSUM, checksum);

	return 0;
}

int hdmi_configure_csc(enum hdmi_core_av_csc csc)
{
	int var;
	switch (csc) {
	/*Setting the AVI infroframe to respective color mode
	* As the quantization is in default mode ie it selects
	* full range for RGB (except for VGA ) and limited range
	* for YUV we dont have to make any changes for this */
	case RGB:
			hdmi.avi_param.db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_RGB;
			hdmi_core_audio_infoframe_avi(hdmi.avi_param);
			var = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN);
			var = FLD_MOD(var, 0, 2, 2);
			hdmi_write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN, var);
			break;
	case RGB_TO_YUV:
			hdmi.avi_param.db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_YUV422;
			hdmi_core_audio_infoframe_avi(hdmi.avi_param);
			var = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN);
			var = FLD_MOD(var, 1, 2, 2);
			hdmi_write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN, var);
			break;
	case YUV_TO_RGB:
			hdmi.avi_param.db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_RGB;
			hdmi_core_audio_infoframe_avi(hdmi.avi_param);
			var = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_MODE);
			var = FLD_MOD(var, 1, 3, 3);
			hdmi_write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_MODE, var);
			break;
	default:
			break;
	}
	return 0;

}
int hdmi_configure_lrfr(enum hdmi_range lr_fr, int force_set)
{
	int var;
	switch (lr_fr) {
	/*Setting the AVI infroframe to respective limited range
	* 0 if for limited range 1 for full range */
	case HDMI_LIMITED_RANGE:
			hdmi.avi_param.db3q_default_lr_fr = INFOFRAME_AVI_DB3Q_LR;
			hdmi_core_audio_infoframe_avi(hdmi.avi_param);
			if (force_set) {
				var = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN);
				var = FLD_MOD(var, 1, 1, 1);
				hdmi_write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_ACEN, var);
			}
			break;
	case HDMI_FULL_RANGE:
			if (hdmi.avi_param.db1y_rgb_yuv422_yuv444 == INFOFRAME_AVI_DB1Y_YUV422) {
				printk(KERN_ERR"It is only limited range for YUV");
				return -1;
			}
			hdmi.avi_param.db3q_default_lr_fr = INFOFRAME_AVI_DB3Q_FR;
			hdmi_core_audio_infoframe_avi(hdmi.avi_param);
			if (force_set) {
				var = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_MODE);
				var = FLD_MOD(var, 1, 4, 4);
				hdmi_write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_VID_MODE, var);
			}

			break;
	default:
			break;
	}
	return 0;

}

static int hdmi_core_av_packet_config(u32 name,
	struct hdmi_core_packet_enable_repeat r_p)
{

	/*enable/repeat the infoframe*/
	hdmi_write_reg(name, HDMI_CORE_AV_PB_CTRL1,
		(r_p.MPEGInfoFrameED << 7)|
		(r_p.MPEGInfoFrameRepeat << 6)|
		(r_p.AudioPacketED << 5)|
		(r_p.AudioPacketRepeat << 4)|
		(r_p.SPDInfoFrameED << 3)|
		(r_p.SPDInfoFrameRepeat << 2)|
		(r_p.AVIInfoFrameED << 1)|
		(r_p.AVIInfoFrameRepeat));

	/*enable/repeat the packet*/
	hdmi_write_reg(name, HDMI_CORE_AV_PB_CTRL2,
		(r_p.GeneralcontrolPacketED << 3)|
		(r_p.GeneralcontrolPacketRepeat << 2)|
		(r_p.GenericPacketED << 1)|
		(r_p.GenericPacketRepeat));
	return 0;
}

static void hdmi_w1_init(struct hdmi_video_timing *t_p,
	struct hdmi_video_format *f_p,
	struct hdmi_video_interface *i_p,
	struct hdmi_irq_vector *pIrqVectorEnable,
	struct hdmi_audio_format *audio_fmt,
	struct hdmi_audio_dma *audio_dma)
{
	DBG("Enter HDMI_W1_GlobalInitVars()\n");

	t_p->horizontalBackPorch = 0;
	t_p->horizontalFrontPorch = 0;
	t_p->horizontalSyncPulse = 0;
	t_p->verticalBackPorch = 0;
	t_p->verticalFrontPorch = 0;
	t_p->verticalSyncPulse = 0;

	f_p->packingMode = HDMI_PACK_10b_RGB_YUV444;
	f_p->linePerPanel = 0;
	f_p->pixelPerLine = 0;

	i_p->vSyncPolarity = 0;
	i_p->hSyncPolarity = 0;

	i_p->interlacing = 0;
	i_p->timingMode = 0; /* HDMI_TIMING_SLAVE */

	pIrqVectorEnable->pllRecal = 0;
	pIrqVectorEnable->pllUnlock = 0;
	pIrqVectorEnable->pllLock = 0;
	pIrqVectorEnable->phyDisconnect = 1;
	pIrqVectorEnable->phyConnect = 1;
	pIrqVectorEnable->phyShort5v = 0;
	pIrqVectorEnable->videoEndFrame = 0;
	pIrqVectorEnable->videoVsync = 0;
	pIrqVectorEnable->fifoSampleRequest = 0;
	pIrqVectorEnable->fifoOverflow = 0;
	pIrqVectorEnable->fifoUnderflow = 0;
	pIrqVectorEnable->ocpTimeOut = 0;
	pIrqVectorEnable->core = 1;
}


static void hdmi_w1_irq_enable(struct hdmi_irq_vector *pIrqVectorEnable)
{
	u32 r = 0;

	r = ((pIrqVectorEnable->pllRecal << 31) |
		(pIrqVectorEnable->pllUnlock << 30) |
		(pIrqVectorEnable->pllLock << 29) |
		(pIrqVectorEnable->phyDisconnect << 26) |
		(pIrqVectorEnable->phyConnect << 25) |
		(pIrqVectorEnable->phyShort5v << 24) |
		(pIrqVectorEnable->videoEndFrame << 17) |
		(pIrqVectorEnable->videoVsync << 16) |
		(pIrqVectorEnable->fifoSampleRequest << 10) |
		(pIrqVectorEnable->fifoOverflow << 9) |
		(pIrqVectorEnable->fifoUnderflow << 8) |
		(pIrqVectorEnable->ocpTimeOut << 4) |
		(pIrqVectorEnable->core << 0));

	hdmi_write_reg(HDMI_WP, HDMI_WP_IRQENABLE_SET, r);
}

static inline int hdmi_w1_wait_for_bit_change(const u32 ins,
	u32 idx, int b2, int b1, int val)
{
	int t = 0;
	while (val != FLD_GET(hdmi_read_reg(ins, idx), b2, b1)) {
		udelay(1);
		if (t++ > 1000)
			return !val;
	}
	return val;
}

/* todo: add timeout value */
int hdmi_w1_set_wait_srest(void)
{
	/* reset W1 */
	REG_FLD_MOD(HDMI_WP, HDMI_WP_SYSCONFIG, 0x1, 0, 0);

	/* wait till SOFTRESET == 0 */
	while (FLD_GET(hdmi_read_reg(HDMI_WP, HDMI_WP_SYSCONFIG), 0, 0))
		;
	return 0;
}

/* PHY_PWR_CMD */
int hdmi_w1_set_wait_phy_pwr(HDMI_PhyPwr_t val)
{
	DBG("*** Set PHY power mode to %d\n", val);
	REG_FLD_MOD(HDMI_WP, HDMI_WP_PWR_CTRL, val, 7, 6);

	if (hdmi_w1_wait_for_bit_change(HDMI_WP,
		HDMI_WP_PWR_CTRL, 5, 4, val) != val) {
		ERR("Failed to set PHY power mode to %d\n", val);
		return -ENODEV;
	}
	return 0;
}

/* PLL_PWR_CMD */
int hdmi_w1_set_wait_pll_pwr(HDMI_PllPwr_t val)
{
	REG_FLD_MOD(HDMI_WP, HDMI_WP_PWR_CTRL, val, 3, 2);

	/* wait till PHY_PWR_STATUS=ON */
	if (hdmi_w1_wait_for_bit_change(HDMI_WP,
		HDMI_WP_PWR_CTRL, 1, 0, val) != val) {
		ERR("Failed to set PHY_PWR_STATUS to ON\n");
		return -ENODEV;
	}

	return 0;
}

void hdmi_w1_video_stop(void)
{
	REG_FLD_MOD(HDMI_WP, HDMI_WP_VIDEO_CFG, 0, 31, 31);
}

void hdmi_w1_video_start(void)
{
	REG_FLD_MOD(HDMI_WP, HDMI_WP_VIDEO_CFG, (u32)0x1, 31, 31);
}

static void hdmi_w1_video_init_format(struct hdmi_video_format *f_p,
	struct hdmi_video_timing *t_p, struct hdmi_config *param)
{
	DBG("Enter HDMI_W1_ConfigVideoResolutionTiming()\n");

	f_p->linePerPanel = param->lpp;
	f_p->pixelPerLine = param->ppl;

	t_p->horizontalBackPorch = param->hbp;
	t_p->horizontalFrontPorch = param->hfp;
	t_p->horizontalSyncPulse = param->hsw;
	t_p->verticalBackPorch = param->vbp;
	t_p->verticalFrontPorch = param->vfp;
	t_p->verticalSyncPulse = param->vsw;
}

static void hdmi_w1_video_config_format(
	struct hdmi_video_format *f_p)
{
	u32 l = 0;

	REG_FLD_MOD(HDMI_WP, HDMI_WP_VIDEO_CFG, f_p->packingMode, 10, 8);

	l |= FLD_VAL(f_p->linePerPanel, 31, 16);
	l |= FLD_VAL(f_p->pixelPerLine, 15, 0);
	hdmi_write_reg(HDMI_WP, HDMI_WP_VIDEO_SIZE, l);
}

static void hdmi_w1_video_config_interface(
	struct hdmi_video_interface *i_p)
{
	u32 r;
	DBG("Enter HDMI_W1_ConfigVideoInterface()\n");

	r = hdmi_read_reg(HDMI_WP, HDMI_WP_VIDEO_CFG);
	r = FLD_MOD(r, i_p->vSyncPolarity, 7, 7);
	r = FLD_MOD(r, i_p->hSyncPolarity, 6, 6);
	r = FLD_MOD(r, i_p->interlacing, 3, 3);
	r = FLD_MOD(r, i_p->timingMode, 1, 0);
	hdmi_write_reg(HDMI_WP, HDMI_WP_VIDEO_CFG, r);
}

static void hdmi_w1_video_config_timing(
	struct hdmi_video_timing *t_p)
{
	u32 timing_h = 0;
	u32 timing_v = 0;

	DBG("Enter HDMI_W1_ConfigVideoTiming ()\n");

	timing_h |= FLD_VAL(t_p->horizontalBackPorch, 31, 20);
	timing_h |= FLD_VAL(t_p->horizontalFrontPorch, 19, 8);
	timing_h |= FLD_VAL(t_p->horizontalSyncPulse, 7, 0);
	hdmi_write_reg(HDMI_WP, HDMI_WP_VIDEO_TIMING_H, timing_h);

	timing_v |= FLD_VAL(t_p->verticalBackPorch, 31, 20);
	timing_v |= FLD_VAL(t_p->verticalFrontPorch, 19, 8);
	timing_v |= FLD_VAL(t_p->verticalSyncPulse, 7, 0);
	hdmi_write_reg(HDMI_WP, HDMI_WP_VIDEO_TIMING_V, timing_v);
}

int hdmi_lib_enable(struct hdmi_config *cfg)
{
	u32 r, deep_color = 0;

	u32 av_name = HDMI_CORE_AV;

	/*HDMI*/
	struct hdmi_video_timing VideoTimingParam;
	struct hdmi_video_format VideoFormatParam;
	struct hdmi_video_interface VideoInterfaceParam;
	struct hdmi_irq_vector IrqHdmiVectorEnable;
	struct hdmi_audio_format audio_fmt;
	struct hdmi_audio_dma audio_dma;

	/*HDMI core*/
	struct hdmi_core_video_config_t v_core_cfg;
	struct hdmi_core_audio_config audio_cfg;
	struct hdmi_core_packet_enable_repeat repeat_param;

	hdmi_w1_init(&VideoTimingParam, &VideoFormatParam,
		&VideoInterfaceParam, &IrqHdmiVectorEnable,
		&audio_fmt, &audio_dma);

	hdmi_core_init(cfg->deep_color, &v_core_cfg,
		&audio_cfg,
		&hdmi.avi_param,
		&repeat_param);

	/* Enable PLL Lock and UnLock intrerrupts */
	IrqHdmiVectorEnable.pllUnlock = 1;
	IrqHdmiVectorEnable.pllLock = 1;

	/***************** init DSS register **********************/
	hdmi_w1_irq_enable(&IrqHdmiVectorEnable);

	hdmi_w1_video_init_format(&VideoFormatParam,
			&VideoTimingParam, cfg);

	hdmi_w1_video_config_timing(&VideoTimingParam);

	/*video config*/
	switch (cfg->deep_color) {
	case 0:
		VideoFormatParam.packingMode = HDMI_PACK_24b_RGB_YUV444_YUV422;
		VideoInterfaceParam.timingMode = HDMI_TIMING_MASTER_24BIT;
		break;
	case 1:
		VideoFormatParam.packingMode = HDMI_PACK_10b_RGB_YUV444;
		VideoInterfaceParam.timingMode = HDMI_TIMING_MASTER_30BIT;
		break;
	case 2:
		VideoFormatParam.packingMode = HDMI_PACK_ALREADYPACKED;
		VideoInterfaceParam.timingMode = HDMI_TIMING_MASTER_36BIT;
		break;
	}

	hdmi_w1_video_config_format(&VideoFormatParam);

	/* FIXME */
	VideoInterfaceParam.vSyncPolarity = cfg->v_pol;
	VideoInterfaceParam.hSyncPolarity = cfg->h_pol;
	VideoInterfaceParam.interlacing = cfg->interlace;

	hdmi_w1_video_config_interface(&VideoInterfaceParam);

#if 0
	/* hnagalla */
	val = hdmi_read_reg(HDMI_WP, HDMI_WP_VIDEO_SIZE);

	val &= 0x0FFFFFFF;
	val |= ((0x1f) << 27); /* wakeup */
	hdmi_write_reg(HDMI_WP, HDMI_WP_VIDEO_SIZE, val);
#endif

	/****************************** CORE *******************************/
	/************* configure core video part ********************************/
	/*set software reset in the core*/
	hdmi_core_swreset_assert();

	/*power down off*/
	hdmi_core_powerdown_disable();

	v_core_cfg.CoreHdmiDvi = cfg->hdmi_dvi;

	r = hdmi_read_reg(HDMI_WP, HDMI_WP_VIDEO_CFG);
	switch(r & 0x03) {
	case 1:
		deep_color = 100;
		break;
	case 2:
		deep_color = 125;
		break;
	case 3:
		deep_color = 150;
		break;
	case 4:
		printk(KERN_ERR "Invalid deep color configuration, "
				"using no deep-color\n");
		deep_color = 100;
		break;
	}

	r = hdmi_core_video_config(&v_core_cfg);

	/*release software reset in the core*/
	hdmi_core_swreset_release();

	/*configure packet*/
	/*info frame video see doc CEA861-D page 65*/
	hdmi.avi_param.db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_RGB;
	hdmi.avi_param.db1a_active_format_off_on =
		INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_OFF;
	hdmi.avi_param.db1b_no_vert_hori_verthori = INFOFRAME_AVI_DB1B_NO;
	hdmi.avi_param.db1s_0_1_2 = INFOFRAME_AVI_DB1S_0;
	hdmi.avi_param.db2c_no_itu601_itu709_extented = INFOFRAME_AVI_DB2C_NO;
	hdmi.avi_param.db2m_no_43_169 = INFOFRAME_AVI_DB2M_NO;
	hdmi.avi_param.db2r_same_43_169_149 = INFOFRAME_AVI_DB2R_SAME;
	hdmi.avi_param.db3itc_no_yes = INFOFRAME_AVI_DB3ITC_NO;
	hdmi.avi_param.db3ec_xvyuv601_xvyuv709 = INFOFRAME_AVI_DB3EC_XVYUV601;
	hdmi.avi_param.db3q_default_lr_fr = INFOFRAME_AVI_DB3Q_DEFAULT;
	hdmi.avi_param.db3sc_no_hori_vert_horivert = INFOFRAME_AVI_DB3SC_NO;
	hdmi.avi_param.db4vic_videocode = cfg->video_format;
	hdmi.avi_param.db5pr_no_2_3_4_5_6_7_8_9_10 = INFOFRAME_AVI_DB5PR_NO;
	hdmi.avi_param.db6_7_lineendoftop = 0;
	hdmi.avi_param.db8_9_linestartofbottom = 0;
	hdmi.avi_param.db10_11_pixelendofleft = 0;
	hdmi.avi_param.db12_13_pixelstartofright = 0;

	r = hdmi_core_audio_infoframe_avi(hdmi.avi_param);

	/*enable/repeat the infoframe*/
	repeat_param.AVIInfoFrameED = PACKETENABLE;
	repeat_param.AVIInfoFrameRepeat = PACKETREPEATON;
	/* wakeup */
	repeat_param.AudioPacketED = PACKETENABLE;
	repeat_param.AudioPacketRepeat = PACKETREPEATON;
        /* ISCR1 transmission */
	repeat_param.MPEGInfoFrameED = PACKETENABLE;
	repeat_param.MPEGInfoFrameRepeat = PACKETREPEATON;
        /* ACP transmission */
	repeat_param.SPDInfoFrameED = PACKETENABLE;
	repeat_param.SPDInfoFrameRepeat = PACKETREPEATON;

	r = hdmi_core_av_packet_config(av_name, repeat_param);

	REG_FLD_MOD(av_name, HDMI_CORE_AV_HDMI_CTRL, cfg->hdmi_dvi, 0, 0);
	return r;
}

int hdmi_lib_init(void){
	u32 rev;

	hdmi.base_wp = ioremap(HDMI_BASE, 0x1000);

	if (!hdmi.base_wp) {
		ERR("can't ioremap WP\n");
		return -ENOMEM;
	}

	mutex_init(&hdmi.mutex);
	INIT_LIST_HEAD(&hdmi.notifier_head);

	rev = hdmi_read_reg(HDMI_WP, HDMI_WP_REVISION);

	printk(KERN_INFO "OMAP HDMI W1 rev %d.%d\n",
		FLD_GET(rev, 10, 8), FLD_GET(rev, 5, 0));

	return 0;
}

void hdmi_lib_exit(void){
	iounmap(hdmi.base_wp);
}

int hdmi_set_irqs(void)
{
	u32 r = 0 , hpd = 0;
	struct hdmi_irq_vector pIrqVectorEnable;

	pIrqVectorEnable.pllRecal = 0;
	pIrqVectorEnable.phyShort5v = 0;
	pIrqVectorEnable.videoEndFrame = 0;
	pIrqVectorEnable.videoVsync = 0;
	pIrqVectorEnable.fifoSampleRequest = 0;
	pIrqVectorEnable.fifoOverflow = 0;
	pIrqVectorEnable.fifoUnderflow = 0;
	pIrqVectorEnable.ocpTimeOut = 0;
	pIrqVectorEnable.pllUnlock = 1;
	pIrqVectorEnable.pllLock = 1;
	pIrqVectorEnable.phyDisconnect = 1;
	pIrqVectorEnable.phyConnect = 1;
	pIrqVectorEnable.core = 1;

	hdmi_w1_irq_enable(&pIrqVectorEnable);

	r = hdmi_read_reg(HDMI_WP, HDMI_WP_IRQENABLE_SET);
	DBG("Irqenable %x \n", r);
	hpd = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_UMASK1);
	DBG("%x hpd\n", hpd);
	return 0;
}

/* Interrupt handler*/
void HDMI_W1_HPD_handler(int *r)
{
	u32 val, set = 0, hpd_intr;

	mdelay(30);
	DBG("-------------DEBUG-------------------");
	DBG("%x hdmi_wp_irqstatus\n", \
		hdmi_read_reg(HDMI_WP, HDMI_WP_IRQSTATUS));
	DBG("%x hdmi_core_intr_state\n", \
		hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR_STATE));
	DBG("%x hdmi_core_irqstate\n", \
		hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR1));
	DBG("%x hdmi_core_sys_sys_stat\n", \
		hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_SYS_STAT));
	DBG("-------------DEBUG-------------------");

	val = hdmi_read_reg(HDMI_WP, HDMI_WP_IRQSTATUS);
	mdelay(30);

	set = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_SYS_STAT);
	mdelay(30);

	hpd_intr = hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR1);
	mdelay(30);

	hdmi_write_reg(HDMI_WP, HDMI_WP_IRQSTATUS, val);
	/* flush posted write */
	hdmi_read_reg(HDMI_WP, HDMI_WP_IRQSTATUS);
	mdelay(30);

	if (val & 0x02000000) {
		DBG("connect, ");
		*r = HDMI_CONNECT;
	}
	if (hpd_intr & 0x00000040) {
		if  (set & 0x00000002)
			*r = HDMI_HPD;
		else
			*r = HDMI_DISCONNECT;
	}
	if ((val & 0x04000000) && (!(val & 0x02000000))) {
		DBG("Disconnect");
		*r = HDMI_DISCONNECT;
	}
	/* flush posted write */
	hdmi_write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR1, hpd_intr);
	mdelay(30);
	hdmi_set_irqs();
	/*Read to flush*/
	hdmi_read_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR1);
}


/* wrapper functions to be used until L24.5 release*/
int HDMI_CORE_DDC_READEDID(u32 name, u8 *p, u16 max_length)
{
	int r = read_edid(p, max_length);
	return r;
}

int HDMI_W1_StopVideoFrame(u32 name)
{
	DBG("Enter HDMI_W1_StopVideoFrame()\n");
	hdmi_w1_video_stop();
	return 0;
}

int HDMI_W1_StartVideoFrame(u32 name)
{
	DBG("Enter HDMI_W1_StartVideoFrame  ()\n");
	hdmi_w1_video_start();
	return 0;
}

/* PHY_PWR_CMD */
int HDMI_W1_SetWaitPhyPwrState(u32 name,
	HDMI_PhyPwr_t param)
{
	int r = hdmi_w1_set_wait_phy_pwr(param);
	return r;
}

/* PLL_PWR_CMD */
int HDMI_W1_SetWaitPllPwrState(u32 name,
		HDMI_PllPwr_t param)
{
	int r = hdmi_w1_set_wait_pll_pwr(param);
	return r;
}

int HDMI_W1_SetWaitSoftReset(void)
{
	/* reset W1 */
	REG_FLD_MOD(HDMI_WP, HDMI_WP_SYSCONFIG, 0x1, 0, 0);

	/* wait till SOFTRESET == 0 */
	while (FLD_GET(hdmi_read_reg(HDMI_WP, HDMI_WP_SYSCONFIG), 0, 0))
		;
	return 0;
}

void hdmi_add_notifier(struct hdmi_notifier *notifier)
{
	mutex_lock(&hdmi.mutex);
	list_add_tail(&notifier->list, &hdmi.notifier_head);
	mutex_unlock(&hdmi.mutex);
}

void hdmi_remove_notifier(struct hdmi_notifier *notifier)
{
	struct hdmi_notifier *cur, *next;

	list_for_each_entry_safe(cur, next, &hdmi.notifier_head, list) {
		if (cur == notifier) {
			mutex_lock(&hdmi.mutex);
			list_del(&cur->list);
			mutex_unlock(&hdmi.mutex);
		}
	}
}

void hdmi_notify_hpd(int state)
{
	struct hdmi_notifier *cur, *next;

	list_for_each_entry_safe(cur, next, &hdmi.notifier_head, list) {
		if (cur->hpd_notifier)
			cur->hpd_notifier(state, cur->private_data);
	}
}
