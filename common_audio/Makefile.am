lib_LTLIBRARIES         = libcommon_audio.la
libcommon_audio_la_LIBADD = ../system_wrappers/libsystem_wrappers.la
libcommon_audio_la_CPPFLAGS = -I. -Iresampler/include -Isignal_processing/include -Ivad/include

libcommon_audio_la_SOURCES  = \
	resampler/resampler.cc
	
libcommon_audio_la_SOURCES +=        signal_processing/auto_corr_to_refl_coef.c\
       signal_processing/auto_correlation.c\
       signal_processing/complex_bit_reverse.c\
       signal_processing/complex_fft.c\
       signal_processing/copy_set_operations.c\
       signal_processing/cross_correlation.c\
       signal_processing/division_operations.c\
       signal_processing/dot_product_with_scale.c\
       signal_processing/downsample_fast.c\
       signal_processing/energy.c\
       signal_processing/filter_ar.c\
       signal_processing/filter_ar_fast_q12.c\
       signal_processing/filter_ma_fast_q12.c\
       signal_processing/get_hanning_window.c\
       signal_processing/get_scaling_square.c\
       signal_processing/ilbc_specific_functions.c\
       signal_processing/levinson_durbin.c\
       signal_processing/lpc_to_refl_coef.c\
       signal_processing/min_max_operations.c\
       signal_processing/randomization_functions.c\
       signal_processing/real_fft.c\
       signal_processing/refl_coef_to_lpc.c\
       signal_processing/resample.c\
       signal_processing/resample_48khz.c\
       signal_processing/resample_by_2.c\
       signal_processing/resample_by_2_internal.c\
       signal_processing/resample_fractional.c\
       signal_processing/spl_sqrt.c\
       signal_processing/spl_sqrt_floor.c\
       signal_processing/spl_version.c\
       signal_processing/splitting_filter.c\
       signal_processing/sqrt_of_one_minus_x_squared.c\
       signal_processing/vector_scaling_operations.c\
       signal_processing/webrtc_fft_t_1024_8.c\
       signal_processing/webrtc_fft_t_rad.c

if UNDER_X86
endif

if UNDER_ARM
libcommon_audio_la_SOURCES += signal_processing/spl_sqrt_floor_arm.s\
	signal_processing/complex_bit_reverse_arm.s
endif

if UNDER_ARM_V7
libcommon_audio_la_SOURCES +=signal_processing/filter_ar_fast_q12_armv7.s
endif

if UNDER_ARM_NEON
libcommon_audio_la_SOURCES +=signal_processing/vector_scaling_operations_neon.s\
	signal_processing/min_max_operations_neon.s\
	signal_processing/downsample_fast_neon.s\
	signal_processing/cross_correlation_neon.s
endif

libcommon_audio_la_SOURCES +=vad/vad_core.c\
	vad/vad_filterbank.c\
	vad/vad_gmm.c\
	vad/vad_sp.c\
	vad/webrtc_vad.c
	

libcommon_audio_la_LDFLAGS = $LDFLAGS -no-undefined
if UNDER_ANDROID
libcommon_audio_la_LDFLAGS += -static
endif

#resampler_includedir = $(includedir)/common_audio/resampler/include
#resampler_include_HEADERS = resampler/include/*.h
#vad_includedir = $(includedir)/common_audio/vad/include
#vad_include_HEADERS = vad/include/*.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = common_audio.pc
