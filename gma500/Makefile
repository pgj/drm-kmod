# $FreeBSD$

SRCDIR=	${.CURDIR:H}/drivers/gpu/drm/gma500

.PATH:	${SRCDIR}

.include "../kconfig.mk"
.include "../linuxkpi_version.mk"

KMOD=	gma500kms
SRCS=	accel_2d.c \
	backlight.c \
	blitter.c \
	cdv_device.c \
	cdv_intel_crt.c \
	cdv_intel_display.c \
	cdv_intel_dp.c \
	cdv_intel_hdmi.c \
	cdv_intel_lvds.c \
	framebuffer.c \
	gem.c \
	gma_device.c \
	gma_display.c \
	gtt.c \
	intel_bios.c \
	intel_gmbus.c \
	intel_i2c.c \
	mdfld_device.c \
	mdfld_dsi_dpi.c \
	mdfld_dsi_output.c \
	mdfld_dsi_pkg_sender.c \
	mdfld_intel_display.c \
	mdfld_output.c \
	mdfld_tmd_vid.c \
	mdfld_tpo_vid.c \
	mid_bios.c \
	mmu.c \
	oaktrail_crtc.c \
	oaktrail_device.c \
	oaktrail_hdmi_i2c.c \
	oaktrail_hdmi.c \
	oaktrail_lvds_i2c.c \
	oaktrail_lvds.c \
	opregion.c \
	power.c \
	psb_device.c \
	psb_drv.c \
	psb_intel_display.c \
	psb_intel_lvds.c \
	psb_intel_modes.c \
	psb_intel_sdvo.c \
	psb_irq.c \
	psb_lid.c \
	tc35876x-dsi-lvds.c

CLEANFILES+= ${KMOD}.ko.full ${KMOD}.ko.debug

CFLAGS+= -I${.CURDIR:H}/linuxkpi/gplv2/include
CFLAGS+= -I${.CURDIR:H}/linuxkpi/bsd/include
CFLAGS+= -I${SYSDIR}/compat/linuxkpi/common/include
CFLAGS+= -I${.CURDIR:H}/linuxkpi/dummy/include
CFLAGS+= -I${SYSDIR}/compat/linuxkpi/dummy/include

CFLAGS+= -I${.CURDIR:H}/include
CFLAGS+= -I${.CURDIR:H}/include/drm
CFLAGS+= -I${.CURDIR:H}/include/uapi
CFLAGS+= -I${.CURDIR:H}/include/drm/display
CFLAGS+= -I${SRCDIR:H:H} -I${SRCDIR} -I${SRCDIR}/display

CFLAGS+= '-DKBUILD_MODNAME="${KMOD}"'
CFLAGS+= '-DLINUXKPI_PARAM_PREFIX=gma500_' -DDRM_SYSCTL_PARAM_PREFIX=_${KMOD}
CFLAGS+= ${KCONFIG:C/(.*)/-DCONFIG_\1/}

SRCS	+=			\
	opt_acpi.h		\
	opt_compat.h		\
	opt_drm.h		\
	opt_syscons.h		\
	acpi_if.h		\
	bus_if.h		\
	fb_if.h			\
	device_if.h		\
	pci_if.h		\
	pci_iov_if.h		\
	vnode_if.h

.include <bsd.kmod.mk>

CWARNFLAGS += -Wno-pointer-arith -Wno-format -Wno-cast-qual
