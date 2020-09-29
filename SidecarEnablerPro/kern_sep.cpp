//
//  kern_nse.cpp
//  NSE
//
//  Copyright © 2020 SXX. All rights reserved.
//

#include <Headers/kern_api.hpp>
#include <Headers/kern_file.hpp>
#include <Headers/kern_mach.hpp>
#include <sys/vnode.h>
#include "kern_sep.hpp"

static const char videoProcessing[] = "/System/Library/PrivateFrameworks/VideoProcessing.framework/Versions/A/VideoProcessing";

static const char appleGVA[] = "/System/Library/PrivateFrameworks/AppleGVA.framework/Versions/A/AppleGVA";
//static const char *avConference = "/System/Library/PrivateFrameworks/AVConference.framework/Versions/A/AVConference";

static const char avconferenced[] = "/usr/libexec/avconferenced";

static uint32_t avconferencedSize = sizeof(avconferenced) - 1;
static const char sidecarDisplayAgent[] = "/usr/libexec/SidecarDisplayAgent";
static uint32_t sidecarDisplayAgentSize = sizeof(sidecarDisplayAgent) - 1;

static uint8_t find1[] = {
    'M',0,'a',0,'c',0,'-',0,'7',0,'B',0,'A',0,'5',0,'B',0,'2',0,'D',0,'9',0,'E',0,'4',0,'2',0,'D',0,'D',0,'D',0,'9',0,'4',0
}; //iMacPro1,1
static uint8_t find2[] = {
    'M',0,'a',0,'c',0,'-',0,'2',0,'7',0,'A',0,'D',0,'2',0,'F',0,'9',0,'1',0,'8',0,'A',0,'E',0,'6',0,'8',0,'F',0,'6',0,'1',0
}; //MacPro7,1

static uint8_t replace[] = {'M',0,'a',0,'c',0,'-',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0};

static uint8_t find3[] = "Mac-7BA5B2D9E42DDD94"; //iMacPro1,1
static uint8_t find4[] = "Mac-27AD2F918AE68F61"; // MacPro7,1
static uint8_t replace2[] = "Mac-0000000000000000";

static UserPatcher::BinaryModPatch patches[] = {
    {
        CPU_TYPE_X86_64,
        0,
        find1,
        replace,
        arrsize(find1),
        0,
        1,
        UserPatcher::FileSegment::SegmentTextUstring,
        1
    },
    {
        CPU_TYPE_X86_64,
        0,
        find2,
        replace,
        arrsize(find2),
        0,
        1,
        UserPatcher::FileSegment::SegmentTextUstring,
        1
    }
};

static UserPatcher::BinaryModPatch patches2[] = {
    {
        CPU_TYPE_X86_64,
        0,
        find3,
        replace2,
        sizeof(find3),
        0,
        1,
        UserPatcher::FileSegment::SegmentTextCstring,
        1
    },
    {
        CPU_TYPE_X86_64,
        0,
        find4,
        replace2,
        sizeof(find4),
        0,
        1,
        UserPatcher::FileSegment::SegmentTextCstring,
        1
    },
};

static UserPatcher::BinaryModInfo modInfo[] {
    { videoProcessing, patches, sizeof(patches) / sizeof(patches[0]) },
    {appleGVA, patches2, sizeof(patches2) / sizeof(patches2[0])},
};

static UserPatcher::ProcInfo procInfo[] {
    { avconferenced, avconferencedSize, 1 },
    {sidecarDisplayAgent, sidecarDisplayAgentSize, 1}
};

void com_sxx_SEP::init() {
    lilu.onProcLoad(procInfo, 2, nullptr, nullptr, modInfo, 2);
}

void com_sxx_SEP::deinit() {
    
}
