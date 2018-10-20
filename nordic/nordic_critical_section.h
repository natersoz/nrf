/**
 * Copied from sdk-14.2/components/libraries/util/app_util_platform.h
 *
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void app_util_critical_region_enter(uint8_t *p_nested);
void app_util_critical_region_exit(uint8_t nested);

/**
 * Macro for entering a critical region.
 *
 * @note Due to implementation details, there must exist one and only one call to
 *       CRITICAL_REGION_EXIT() for each call to CRITICAL_REGION_ENTER(), and they must be located
 *       in the same scope.
 */
#ifdef SOFTDEVICE_PRESENT
#define CRITICAL_REGION_ENTER() {                       \
        uint8_t __CR_NESTED = 0;                        \
        app_util_critical_region_enter(&__CR_NESTED);

#else
#define CRITICAL_REGION_ENTER() app_util_critical_region_enter(NULL)
#endif

/**
 * Macro for leaving a critical region.
 *
 * @note Due to implementation details, there must exist one and only one call to
 *       CRITICAL_REGION_EXIT() for each call to CRITICAL_REGION_ENTER(), and they must be located
 *       in the same scope.
 */
#ifdef SOFTDEVICE_PRESENT
#define CRITICAL_REGION_EXIT()                          \
        app_util_critical_region_exit(__CR_NESTED);     \
    }

#else
#define CRITICAL_REGION_EXIT() app_util_critical_region_exit(0)
#endif

#ifdef __cplusplus
}
#endif
