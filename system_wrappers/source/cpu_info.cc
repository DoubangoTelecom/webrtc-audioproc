/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "cpu_info.h"

#include<stdio.h>
#if defined(_WIN32)
#include <Windows.h>
#elif defined(WEBRTC_MAC)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(WEBRTC_MAC_INTEL)
// Intentionally empty
#elif defined(WEBRTC_ANDROID)
// Not implemented yet, might be possible to use Linux implementation
#else // defined(WEBRTC_LINUX)
#include <sys/sysinfo.h>
#endif

#include "trace.h"

namespace webrtc {

WebRtc_UWord32 CpuInfo::_numberOfCores = 0;

#define DOUBANGO_DEBUG_INFO(FMT, ...) fprintf(stderr, "*WEBRTC_INFO: " FMT "\n", ##__VA_ARGS__);
#define DOUBANGO_DEBUG_ERROR(FMT, ...) fprintf(stderr, "***WEBRTC_ERROR: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__);
#define DOUBANGO_DEBUG_WARN(FMT, ...) fprintf(stderr, "**WARN: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__);

#define DOUBANGO_LOGS 1

WebRtc_UWord32 CpuInfo::DetectNumberOfCores()
{
    if (!_numberOfCores)
    {
#if defined(_WIN32)
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        _numberOfCores = static_cast<WebRtc_UWord32>(si.dwNumberOfProcessors);
#	if DOUBANGO_LOGS
		DOUBANGO_DEBUG_INFO("Available number of cores:%d", _numberOfCores);
#	else
        WEBRTC_TRACE(kTraceStateInfo, kTraceUtility, -1,
                     "Available number of cores:%d", _numberOfCores);
#	endif /* DOUBANGO_LOGS */

#elif defined(WEBRTC_LINUX) && !defined(WEBRTC_ANDROID)
        _numberOfCores = get_nprocs();
#	if DOUBANGO_LOGS
		DOUBANGO_DEBUG_INFO("Available number of cores:%d", _numberOfCores);
#	else
        WEBRTC_TRACE(kTraceStateInfo, kTraceUtility, -1,
                     "Available number of cores:%d", _numberOfCores);
#	endif /* DOUBANGO_LOGS */
#elif (defined(WEBRTC_MAC) || defined(WEBRTC_MAC_INTEL))
        int name[] = {CTL_HW, HW_AVAILCPU};
        int ncpu;
        size_t size = sizeof(ncpu);
        if(0 == sysctl(name, 2, &ncpu, &size, NULL, 0))
        {
            _numberOfCores = static_cast<WebRtc_UWord32>(ncpu);
#		if DOUBANGO_LOGS
			DOUBANGO_DEBUG_INFO("Available number of cores:%d", _numberOfCores);
#		else
			WEBRTC_TRACE(kTraceStateInfo, kTraceUtility, -1,
                     "Available number of cores:%d", _numberOfCores);
#		endif /* DOUBANGO_LOGS */
    } else
    {
#		if DOUBANGO_LOGS
			DOUBANGO_DEBUG_ERROR(Failed to get number of cores"");
#		else
			WEBRTC_TRACE(kTraceError, kTraceUtility, -1,
                         "Failed to get number of cores");
#		endif /* DOUBANGO_LOGS */
            
            _numberOfCores = 1;
    }
#else
#	if DOUBANGO_LOGS
		DOUBANGO_DEBUG_WARN("No function to get number of cores");
#	else
        WEBRTC_TRACE(kTraceWarning, kTraceUtility, -1,
                     "No function to get number of cores");
#	endif/* DOUBANGO_LOGS */
        _numberOfCores = 1;
#endif
    }
    return _numberOfCores;
}

} // namespace webrtc
