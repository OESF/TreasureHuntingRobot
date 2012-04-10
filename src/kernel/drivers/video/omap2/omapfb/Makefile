obj-$(CONFIG_FB_OMAP2) += omapfb.o
ifdef CONFIG_OMAP4_USE_OLD_API_VIDEO
omapfb-y := omapfb-main_old.o omapfb-sysfs.o omapfb-ioctl.o
else
omapfb-y := omapfb-main.o omapfb-sysfs.o omapfb-ioctl.o
endif
