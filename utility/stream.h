/**
 * @file stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A character driver-like set of classes.
 */

#pragma once

#include <cstddef>

namespace io
{

/**
 * @interface input_stream
 * An abstract class for receiving data from a device.
 */
class input_stream
{
public:
    virtual ~input_stream()                       = default;

    input_stream()                                = default;
    input_stream(input_stream const&)             = delete;
    input_stream(input_stream&&)                  = default;
    input_stream& operator=(input_stream const&)  = delete;
    input_stream& operator=(input_stream&&)       = default;

    /**
     * Read data from the stream into a the user supplied buffer.
     *
     * @param buffer The user supplied buffer to read data into.
     * @param length The length of the user supplied buffer in bytes.
     *
     * @return std::size_t The actual number of bytes read into the buffer.
     */
    virtual std::size_t read(void *buffer, std::size_t length) = 0;

    /**
     * @return std::size_t The number of bytes which have been received on the
     *                     stream and may be ready to read.
     * @note A call to the fill() method may be required prior to reading
     *       any or all of the bytes into a user buffer.
     */
    virtual std::size_t read_pending() const = 0;

    /**
     * @return std::size_t The number of bytes remaining in the stream before
     *                     the internal buffer overflows and loses data.
     */
    virtual std::size_t read_avail() const = 0;

    /**
     * Force the stream to acquire all data in transit into its internal buffer
     * so that a read() operation can be performed and obtain all received data.
     * This call blocks until the fill operation is complete.
     */
    virtual void fill() = 0;
};

/**
 * @interface output_stream
 * An abstract class for sending data to a device.
 */
class output_stream
{
public:
    virtual ~output_stream()                        = default;

    output_stream()                                 = default;
    output_stream(output_stream const&)             = delete;
    output_stream(output_stream&&)                  = default;
    output_stream& operator=(output_stream const&)  = delete;
    output_stream& operator=(output_stream&&)       = default;

    /**
     * Write data to a stream from the user supplied buffer.
     *
     * @param buffer The user supplied buffer to write data from.
     * @param length The length of the user supplied buffer in bytes.
     *
     * @return std::size_t The actual number of bytes written into the stream.
     */
    virtual std::size_t write(void const *buffer, std::size_t length) = 0;

    /**
     * @return std::size_t The number of bytes written to the stream but remain
     *                     in the stream's internal buffer waiting to be sent.
     */
    virtual std::size_t write_pending() const = 0;

    /**
     * @return std::size_t The number of bytes which may be written into the
     *                     stream before the internal buffer becomes full.
     */
    virtual std::size_t write_avail() const = 0;

    /**
     * Force the data stored in the stream's internal buffer to be written to
     * the sink device or end point.
     * This call blocks until the fill operation is complete.
     */
    virtual void flush() = 0;
};

}  // namespace io
