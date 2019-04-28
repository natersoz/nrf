/**
 * @file segger_rtt.cc
 * RTT version: 6.18a
 *
 * (c) 2014 - 2017  SEGGER Microcontroller GmbH & Co. KG
 * This is a derivative work and subject to the restrictions and copyright
 * claims found at the end of the file.
 */

#include "segger_rtt.h"

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <type_traits>

#include "nordic_critical_section.h"
#include "project_assert.h"

static constexpr rtt_channel_t const channel_count_max = 4u;

static char const* const channel_name [channel_count_max] = {
    "Terminal",
    "SysView",
    "J-Scope_t4i4",
    "Aux"
};

/**
 * @enum rtt_mode
 * Only RTT_MODE_NO_BLOCK_TRIM is supported to reduce complexity.
 * When writing the buffer the number of bytes written is returned and it is
 * up to the client to determine if the writing of the unwritten bytes needs
 * to be performed. Or more convenient: increase the output (UP) buffer size.
 * These flags are here for RTT buffer compliance.
 */
enum rtt_mode
{
    /// Skip. Do not block, output nothing. (Default)
    RTT_MODE_NO_BLOCK_SKIP          = 0u,
    /// Trim: Do not block, output as much as fits.
    RTT_MODE_NO_BLOCK_TRIM          = 1u,

    /// Block: Wait until there is space in the buffer.
    RTT_MODE_BLOCK_IF_FIFO_FULL     = 2u,

    /// Bits allocation within the rtt_buffer_up_t::flags   for mode.
    /// Bits allocation within the rtt_buffer_down_t::flags for mode.
    RTT_MODE_MASK                   = 3u
};

using signed_size_t = typename std::make_signed<size_t>::type;

/// Using size_t and uint32_t within this implementation.
/// SEGGER uses native types.
/// This insures the buffer implememtation will work with the RTT host.
/// size_t types don't have to check here to work properly,
/// but I want to know if they do not.
#if defined __arm__
static_assert(sizeof(unsigned int) == sizeof(uint32_t));
static_assert(sizeof(unsigned int) == sizeof(size_t));
static_assert(sizeof(unsigned int) == sizeof(signed_size_t));
#endif // __arm__

/// Circular buffer which is used as the up-buffer; target to host.
extern "C" struct rtt_buffer_up_t
{
    /// Optional name. Gets set when the channel is allocated.
    char const *name;

    /// Pointer to start of buffer
    char *base_pointer;

    /// Buffer size in bytes.
    /// @note The actual capacity is length - 1. When the write_offset equals
    /// read_offset the buffer is considered empty, not full.
    uint32_t length;

    /// The position to write the next character into the buffer.
    uint32_t write_offset;

    /// The position to read the next character from the buffer.
    uint32_t volatile read_offset;

    /// This implementation only sets the flags to RTT_MODE_NO_BLOCK_TRIM.
    uint32_t flags;
};

/// Circular buffer which is used as the down-buffer; host to target.
///
/// @note This is exactly the same layout as the struct rtt_buffer_up_t
/// except that the volatile qualifier is on the write_offset data member
/// rather than the read_offset. It is these volatile member value which are
/// modified by the Segger host software.
extern "C" struct rtt_buffer_down_t
{
    /// Optional name.
    /// Standard names so far are: "Terminal", "SysView", "J-Scope_t4i4"
    char const*         name;
    char*               base_pointer;
    uint32_t            length;
    uint32_t volatile   write_offset;
    uint32_t            read_offset;
    uint32_t            flags;
};

// Only perform these checks when compiling for ARM.
#if defined __arm__
static_assert(sizeof(struct rtt_buffer_down_t) == sizeof(uint32_t) * 6u);
static_assert(sizeof(struct rtt_buffer_up_t)   == sizeof(uint32_t) * 6u);
#endif

/**
 * RTT control block which describes the number of buffers available
 * as well as the configuration for each buffer.
 *
 * @note The buffer layouts are cast in stone and must be in the formwat
 * determined by Segger. The member signature, followed by the data
 * layout of this struct
 *
 * follows is assumed by the host software. Changing the struct layouts
 * results in the Segger host software to understand the buffer structs
 * and contents.
 */
extern "C" struct rtt_control_block_t
{
    /// Must be Initialized to "SEGGER RTT". This is the signature used by the
    /// Segger RTT client host software to determine where the buffer layouts
    /// are held. This must be 16 bytes in length and carry 4 byte alignment.
    char signature[16u];

    /// Must be initialized to RTT_BUFFER_UP_COUNT.
    uint32_t rtt_buffer_up_count;

    /// Must be Initialized to RTT_BUFFER_DOWN_COUNT.
    uint32_t rtt_buffer_down_count;

    /// Up buffers, transferring information up from target to host
    struct rtt_buffer_up_t buffer_up[channel_count_max];

    /// Down buffers, transferring information down from host to target
    struct rtt_buffer_down_t buffer_down[channel_count_max];
};

// Setting the alignment since there is no actual guarantee that the signature
// will be algined.
static struct rtt_control_block_t rtt_control_block __attribute__((aligned(4)));

#if 0   /// @todo Will probably not use.
static unsigned char const terminal_id[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/**
 * @note The Segger RTT appears to allow for the switching of displayed
 * terminals. This seems to be done by sending the sequence
 * {0xFF, terminal_id} into the active write ring buffer.
 *
 */
static char rtt_terminal_active;
#endif

void segger_rtt_channel_allocate(struct rtt_channel_alloc const* channel_alloc)
{
    // Channel allocations must occur before calling segger_rtt_init().
    ASSERT(rtt_control_block.signature[0] == 0);
    ASSERT(channel_alloc);

    if (channel_alloc->direction == rtt_channel_alloc::up)
    {
        ASSERT(channel_alloc->channel < channel_count_max);
        rtt_channel_t const channel = channel_alloc->channel;
        struct rtt_buffer_up_t *buffer = &rtt_control_block.buffer_up[channel];

        // Fault if the channel is already allocated.
        ASSERT(buffer->base_pointer == nullptr);

        buffer->name            = channel_name[channel];
        buffer->base_pointer    = reinterpret_cast<char*>(channel_alloc->buffer);
        buffer->length          = channel_alloc->buffer_size;
        buffer->read_offset     = 0u;
        buffer->write_offset    = 0u;
        buffer->flags           = RTT_MODE_NO_BLOCK_TRIM;

        rtt_control_block.rtt_buffer_up_count += 1u;
    }
    else if (channel_alloc->direction == rtt_channel_alloc::down)
    {
        ASSERT(channel_alloc->channel < channel_count_max);
        rtt_channel_t const channel = channel_alloc->channel;
        struct rtt_buffer_down_t *buffer = &rtt_control_block.buffer_down[channel];

        // Fault if the channel is already allocated.
        ASSERT(buffer->base_pointer == nullptr);

        buffer->name            = channel_name[channel];
        buffer->base_pointer    = reinterpret_cast<char*>(channel_alloc->buffer);
        buffer->length          = channel_alloc->buffer_size;
        buffer->read_offset     = 0u;
        buffer->write_offset    = 0u;
        buffer->flags           = RTT_MODE_NO_BLOCK_TRIM;

        rtt_control_block.rtt_buffer_down_count += 1u;
    }
    else
    {
        // Channels must be either up or down.
        ASSERT(0);
    }
}

/**
 * Finalize the control block ionitialization which will tell the RTT host to
 * start consuming data.
 */
void segger_rtt_enable(void)
{
    if (rtt_control_block.signature[0] != 0)
    {
        return;             // Already initialized.
    }

    if ((rtt_control_block.rtt_buffer_down_count == 0u) &&
        (rtt_control_block.rtt_buffer_up_count == 0u))
    {
        // Nothing allocated.
        ASSERT(0);
    }

    std::fill(std::begin(rtt_control_block.signature),
              std::end(rtt_control_block.signature),
              0);

    // Finish initialization of the control block.
    // Copy Id string in three steps to make sure "SEGGER RTT" is not found
    // in initializer memory (usually flash) by J-Link.
    // Once this signaure is set the host will start looking for data to consume.
    constexpr char  const id_str_1[] = "SEGGER";
    constexpr char  const id_str_2[] = "RTT";
    constexpr char  const id_sep     = ' ';
    constexpr char* const str_1_loc  = std::begin(rtt_control_block.signature);
    constexpr char* const str_2_loc  = str_1_loc + std::size(id_str_1);
    constexpr char* const sep_loc    = str_2_loc - 1u;

    std::copy(std::begin(id_str_2), std::end(id_str_2), str_2_loc);
    std::copy(std::begin(id_str_1), std::end(id_str_1), str_1_loc);
    *sep_loc = id_sep;
}

static size_t rtt_write_avail(size_t read_offset, size_t write_offset, size_t length)
{
    signed_size_t const delta_offset = read_offset - write_offset;
    if (delta_offset > 0)
    {
        return delta_offset - 1u;
    }
    else
    {
        return delta_offset + length - 1u;
    }
}

static size_t rtt_read_avail(size_t write_offset, size_t read_offset, size_t length)
{
    signed_size_t read_avail = write_offset - read_offset;

    read_avail += (read_avail <= 0) ? length : 0u;
    return read_avail;
}

/**
 * Write data from a user supplied buffer into the Segger RTT up ring buffer .
 *
 * @param rtt_ring_buffer The ring buffer to write to; device to host.
 * @param buffer          Pointer to the user supplied buffer.
 * @param buffer_length   The number of bytes in the user supplied data buffer.
 *
 * @return The number of bytes written into the ring buffer.
 *         This value is the minimum of the number of bytes available in the
 *         ring buffer for writing and the request buffer length.
 */
static size_t rtt_write(struct rtt_buffer_up_t* rtt_ring_buffer,
                        void const*             buffer,
                        size_t                  buffer_length)
{
    char   const* buffer_iter   = reinterpret_cast<char const *>(buffer);
    size_t        write_offset  = rtt_ring_buffer->write_offset;
    size_t const  read_offset   = rtt_ring_buffer->read_offset;
    size_t        write_avail   = rtt_write_avail(read_offset, write_offset,
                                                  rtt_ring_buffer->length);
    size_t const  write_linear  = rtt_ring_buffer->length - write_offset;
    size_t const  avail_linear  = std::min(write_avail,  write_linear);
    size_t const  buffer_linear = std::min(avail_linear, buffer_length);

    std::copy(buffer_iter,
              buffer_iter + buffer_linear,
              &rtt_ring_buffer->base_pointer[write_offset]);

    write_offset    += buffer_linear;
    buffer_iter     += buffer_linear;
    buffer_length   -= buffer_linear;
    write_avail     -= buffer_linear;

    size_t const write_remain = std::min(buffer_length, write_avail);

    if (write_remain > 0u)
    {
        // The first copy() wrote what could be fit in after write_offset
        // to the end of the allocated ring write buffer.
        // Fill what remains of the write request.
        std::copy(buffer_iter,
                  buffer_iter + write_remain,
                  rtt_ring_buffer->base_pointer);

        write_offset = write_remain;
        buffer_iter += write_remain;
    }
    else
    {
        // Handle the condition when 'buffer_length' matches the linear
        // available ring write buffer space.
        // Do not let write_offset dangle past the end of the ring buffer.
        // Note: '==' would be sufficient, '>=' is being defensive.
        if (write_offset >= rtt_ring_buffer->length)
        {
            write_offset = 0u;
        }
    }

    rtt_ring_buffer->write_offset = write_offset;
    return buffer_iter - reinterpret_cast<char const *>(buffer);
}

static size_t rtt_putc(struct rtt_buffer_up_t* rtt_ring_buffer, char value)
{
    size_t       write_offset = rtt_ring_buffer->write_offset;
    size_t const read_offset  = rtt_ring_buffer->read_offset;
    size_t const write_avail  = rtt_write_avail(read_offset, write_offset,
                                                rtt_ring_buffer->length);
    if (write_avail > 0u)
    {
        rtt_ring_buffer->base_pointer[write_offset] = value;
        write_offset += 1u;
        if (write_offset >= rtt_ring_buffer->length)
        {
            write_offset = 0u;
        }

        rtt_ring_buffer->write_offset = write_offset;
        return 1u;
    }

    return 0u;
}

/**
 * Read data from a Segger RTT 'down' ring buffer into a user supplied buffer.
 *
 * @param rtt_ring_buffer The down ring buffer to read from; host to device.
 * @param [out] buffer    Pointer to the user supplied buffer.
 * @param buffer_length   The user supplied data buffer capacity in bytes.
 *
 * @return The number of bytes read from the ring buffer.
 *         This value is the minimum of the number of bytes available for
 *         reading from the ring buffer and the user buffer length.
 */
static size_t rtt_read(struct rtt_buffer_down_t*    rtt_ring_buffer,
                       void*                        buffer,
                       size_t                       buffer_length)
{
    size_t        read_offset   = rtt_ring_buffer->read_offset;
    size_t const  write_offset  = rtt_ring_buffer->write_offset;
    char*         buffer_iter   = reinterpret_cast<char *>(buffer);
    size_t        read_avail    = rtt_read_avail(write_offset, read_offset, rtt_ring_buffer->length);
    size_t const  read_linear   = rtt_ring_buffer->length - read_offset;
    size_t const  avail_linear  = std::min(read_avail,   read_linear);
    size_t const  buffer_linear = std::min(avail_linear, buffer_length);

    std::copy(&rtt_ring_buffer->base_pointer[read_offset],
              &rtt_ring_buffer->base_pointer[read_offset] + buffer_linear,
              buffer_iter);

    read_offset     += buffer_linear;
    buffer_iter     += buffer_linear;
    buffer_length   -= buffer_linear;
    read_avail      -= buffer_linear;

    size_t const read_remain = std::min(buffer_length, read_avail);

    if (read_remain > 0u)
    {
        // The first copy() read what was available after read_offset
        // to the end of the allocated ring read buffer.
        // Fill what remains of the read request.
        std::copy(rtt_ring_buffer->base_pointer,
                  rtt_ring_buffer->base_pointer + read_remain,
                  buffer_iter);

        read_offset  = read_remain;
        buffer_iter += read_remain;
    }
    else
    {
        // Handle the condition when 'buffer_length' matches the linear
        // available read ring buffer space.
        // Do not let read_offset dangle past the end of the ring buffer.
        // Note: '==' would be sufficient, '>=' is being defensive.
        if (read_offset >= rtt_ring_buffer->length)
        {
            read_offset = 0u;
        }
    }

    rtt_ring_buffer->read_offset = read_offset;
    return buffer_iter - reinterpret_cast<char const *>(buffer);
}

static int rtt_getc(rtt_buffer_down_t* rtt_ring_buffer)
{
    size_t        read_offset   = rtt_ring_buffer->read_offset;
    size_t const  write_offset  = rtt_ring_buffer->write_offset;
    size_t const  read_avail    = rtt_read_avail(write_offset, read_offset,
                                                 rtt_ring_buffer->length);

    if (read_avail > 0u)
    {
        char const value = rtt_ring_buffer->base_pointer[read_offset];
        read_offset += 1u;
        if (read_offset >= rtt_ring_buffer->length)
        {
            read_offset = 0u;
        }

        rtt_ring_buffer->read_offset = read_offset;
        return value;
    }

    return -1;
}

size_t SEGGER_RTT_Write(rtt_channel_t   channel,
                        void const*     buffer,
                        size_t          buffer_length)
{
    nordic::auto_critical_section cs;
    return rtt_write(&rtt_control_block.buffer_up[channel],
                     buffer,
                     buffer_length);
}

size_t SEGGER_RTT_PutChar(rtt_channel_t channel, char value)
{
    nordic::auto_critical_section cs;
    return rtt_putc(&rtt_control_block.buffer_up[channel], value);
}

size_t SEGGER_RTT_WritePending(rtt_channel_t channel)
{
    nordic::auto_critical_section cs;

    signed_size_t const write_pending =
        (rtt_control_block.buffer_up[channel].length - 1u) -
        rtt_write_avail(rtt_control_block.buffer_up[channel].read_offset,
                        rtt_control_block.buffer_up[channel].write_offset,
                        rtt_control_block.buffer_up[channel].length);

    return (write_pending > 0) ? write_pending : 0u;
}

size_t SEGGER_RTT_WriteAvailable(rtt_channel_t channel)
{
    nordic::auto_critical_section cs;
    return rtt_write_avail(
        rtt_control_block.buffer_up[channel].read_offset,
        rtt_control_block.buffer_up[channel].write_offset,
        rtt_control_block.buffer_up[channel].length);
}

size_t SEGGER_RTT_Read(rtt_channel_t    channel,
                       void*            buffer,
                       size_t           buffer_length)
{
    nordic::auto_critical_section cs;
    return rtt_read(&rtt_control_block.buffer_down[channel],
                    buffer, buffer_length);
}

int SEGGER_RTT_GetChar(rtt_channel_t channel)
{
    nordic::auto_critical_section cs;
    return rtt_getc(&rtt_control_block.buffer_down[channel]);
}

size_t SEGGER_RTT_ReadPending(rtt_channel_t channel)
{
    nordic::auto_critical_section cs;
    signed_size_t const read_pending =
        (rtt_control_block.buffer_down[channel].length - 1u) -
        rtt_read_avail(rtt_control_block.buffer_down[channel].write_offset,
                       rtt_control_block.buffer_down[channel].read_offset,
                       rtt_control_block.buffer_down[channel].length);
    return (read_pending > 0)? read_pending : 0u;
}

size_t SEGGER_RTT_ReadAvailable(rtt_channel_t channel)
{
    nordic::auto_critical_section cs;
    return rtt_read_avail(rtt_control_block.buffer_down[channel].write_offset,
                          rtt_control_block.buffer_down[channel].read_offset,
                          rtt_control_block.buffer_down[channel].length);

}

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
