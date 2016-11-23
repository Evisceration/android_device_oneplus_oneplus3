/*
 * Copyright (C) 2016 Alexander Martinz
 * Copyright (C) 2016 The NamelessRom Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "VrHAL"

#include <cutils/log.h>
#include <cutils/properties.h>
#include <string.h>

#include <hardware/hardware.h>
#include <hardware/vr.h>

#include "utils.h"
#include "vr-common.h"

static const char* PROP_PROJECT_NAME = "ro.boot.project_name";
static const char* PROJECT_NAME_T    = "15811";

// OnePlus3 has two inflight numbers. By default, inflight=15 and inflight_low_latency=4.
// Inflight is only used when there is a single GL context, when there is more than one
// context, inflight_low_latency is used. Since we are only interested in affecting
// performance when there is context preemption, we only have to modify the low latency
// parameter.
#define GPU_INFLIGHT_PATH "/sys/class/kgsl/kgsl-3d0/dispatch/inflight_low_latency"

#define DEVFREQ_BUS_SPEED_PATH "/sys/class/devfreq/soc:qcom,gpubw/min_freq"

static struct SysfsTunable sysfsTunables[] = {
    { .path = GPU_INFLIGHT_PATH, .value = 4, .value_vr = 2 },
    { .path = DEVFREQ_BUS_SPEED_PATH, .value = 0, .value_vr = 7904 },
};

// Set global display/GPU/scheduler configuration to used for VR apps.
static void set_vr_performance_configuration() {
    int length = sizeof(sysfsTunables) / sizeof(sysfsTunables[0]);

    for (int i = 0; i < length; i++) {
        write_int(sysfsTunables[i].value_vr, sysfsTunables[i].path);
    }
}

// Reset to default global display/GPU/scheduler configuration.
static void unset_vr_performance_configuration() {
    int length = sizeof(sysfsTunables) / sizeof(sysfsTunables[0]);

    for (int i = 0; i < length; i++) {
        write_int(sysfsTunables[i].value, sysfsTunables[i].path);
    }
}

static void vr_init(struct vr_module *module __unused) {
    char prj_name[PROP_VALUE_MAX];

    ALOGI("%s: Hey ho, let's go!", __FUNCTION__);

    property_get(PROP_PROJECT_NAME, prj_name, NULL);
    if (!strcmp(prj_name, PROJECT_NAME_T)) {
        ALOGV("Setting up for OP3T");
    } else {
        ALOGV("Setting up for OP3");
    }
}

static void vr_set_vr_mode(struct vr_module *module __unused, bool enabled) {
    ALOGV("%s: %s", __FUNCTION__, enabled ? "enabled" : "disabled");

    if (enabled) {
        set_vr_performance_configuration();
    } else {
        unset_vr_performance_configuration();
    }
}

static struct hw_module_methods_t vr_module_methods = {
    .open = NULL, // There are no devices for this HAL interface.
};


vr_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag                = HARDWARE_MODULE_TAG,
        .module_api_version = VR_MODULE_API_VERSION_1_0,
        .hal_api_version    = HARDWARE_HAL_API_VERSION,
        .id                 = VR_HARDWARE_MODULE_ID,
        .name               = "OnePlus3 VR HAL",
        .author             = "Alexander Martinz / The NamelessRom Project",
        .methods            = &vr_module_methods,
    },

    .init = vr_init,
    .set_vr_mode = vr_set_vr_mode,
};
