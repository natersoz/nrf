/**
 * @file segger_rtt_output_stream.h
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

void segger_rtt_output_stream::flush()
{
    while (SEGGER_RTT_HasData(this->rtt_buffer_index_) > 0)
    {
        /// @todo a timeout should be used here.
    }
}

