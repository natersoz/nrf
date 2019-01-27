/**
 * @file segger_rtt.h
 * RTT version: 6.18a
 *
 * (c) 2014 - 2017  SEGGER Microcontroller GmbH & Co. KG
 * This is a derivative work and subject to the restrictions and copyright
 * claims found at the end of the file.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t rtt_channel_t;

/**
 * @struct rtt_channel_alloc
 * Allows for the RTT buffer allocation by the RTT client and passed into the
 * RTT module.
 */
struct rtt_channel_alloc
{
    enum { up, down }   direction;
    rtt_channel_t       channel;
    void*               buffer;
    size_t              buffer_size;
};

/**
 * Provide buffer allocations for each RTT channel which is intended to be
 * used by the application.
 * There is a maximum number of channels that can be open.
 * This is statically set by the internal module variable channel_count_max.
 *
 * @note RTT allocations must be made prior to calling segger_rtt_init() and
 *       not performed after segger_rtt_init() has been called.
 *
 * @note The channel_alloc buffers must have lifetimes for the duration of
 *       RTT usage; most likely forever in an embedded system.
 *       The struct itself is merely used to pass in the buffers and their
 *       usage/role. The struct can be reused and/or discarded after use.
 *
 * @todo It is not known whether the channels allocated need to contiguous.
 *       I speculate that they do not need to be.
 *
 * @param channel_alloc The channel allocation block.
 */
void segger_rtt_channel_allocate(struct rtt_channel_alloc const* channel_alloc);

/**
 * Once all RTT channels have been allocated the RTT can be enabled.
 */
void segger_rtt_enable(void);

/**
 * Write a specified number of characters into the SEGGER Real Time Terminal
 * (RTT) 'up' buffer.
 * Up buffers are used to transfer data from the device to the host computer
 * running the Segger RTT Client software attached via SWID/JTAG.
 *
 * @param channel The RTT channel of "Up"-buffer to be used
 *                  0: "Terminal"
 *                  1: "SysView"
 *                  2: "J-Scope_t4i4"
 * @note The channel must be within the allocated RTT_BUFFER_UP_COUNT value.
 * @param buffer        Pointer to the user supplied data to write to the host.
 * @param buffer_length The number of bytes to write to the host.
 *
 * @return The number of bytes written into the ring buffer.
 *         This value is the minimum of the number of bytes available in the
 *         ring buffer for writing and the write request length.
 */
size_t SEGGER_RTT_Write(rtt_channel_t   channel,
                        void const*     buffer,
                        size_t          buffer_length);

size_t SEGGER_RTT_PutChar(rtt_channel_t channel, char value);

size_t SEGGER_RTT_WritePending(rtt_channel_t);
size_t SEGGER_RTT_WriteAvailable(rtt_channel_t);

/**
 * Read a specified number of characters from the SEGGER Real Time Terminal
 * (RTT) 'down' buffer.
 * Down buffers are used to transfer data from the host computer the device.
 *
 * @param channel  The channel of "Up"-buffer to be used
 *                  0: "Terminal"
 *                  1: "SysView"
 *                  2: "J-Scope_t4i4"
 * @note The channel must be within the allocated RTT_BUFFER_UP_COUNT value.
 *
 * @param [out] buffer  Pointer to the user supplied buffer to to the host.
 * @param buffer_length The number of bytes to write to the host.
 *
 * @return The number of bytes read from the ring buffer.
 *         This value is the minimum of the number of bytes available in the
 *         ring buffer for reading and the buffer length.
 */
size_t SEGGER_RTT_Read(rtt_channel_t    channel,
                       void*            buffer,
                       size_t           buffer_length);

size_t SEGGER_RTT_ReadPending(rtt_channel_t channel);
size_t SEGGER_RTT_ReadAvailable(rtt_channel_t channel);

int SEGGER_RTT_GetChar(rtt_channel_t channel);

#ifdef __cplusplus
}
#endif

/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER RTT * Real Time Transfer for embedded targets         *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* conditions are met:                                                *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this list of conditions and the following disclaimer.    *
*                                                                    *
* o Redistributions in binary form must reproduce the above          *
*   copyright notice, this list of conditions and the following      *
*   disclaimer in the documentation and/or other materials provided  *
*   with the distribution.                                           *
*                                                                    *
* o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
*   nor the names of its contributors may be used to endorse or      *
*   promote products derived from this software without specific     *
*   prior written permission.                                        *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************/

