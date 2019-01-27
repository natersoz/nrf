/**
 * @file rtt_output_stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "stream.h"

class rtt_output_stream: public io::output_stream
{
public:
    using output_stream::output_stream;
    virtual ~rtt_output_stream() override;

    rtt_output_stream() = delete;
    rtt_output_stream(void* buffer, size_t buffer_size);

    virtual size_t write(void const *buffer, size_t length) override;
    virtual size_t write_pending() const                    override;
    virtual size_t write_avail()   const                    override;
    virtual void   flush()                                  override;
};

