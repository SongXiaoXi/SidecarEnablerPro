//
//  kern_start.cpp
//  NSE
//
//  Copyright © 2020 SXX. All rights reserved.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#include "kern_sep.hpp"

static com_sxx_SEP sep;

static const char *bootargOff[] {
    "-sidecaroffpro"
};

static const char *bootargDebug[] {
    "-sidecardbgpro"
};

static const char *bootargBeta[] {
    "-sidecarbetapro"
};

PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal,
    bootargOff,
    1,
    bootargDebug,
    1,
    bootargBeta,
    1,
    KernelVersion::Catalina,
    KernelVersion::BigSur,
    []() {
       sep.init();
    }
};
