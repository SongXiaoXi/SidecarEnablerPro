//
//  kern_sep.cpp
//  SEP
//
//  Copyright © 2020 SXX. All rights reserved.
//

#include <Headers/kern_api.hpp>
#include <Headers/kern_file.hpp>
#include <Headers/kern_mach.hpp>
#include <Headers/kern_policy.hpp>
#include <sys/vnode.h>

static uint8_t VideoProcessing_find[] = {
    'M',0,'a',0,'c',0,'-',0,'F',0,'6',0,'0',0,'D',0,'E',0,'B',0,'8',0,'1',0,'F',0,'F',0,'3',0,'0',0,'A',0,'C',0,'F',0,'6',0, // Mac Pro 6,1
    0, 0, 0, 0,
    'M',0,'a',0,'c',0,'-',0,'7',0,'B',0,'A',0,'5',0,'B',0,'2',0,'D',0,'9',0,'E',0,'4',0,'2',0,'D',0,'D',0,'D',0,'9',0,'4',0, // iMac Pro 1,1
    0, 0, 0, 0,
    'M',0,'a',0,'c',0,'-',0,'2',0,'7',0,'A',0,'D',0,'2',0,'F',0,'9',0,'1',0,'8',0,'A',0,'E',0,'6',0,'8',0,'F',0,'6',0,'1',0, // Mac Pro 7,1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    'M',0,'a',0,'c',0,'-',0,'2',0,'7',0,'A',0,'D',0,'2',0,'F',0,'9',0,'1',0,'8',0,'A',0,'E',0,'6',0,'8',0,'F',0,'6',0,'1',0, // Mac Pro 7,1
};

static uint8_t VideoProcessing_replace[] = {
    'M',0,'a',0,'c',0,'-',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,
    0, 0, 0, 0,
    'M',0,'a',0,'c',0,'-',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,
    0, 0, 0, 0,
    'M',0,'a',0,'c',0,'-',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    'M',0,'a',0,'c',0,'-',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,
};

static_assert(sizeof(VideoProcessing_find) == sizeof(VideoProcessing_replace), "size mismatched");

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

static uint8_t AppleGVA_find[] = "Mac-7BA5B2D9E42DDD94\0Mac-F60DEB81FF30ACF6\0Mac-27AD2F918AE68F61";
static uint8_t AppleGVA_replace[] = "Mac-0000000000000000\0Mac-0000000000000000\0Mac-0000000000000000";

#define MODULE_SHORT "fu_fix"

static mach_vm_address_t orig_cs_validate {};

#pragma mark - Kernel patching code

template <size_t findSize, size_t replaceSize>
static inline bool searchAndPatch(const void *haystack, size_t haystackSize, const char *path, const uint8_t (&needle)[findSize], const uint8_t (&patch)[replaceSize], const char *name) {
    if (UNLIKELY(KernelPatcher::findAndReplace(const_cast<void *>(haystack), haystackSize, needle, findSize, patch, replaceSize))) {
        DBGLOG(MODULE_SHORT, "found function %s to patch at %s!", name, path);
        return true;
    }
    return false;
}

#pragma mark - Patched functions

// For Big Sur and newer
static void patched_cs_validate_page(vnode_t vp, memory_object_t pager, memory_object_offset_t page_offset, const void *data, int *validated_p, int *tainted_p, int *nx_p) {
    char path[PATH_MAX];
    int pathlen = PATH_MAX;
    FunctionCast(patched_cs_validate_page, orig_cs_validate)(vp, pager, page_offset, data, validated_p, tainted_p, nx_p);
    // We check the number of times we've patched to limit wasted loops
    // Because AirPlay/Sidecar patches are long in length, repetitively calling the function is expensive
    // Since we know how many times these strings will appear in the dyld, we can effectively avoid extra checks
    // This resolves issues of system stability when a system is slower/low on memory as well as improve battery life
    if (vn_getpath(vp, path, &pathlen) == 0) {
        // dyld cache patching
        if (UserPatcher::matchSharedCachePath(path)) {
            bool ret = searchAndPatch(data, PAGE_SIZE, path, VideoProcessing_find, VideoProcessing_replace, "VideoProcessing");
            if (ret) {
                return;
            }
            searchAndPatch(data, PAGE_SIZE, path, AppleGVA_find, AppleGVA_replace, "AppleGVA");
        }
    }
}

#pragma mark - Patches on start/stop

static void pluginStart() {
    DBGLOG(MODULE_SHORT, "start");
    
    lilu.onPatcherLoadForce([](void *user, KernelPatcher &patcher) {
        KernelPatcher::RouteRequest csRoute = KernelPatcher::RouteRequest("_cs_validate_page", patched_cs_validate_page, orig_cs_validate);
        if (!patcher.routeMultipleLong(KernelPatcher::KernelID, &csRoute, 1))
            SYSLOG(MODULE_SHORT, "failed to route cs validation pages");
    });
}

// Boot args.
static const char *bootargOff[] {
    "-sidecaroffpro"
};
static const char *bootargDebug[] {
    "-sidecardbgpro"
};
static const char *bootargBeta[] {
    "-sidecarbetapro"
};

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

// Plugin configuration.
PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal,
    bootargOff,
    arrsize(bootargOff),
    bootargDebug,
    arrsize(bootargDebug),
    bootargBeta,
    arrsize(bootargBeta),
    KernelVersion::Catalina,
    KernelVersion::Ventura,
    pluginStart
};
