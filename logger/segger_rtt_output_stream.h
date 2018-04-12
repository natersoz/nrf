/**
 * @file segger_rtt_output_stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "stream.h"

class segger_rtt_output_stream: public output_stream
{
public:
    virtual ~segger_rtt_output_stream() override;

    segger_rtt_output_stream(unsigned short int rtt_buffer_index = 0u)
    : rtt_buffer_index_(rtt_buffer_index)
    {}

    segger_rtt_output_stream(segger_rtt_output_stream const& other) = delete;
    segger_rtt_output_stream& operator=(segger_rtt_output_stream const& other) = delete;

    virtual size_t write(void const *buffer, size_t length) override;
    virtual size_t write_pending()                          override;
    virtual size_t write_avail()                            override;
    virtual void   flush()                                  override;

private:
    unsigned short int rtt_buffer_index_;
};

