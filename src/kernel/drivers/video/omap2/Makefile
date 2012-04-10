obj-$(CONFIG_OMAP2_VRAM) += vram.o
obj-$(CONFIG_OMAP2_VRFB) += vrfb.o

ifndef CONFIG_OMAP4_USE_OLD_API_VIDEO
 obj-$(CONFIG_OMAP2_DSS) += dss/
else
 obj-$(CONFIG_OMAP2_DSS) += dss_old/
endif

obj-$(CONFIG_FB_OMAP2) += omapfb/
obj-y += displays/
