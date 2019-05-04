/**
 * @file ble/gap_advertising_data.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_types.h"

#include <cstdint>
#include <cstddef>
#include <array>
#include <iterator>

#include <boost/operators.hpp>

namespace ble
{
namespace gap
{

/**
 * @struct ltv_data
 * Wraps raw ltv data in a read-only accessor.
 */
struct ltv_data
{
public:
    static constexpr uint8_t offset_length  = 0u;
    static constexpr uint8_t offset_type    = 1u;
    static constexpr uint8_t offset_data    = 2u;

    ~ltv_data()                             = default;

    ltv_data()                              = delete;
    ltv_data(ltv_data const&)               = delete;
    ltv_data(ltv_data &&)                   = delete;
    ltv_data& operator=(ltv_data const&)    = delete;
    ltv_data& operator=(ltv_data&&)         = delete;

    ltv_data(void const *ltv)
    : ltv_pointer(reinterpret_cast<uint8_t const*>(ltv))
    {
    }

    /**
     * Determine the length of the value data field within the LTV encoded
     * object.
     *
     * @return att::length_t The length of the value portion of the LTV encoded
     * data.
     * @note The length includes the type octect as well as the payload data
     * length octets. It does not include the length octect itself.
     */
    att::length_t length() const { return this->ltv_pointer[offset_length]; };

    /**
     * Determine the type the LTV encoded object.
     *
     * @return ble::gap::type The LTV object type.
     */
    ble::gap::type type() const {
        return static_cast<ble::gap::type>(this->ltv_pointer[offset_type]);
    };

    /// @return void const* The pointer to the data portion of the LTV object.
    void const* data() const { return &this->ltv_pointer[offset_data]; };

    uint8_t const* const ltv_pointer;

    uint8_t const* begin() const { return &this->ltv_pointer[0u]; };
    uint8_t const* end()   const { return &this->ltv_pointer[this->length()]; };
};

/**
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part C page 2086
 * 11 ADVERTISING AND SCAN RESPONSE DATA FORMAT, Figure 11.1
 */
class advertising_data
{
public:
    /**
     * @class advertising_data::iterator
     * Define a class which can const iterate forward through advertising data.
     * Inherits from less_than_comparable<> to get operators <=, >, >=.
     */
    class iterator : public std::iterator<std::input_iterator_tag, ltv_data>,
                     public boost::less_than_comparable<iterator>
    {
    public:
        ~iterator()                             = default;
        iterator()                              = default;
        iterator(iterator const&)               = default;
        iterator(iterator &&)                   = default;
        iterator& operator=(iterator const&)    = default;
        iterator& operator=(iterator&&)         = default;

        iterator(void const* ltv)
        : ltv_pointer(reinterpret_cast<uint8_t const*>(ltv))
        {
        }

        /**
         * Pre-increment operator: ++iterator.
         * Move the iterator to the next instance of LTV data.
         * @return iterator& The iterator after incrementation.
         */
        iterator& operator++()
        {
            // The LTV pointer points to the length part of the LTV object.
            // The length contains the type octet and the data payload octets
            // but not the length octet itself.
            this->ltv_pointer += this->ltv_pointer[ltv_data::offset_length] + 1u;
            return *this;
        }

        /**
         * Post-increment operator: iterator++.
         * Move the iterator to the next instance of LTV data.
         * @return iterator& The iterator before incrementation.
         */
        iterator operator++(int)
        {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        /**
         * @note Since advertising data may be malformed, and the iterator
         * relies on the length parameter, operator != is not defined.
         * Use < rather than != to bound malformed advertising data iteration.
         */
        bool operator==(iterator const& other) const
        {
            return (this->ltv_pointer == other.ltv_pointer);
        }

        bool operator<(iterator const& other) const
        {
            return (this->ltv_pointer < other.ltv_pointer);
        }

        value_type operator*() const { return ltv_data(this->ltv_pointer); }

    private:
        uint8_t const* ltv_pointer;
    };

    static constexpr att::length_t const max_length = 31u;

    using container = std::array<uint8_t, max_length>;

    ~advertising_data()                                     = default;

    advertising_data(advertising_data const&)               = delete;
    advertising_data(advertising_data &&)                   = delete;
    advertising_data& operator=(advertising_data const&)    = delete;
    advertising_data& operator=(advertising_data&&)         = delete;

    /**
     * The default ctor is typically used for creating advertising data within
     * the BLE peripheral role.
     */
    advertising_data(): data_(), index_(data_.begin())
    {
        this->data_.fill(0);
    }

    /**
     * Supplying a data pointer, length to the ctor is typically used within the
     * BLE central role. The advertsing data can be iterated and parsed.
     *
     * @param adv_data The advertising data received from a scan.
     * @param length   The length of the advertising data received from a scan.
     */
    advertising_data(void const *adv_data, att::length_t length)
    {
        container::value_type const* adv_data_ptr =
            reinterpret_cast<container::value_type const*>(adv_data);

        size_t const adv_data_length =
            std::min<size_t>(length, this->data_.max_size());

        std::copy(adv_data_ptr, adv_data_ptr + adv_data_length,
                  std::begin(this->data_));

        this->index_ = this->data() + length;
    }

    /** @{
     * In he peripheral role the push_back functions are used when forming
     * advertising data in the peripheral role using the ltv_encode functions.
     */
    void push_back(uint8_t value) { *this->index_++ = value; }
    void push_back(ltv_data const& ltv) {
        for (uint8_t data : ltv) { this->push_back(data); }
    }
    /// @}

    /** @{
     * The size and capacity functions are used to determine how much space
     * remains available in the advertising data buffer.
     */
    std::size_t size()     const { return this->index_ - this->data_.begin(); }
    std::size_t capacity() const { return this->data_.max_size(); }
    /// @}

    /** @{
     * The advertising_data::iterator allows forward, const iteration within
     * the advertising data typically for parsing the data received from a scan.
     * The dereferenced iterator type is ltv_data.
     */
    iterator begin() const { return iterator(this->data()); }
    iterator end()   const { return iterator(this->index_); }
    /// @}

    /** @{
     * Obtain the beginning and end pointers to the advertising data.
     */
    uint8_t*       end_pointer()       { return this->index_; }
    uint8_t const* end_pointer() const { return this->index_; }

    uint8_t const* data() const { return this->data_.data(); }
    uint8_t*       data()       { return this->data_.data(); }
    /// @}

private:
    container data_;

    /**
     * Points to 'one past the end' of the advertising data.
     * Keeps track of the advertising data usage.
     */
    container::iterator index_;
};

} // namespace gap
} // namespace ble

