/**
 * @file segger_rtt_output_stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "segger_rtt_output_stream.h"
#include "SEGGER_RTT.h"

segger_rtt_output_stream::~segger_rtt_output_stream()
{
    /// @todo release or close the SEGGER RTT buffer?
}

size_t segger_rtt_output_stream::write(void const *buffer, size_t length)
{
    return SEGGER_RTT_Write(this->rtt_buffer_index_, buffer, length);
}

size_t segger_rtt_output_stream::write_pending()
{
    return SEGGER_RTT_WritePending(this->rtt_buffer_index_);
}

size_t segger_rtt_output_stream::write_avail()
{
    return SEGGER_RTT_WritePending(this->rtt_buffer_index_);
}

void segger_rtt_output_stream::flush()
{
    while (this->write_pending() > 0)
    {
        /// @todo a timeout should be used here.
    }
}

