#
# Copyright (C) 2010-2014 Mamadou DIOP
# Copyright (C) 2011-2014 Doubango Telecom <http://www.doubango.org>
# License: GPLv3 or commercial
# This file is part of Doubango VoIP framework <https://code.google.com/p/doubango/>
#

AC_PREREQ([2.0])
AC_INIT(libwebrtc-audioproc, 2.0.0, doubango(at)googlegroups(dot)com)
AC_CANONICAL_SYSTEM
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([system_wrappers/source/aligned_malloc.cc])

### HOST FOR CROSS-COMPILING
target_os=`uname -s`
target_cpu=unknown

AM_CONDITIONAL([UNDER_ARM], false)
AM_CONDITIONAL([UNDER_X86], false)
AM_CONDITIONAL([UNDER_ARM_V7], false)
AM_CONDITIONAL([UNDER_ARM_NEON], false)

case "$host" in
  arm-*-linux-androideabi)
    target_os=android
    target_cpu=armv5te
    AM_CONDITIONAL([UNDER_ARM], true)
    ;;
   i686-*-linux-android)
    target_os=android
    target_cpu=x86
    AM_CONDITIONAL([UNDER_X86], true)
    ;;
   x86_64*)
     target_cpu=x86_64
     AM_CONDITIONAL([UNDER_X86], true)
    ;;
   i386*)
     target_cpu=i386
     AM_CONDITIONAL([UNDER_X86], true)
    ;;
  *)
    AC_MSG_NOTICE(["Using host as target os: $host"])
    ;;
esac

case $target_os in
	mingw*)
		target_os=windows
	;;
	darwin*)
		target_os=osx
	;;
	android*)
		
	;;
	*)
		target_os=linux
	;;
esac

AM_CONDITIONAL([UNDER_WINDOWS], [test $target_os = windows])
AM_CONDITIONAL([UNDER_OSX], [test $target_os = osx])
AM_CONDITIONAL([UNDER_ANDROID], [test $target_os = android])
AM_CONDITIONAL([UNDER_LINUX], [test $target_os = linux])

### ANDROID-CPU
AC_ARG_WITH(android-cpu, 
[  --with-android-cpu[=cpu] set android CPU type [[default=armv5te]]
                       armv5te: ARMv5TE
                       armv7-a: ARMv7-a *without* NEON
                       armv7-a-neon: ARMv7-a *with* NEON
                       x86: X86],
[ target_cpu=$withval ])

### ANDROID-TOOLCHAIN (http://www.kandroid.org/ndk/docs/STANDALONE-TOOLCHAIN.html)
if test "$target_os" = "android"; then
	case "$target_cpu" in
		x86)
			export SYSROOT=$NDK/platforms/android-9/arch-x86
    			export ANDROID_PREFIX=i686-linux-android
			export ANDROID_TOOLCHAIN=$(pwd)/my-android-toolchain-android-x86
    			$NDK/build/tools/make-standalone-toolchain.sh --platform=android-9 --arch=x86 --install-dir=$ANDROID_TOOLCHAIN
			AM_CONDITIONAL([UNDER_X86], true)
		;;
		armv7-a)
			export ANDROID_PREFIX=arm-linux-androideabi
			export SYSROOT=$NDK/platforms/android-3/arch-arm
			export ANDROID_TOOLCHAIN=$(pwd)/my-android-toolchain-android-armv7-a
    			$NDK/build/tools/make-standalone-toolchain.sh --platform=android-3 --install-dir=$ANDROID_TOOLCHAIN
    			export CFLAGS='-march=armv7-a -mfloat-abi=softfp -D__ARMEL__'
    			export LDFLAGS='-Wl,--fix-cortex-a8'
			AM_CONDITIONAL([UNDER_ARM], true)
			AM_CONDITIONAL([UNDER_ARM_V7], true)
		;;
		armv7-a-neon)
			export ANDROID_PREFIX=arm-linux-androideabi
			export SYSROOT=$NDK/platforms/android-3/arch-arm
    			export ANDROID_TOOLCHAIN=$(pwd)/my-android-toolchain-android-armv7-a-neon
    			$NDK/build/tools/make-standalone-toolchain.sh --platform=android-3 --install-dir=$ANDROID_TOOLCHAIN
    			export CFLAGS='-march=armv7-a -mfloat-abi=softfp -mfpu=neon -D__ARMEL__'
    			export LDFLAGS='-Wl,--fix-cortex-a8'
			AM_CONDITIONAL([UNDER_ARM], true)
			AM_CONDITIONAL([UNDER_ARM_V7], true)
			AM_CONDITIONAL([UNDER_ARM_NEON], true)
		;;
		*)
			export ANDROID_PREFIX=arm-linux-androideabi
			export SYSROOT=$NDK/platforms/android-3/arch-arm
			export ANDROID_TOOLCHAIN=$(pwd)/my-android-toolchain-armv5te
    			$NDK/build/tools/make-standalone-toolchain.sh --platform=android-3 --install-dir=$ANDROID_TOOLCHAIN
			export CFLAGS='-D__ARMEL__'
			AM_CONDITIONAL([UNDER_ARM], true)
		;;
	esac

	AC_SUBST(NDK, $NDK)
	
	export CFLAGS="${CFLAGS} -DANDROID=1 -DWEBRTC_ANDROID=1 -fPIC"
	export CPPFLAGS=$CFLAGS
	export LDFLAGS="${LDFLAGS} -lc -lm -lgcc"

	export PATH=$ANDROID_TOOLCHAIN/bin:$PATH

	export CC=$ANDROID_TOOLCHAIN/bin/$ANDROID_PREFIX-gcc
	export AR=$ANDROID_TOOLCHAIN/bin/$ANDROID_PREFIX-ar
	export CXX=$ANDROID_TOOLCHAIN/bin/$ANDROID_PREFIX-g++
	export AS=$ANDROID_TOOLCHAIN/bin/$ANDROID_PREFIX-as
	export RANLIB=$ANDROID_TOOLCHAIN/bin/$ANDROID_PREFIX-ranlib
	export STRIP=$ANDROID_TOOLCHAIN/bin/$ANDROID_PREFIX-strip
fi

dnl find suitable C and C++ compilers
AM_PROG_AS
AC_PROG_CC
AC_PROG_CXX

AC_PROG_INSTALL

AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL

AC_PROG_SED

AC_PROG_MAKE_SET

PKG_PROG_PKG_CONFIG

### CROSS COMPILATION?
AM_CONDITIONAL([CROSS_COMPILING], [test "x$cross_compiling" = xyes])

### ENABLE-DEBUG
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug, 
[  --enable-debug[=no/yes] turn on/off debugging 
                       [[default=$debug_default]]],
[],
[])
test "x$enable_debug" = "x" && enable_debug=no

AM_CONDITIONAL(USE_DEBUG, test x"$enable_debug" = x"yes")
if test "$enable_debug" = "yes"; then
	CFLAGS="${CFLAGS} -O0 -g3 -DDEBUG"
	CPPFLAGS=$CFLAGS
	CXXFLAGS=$CFLAGS
else
	if test "$target_os" = "android"; then
		CFLAGS="${CFLAGS} -Os -s"
		
	else
		CFLAGS="${CFLAGS} -O3 -DNDEBUG"
	fi
	CPPFLAGS=$CFLAGS
	CXXFLAGS=$CFLAGS
fi


### CFLAGS && LDFLAGS
if test "$target_os" = "android"; then
	home_dir=`pwd`
	#CFLAGS="${CFLAGS} -I${home_dir}/thirdparties/android/${target_cpu}/include -I${home_dir}/thirdparties/android/common/include -I${home_dir}/thirdparties/common/include/libyuv"
	#CPPFLAGS=$CFLAGS
	#LDFLAGS="${LDFLAGS} -L${home_dir}/thirdparties/android/${target_cpu}/lib/dist"
	#if test "$enable_gpl" = yes; then
	#	LDFLAGS="${LDFLAGS} -L${home_dir}/thirdparties/android/${target_cpu}/lib/dist/gpl"
	#else
	#	LDFLAGS="${LDFLAGS} -L${home_dir}/thirdparties/android/${target_cpu}/lib/dist/lgpl"
	#fi
	AC_SUBST(CFLAGS)
	AC_SUBST(LDFLAGS)
fi

AC_CONFIG_FILES(
Makefile
webrtc-audioproc.pc

system_wrappers/Makefile
system_wrappers/system_wrappers.pc

common_audio/Makefile
common_audio/common_audio.pc

modules/Makefile

modules/audio_processing/Makefile
modules/audio_processing/audio_processing.pc

)

AC_OUTPUT

AC_MSG_NOTICE([
 
**************************************************************************
* 				CONGRATULATIONS
**************************************************************************
Cross Compilation:   ${cross_compiling}
Target OS:           ${target_os}
Host setup:          ${host}
Host CPU:            ${host_cpu}
Install prefix:      ${prefix}
Compiler:            ${CC}

DEBUG:               $enable_debug

Report any issue at https://groups.google.com/group/doubango

])

if test "$target_os" = "android"; then
AC_MSG_NOTICE([

--------------------------------------------------------------------------
- 				    ANDRDOID
--------------------------------------------------------------------------
CPU:                 ${target_cpu}
])
fi
