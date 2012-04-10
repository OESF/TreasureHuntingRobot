/*
 * linux/drivers/video/omap2/dss/wb.c
 * Copyright (C) 2009 Texas Instruments
 * Author: mythripk <mythripk@ti.com>
 * Some code and ideas taken from drivers/video/omap/ driver
 * by Imre Deak.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define DSS_SUBSYS_NAME "WRITEBACK"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <video/omapdss.h>
#include <plat/cpu.h>

#include "dss.h"

static struct list_head wb_list;

static ssize_t wb_buffer_state_show(struct omap_writeback *wb, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", wb->info.buffer_state);
}

static ssize_t wb_buffer_state_store(struct omap_writeback *wb, const char *buf,
		size_t size)
{
	int r;
	struct omap_writeback_info info;

	if (!dss_get_mainclk_state()) {
		DSSERR("mainclk disabled while trying"
			"wb_buffer_state_store, returning\n");
		return 0;
	}

	wb->get_wb_info(wb, &info);

	info.buffer_state = simple_strtoul(buf, NULL, 10);

	r = dispc_move_wb_buffers(info.buffer_state);
	if (r) {
		DSSERR("failed to move buffers\n");
		return r;
	}
	r = wb->set_wb_info(wb, &info);
	if (r)
		return r;

	return size;
}

struct wb_attribute {
	struct attribute attr;
	ssize_t (*show)(struct omap_writeback *, char *);
	ssize_t	(*store)(struct omap_writeback *, const char *, size_t);
};

#define WB_ATTR(_name, _mode, _show, _store) \
	struct wb_attribute wb_attr_##_name = \
	__ATTR(_name, _mode, _show, _store)

static WB_ATTR(buffer_state, S_IRUGO|S_IWUSR,
		wb_buffer_state_show, wb_buffer_state_store);

static struct attribute *writeback_sysfs_attrs[] = {
	&wb_attr_buffer_state.attr,
	NULL
};

static ssize_t writeback_attr_show(struct kobject *kobj, struct attribute *attr,
		char *buf)
{
	struct omap_writeback *wb;
	struct wb_attribute *wb_attr;

	wb = container_of(kobj, struct omap_writeback, kobj);
	wb_attr = container_of(attr, struct wb_attribute, attr);

	if (!wb_attr->show)
		return -ENOENT;

	return wb_attr->show(wb, buf);
}

static ssize_t writeback_attr_store(struct kobject *kobj, struct attribute *attr,
		const char *buf, size_t size)
{
	struct omap_writeback *wb;
	struct wb_attribute *wb_attr;

	wb = container_of(kobj, struct omap_writeback, kobj);
	wb_attr = container_of(attr, struct wb_attribute, attr);

	if (!wb_attr->store)
		return -ENOENT;

	return wb_attr->store(wb, buf, size);
}

static struct sysfs_ops writeback_sysfs_ops = {
	.show = writeback_attr_show,
	.store = writeback_attr_store,
};

static struct kobj_type writeback_ktype = {
	.sysfs_ops = &writeback_sysfs_ops,
	.default_attrs = writeback_sysfs_attrs,
};

bool omap_dss_check_wb(struct writeback_cache_data *wb, int overlayId,
		int managerId)
{
	bool result = false;

	DSSDBG("ovl=%d,mgr=%d,srcty=%d(%s),src=%d\n",
		overlayId, managerId, wb->source_type,
		wb->source_type == OMAP_WB_SOURCE_OVERLAY ?
		"OMAP_WB_SOURCE_OVERLAY" :
		wb->source_type == OMAP_WB_SOURCE_MANAGER ?
		"OMAP_WB_SOURCE_MANAGER" : "???",
		wb->source);

	if ((wb->source_type == OMAP_WB_SOURCE_OVERLAY) &&
			((wb->source - 3) == overlayId)) {
		result = true;
	} else if (wb->source_type == OMAP_WB_SOURCE_MANAGER) {
		switch (wb->source) {
		case OMAP_WB_LCD_1_MANAGER:
			if (managerId == OMAP_DSS_CHANNEL_LCD)
				result = true;
		case OMAP_WB_LCD_2_MANAGER:
			if (managerId == OMAP_DSS_CHANNEL_LCD2)
				result = true;
		case OMAP_WB_TV_MANAGER:
			if (managerId == OMAP_DSS_CHANNEL_DIGIT)
				result = true;
		case OMAP_WB_OVERLAY0:
		case OMAP_WB_OVERLAY1:
		case OMAP_WB_OVERLAY2:
		case OMAP_WB_OVERLAY3:
			break;
		}
	}

	return result;
}

static bool dss_check_wb(struct omap_writeback *wb)
{
	DSSDBG("srcty=%d(%s),src=%d\n", wb->info.source_type,
		wb->info.source_type == OMAP_WB_SOURCE_OVERLAY ?
		"OMAP_WB_SOURCE_OVERLAY" :
		wb->info.source_type == OMAP_WB_SOURCE_MANAGER ?
		"OMAP_WB_SOURCE_MANAGER" : "???", wb->info.source);

	return 0;
}

static int omap_dss_wb_set_info(struct omap_writeback *wb,
		struct omap_writeback_info *info)
{
	int r;
	struct omap_writeback_info old_info;
	old_info = wb->info;
	wb->info = *info;

	r = dss_check_wb(wb);
	if (r) {
		wb->info = old_info;
		return r;
	}

	wb->info_dirty = true;

	return 0;
}

static void omap_dss_wb_get_info(struct omap_writeback *wb,
		struct omap_writeback_info *info)
{
	*info = wb->info;
}

struct omap_writeback *omap_dss_get_wb(int num)
{
	int i = 0;
	struct omap_writeback *wb;

	list_for_each_entry(wb, &wb_list, list) {
		if (i++ == num)
			return wb;
	}

	return NULL;
}

static __attribute__ ((unused)) void omap_dss_add_wb(struct omap_writeback *wb)
{
	list_add_tail(&wb->list, &wb_list);
}


void dss_init_writeback(struct platform_device *pdev)
{
	int r;
	struct omap_writeback *wb;

	INIT_LIST_HEAD(&wb_list);

	wb = kzalloc(sizeof(*wb), GFP_KERNEL);

	BUG_ON(wb == NULL);

	wb->check_wb = &dss_check_wb;
	wb->set_wb_info = &omap_dss_wb_set_info;
	wb->get_wb_info = &omap_dss_wb_get_info;

	mutex_init(&wb->lock);

	omap_dss_add_wb(wb);

	r = kobject_init_and_add(&wb->kobj, &writeback_ktype,
		&pdev->dev.kobj, "writeback", 0);

	if (r)
		DSSERR("failed to create sysfs file\n");
}
