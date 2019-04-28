/**
 * @file rtt_output_stream.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "rtt_output_stream.h"
#include "segger_rtt.h"

// This stream always writes to the RTT "Terminal" output.
static constexpr rtt_channel_t const rtt_channel = 0u;

rtt_output_stream::~rtt_output_stream()
{
}

rtt_output_stream::rtt_output_stream(void* buffer, size_t buffer_size)
{
    rtt_channel_alloc const rtt_chn = {
        .direction   = rtt_channel_alloc::up,
        .channel     = 0u,
        .buffer      = buffer,
        .buffer_size = buffer_size
    };

    segger_rtt_channel_allocate(&rtt_chn);
}

size_t rtt_output_stream::write(void const *buffer, size_t length)
{
    return SEGGER_RTT_Write(rtt_channel, buffer, length);
}

size_t rtt_output_stream::write_pending() const
{
    return SEGGER_RTT_WritePending(rtt_channel);
}

size_t rtt_output_stream::write_avail() const
{
    return SEGGER_RTT_WriteAvailable(rtt_channel);
}

void rtt_output_stream::flush()
{
    while (this->write_pending() > 0)
    {
        /// @todo a timeout should be used here.
    }
}

