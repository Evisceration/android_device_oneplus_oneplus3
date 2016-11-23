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

#define LOG_NDEBUG 0
#define LOG_TAG "VrHAL"

#include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "utils.h"

/**
 * Write 'len' characters from 'input' character array into file at path 'outFile.'
 *
 * Return 0 on success, or a negative error code.
 */
int write_string(const char* input, size_t len, const char* outFile) {
    int fd = -1;
    ssize_t err = 0;

    // Check input strings.
    if (input == NULL || outFile == NULL) {
        ALOGE("%s: Invalid input to write", __FUNCTION__);
        return -1;
    }

    // Open file, check for errors.
    fd = open(outFile, O_WRONLY);
    if (fd < 0) {
        ALOGE("%s: Failed to open file %s, error %s (%d)", __FUNCTION__, outFile, strerror(errno),
              -errno);
        return -errno;
    }

    // Write file, check for errors.
    err = write(fd, input, len);
    if (err < 0) {
        ALOGE("%s: Failed to write file %s, error %s (%d)", __FUNCTION__, outFile, strerror(errno),
              -errno);
        close(fd);
        return -errno;
    }

    // Close and return success.
    close(fd);
    return 0;
}

/**
 * Write integer 'input' formatted as a character string into the file at path 'outFile.'
 *
 * Return 0 on success, or a negative error code.
 */
int write_int(int input, const char* outFile) {
    char buffer[128] = {0,};
    int bytes = snprintf(buffer, sizeof(buffer), "%d", input);

    if (bytes < 0 || (size_t) bytes >= sizeof(buffer)) {
        ALOGE("%s: Failed to format integer %d", __FUNCTION__, input);
        return -EINVAL;
    }

    return write_string(buffer, (size_t) bytes, outFile);
}