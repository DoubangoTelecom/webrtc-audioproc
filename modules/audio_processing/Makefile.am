lib_LTLIBRARIES         = libaudio_processing.la
libaudio_processing_la_LIBADD = ../../system_wrappers/libsystem_wrappers.la ../../common_audio/libcommon_audio.la
libaudio_processing_la_CPPFLAGS = -I. -I../interface -Iutility -Iinclude -Iaec/include -Iaecm/include -Iagc/include -Ins/include -I../../common_audio/signal_processing/include -I../../system_wrappers/interface

libaudio_processing_la_SOURCES =       aec/aec_core.c\
	aec/aec_rdft.c\
	aec/aec_resampler.c\
	aec/echo_cancellation.c\
	\
	aecm/aecm_core.c\
	aecm/echo_control_mobile.c\
	\
	agc/analog_agc.c\
	agc/digital_agc.c\
	\
	ns/noise_suppression.c\
	ns/noise_suppression_x.c\
	ns/ns_core.c\
	ns/nsx_core.c\
	\
	utility/delay_estimator.c\
	utility/delay_estimator_wrapper.c\
	utility/fft4g.c\
	utility/ring_buffer.c\
	\
	processing_component.cc\
	splitting_filter.cc

if UNDER_X86
libaudio_processing_la_SOURCES += aec/aec_core_sse2.c\
	aec/aec_rdft_sse2.c
endif

if UNDER_ARM
endif

if UNDER_ARM_V7
endif

if UNDER_ARM_NEON
libaudio_processing_la_SOURCES += aecm/aecm_core_neon.S\
	aecm/aecm_core_neon_offsets.c\
	ns/nsx_core_neon.S\
	ns/nsx_core_neon_offsets.c
endif
#ns/nsx_core_neon.c
#aecm/aecm_core_neon.c

libaudio_processing_la_LDFLAGS = $LDFLAGS -no-undefined
if UNDER_ANDROID
libaudio_processing_la_LDFLAGS += -static
endif

api_includedir = $(includedir)/webrtc
api_include_HEADERS = ../../typedefs.h aec/include/echo_cancellation.h aecm/include/echo_control_mobile.h agc/include/gain_control.h ns/include/noise_suppression.h ns/include/noise_suppression_x.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audio_processing.pc
