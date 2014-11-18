AUTOMAKE_OPTIONS = foreign dist-tarZ
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = system_wrappers \
	common_audio \
	modules
	

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = webrtc-audioproc.pc


if CROSS_COMPILING
install-data-hook:
	echo "Cross-compilation is active. Do not run ldconfig";
else
install-data-hook:
	$(MKDIR_P) $(DESTDIR)/etc/ld.so.conf.d
	echo "$(libdir)" > $(DESTDIR)/etc/ld.so.conf.d/webrtc-audioproc.conf
	ldconfig
endif

if CROSS_COMPILING
	echo "Cross-compilation is active. Do not remove libs";
else
uninstall-hook:
	rm -f $(DESTDIR)/etc/ld.so.conf.d/webrtc-audioproc.conf
endif