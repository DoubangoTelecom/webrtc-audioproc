ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES         = libsystem_wrappers.la
libsystem_wrappers_la_CPPFLAGS = -Iinterface -I.

libsystem_wrappers_la_SOURCES  = \
	source/aligned_malloc.cc\
	source/cpu.cc\
	source/cpu_features.cc\
	source/cpu_info.cc
	

if UNDER_ANDROID
libsystem_wrappers_la_SOURCES  += source/cpu_features_android.c
endif

if UNDER_WINDOWS
libsystem_wrappers_la_SOURCES  += source/cpu_win.cc
endif

if UNDER_LINUX
libsystem_wrappers_la_SOURCES  += source/cpu_linux.cc
endif

libsystem_wrappers_la_LDFLAGS = $LDFLAGS -no-undefined
if UNDER_ANDROID
libsystem_wrappers_la_LDFLAGS += -static
endif
libsystem_wrappers_ladir = $(includedir)/webrtc

src_includedir = 
src_include_HEADERS = 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = system_wrappers.pc