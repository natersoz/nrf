/**
 * @file fixed_allocator.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <memory>
#include <utility>
#include <cstring>
#include <cstddef>
#include "project_assert.h"

/**
 * @class fixed_allocator
 * @see https://en.cppreference.com/w/cpp/memory/allocator
 *
 * @tparam The data type which is to be allocated.
 * The type is important for correct data alignment.
 */
template<typename data_type>
class fixed_allocator: public std::allocator<data_type>
{
public:
    using value_type = data_type;

    /**
     * @struct rebind is essential for this class to work properly.
     * It tells std::allocator to use our implementation of allocate and
     * deallocate rather than the default operator new, delete.
     */
    template <class other_type> struct rebind
    {
        using other = fixed_allocator<other_type>;
    };

    ~fixed_allocator()                                  = default;

    // Required by rebind.
    fixed_allocator(fixed_allocator const&)             = default;
    fixed_allocator(fixed_allocator &&)                 = delete;
    fixed_allocator& operator=(fixed_allocator const&)  = delete;
    fixed_allocator& operator=(fixed_allocator&&)       = delete;

    fixed_allocator()
        : buffer_(nullptr),
          buffer_length_(0u),
          in_use_(false)
    {
    }

    /**
     * Create a fixed alocator for the specified data_type.
     *
     * @param buffer The fixed backing store to use for allocation.
     * @param length The number of data_type units held in the
     *               backing store allocation.
     */
    fixed_allocator(value_type *buffer, std::size_t length)
        : buffer_(buffer),
          buffer_length_(length),
          in_use_(false)
    {
    }

    /**
     * Allocates n * sizeof(value_type) bytes of uninitialized storage by
     * calling ::operator new(std::size_t) or
     * ::operator new(std::size_t, std::align_val_t) (since C++17).
     *
     * @param length       The number of value_type elements to allocate.
     *                     Must be <= this->buffer_length_.
     *
     * @return value_type* The allocate data block.
     * @note               For this fixed allocation this function must only
     *                     be called once before deallocate is called.
     *
     * @throw std::bad_alloc If the allocation fails.
     */
    value_type* allocate(std::size_t length)
    {
        ASSERT(length <= this->buffer_length_);
        ASSERT(not this->in_use_);
        this->in_use_ = true;
        return this->buffer_;
    }

    /**
     * Releases the fixed allocation block from use.
     * @param buffer The memory block being freed.
     *               Must be the same as this->buffer_.
     * @param length The number of bytes freed. Unchecked.
     */
    void deallocate(value_type *buffer, std::size_t length)
    {
        (void) length;
        ASSERT(buffer == this->buffer_);
        ASSERT(this->in_use_);
        this->in_use_ = false;
    }

private:
    value_type* buffer_;
    std::size_t buffer_length_;
    bool        in_use_;
};

