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
#include <cstring>
#include <array>

namespace ble
{
namespace gap
{

/**
 * @struct tlv_data
 * Wraps raw TLV data in a read-only accessor.
 */
struct tlv_data
{
public:
    static constexpr uint8_t offset_length  = 0u;
    static constexpr uint8_t offset_type    = 1u;
    static constexpr uint8_t offset_data    = 2u;

    ~tlv_data()                             = default;

    tlv_data()                              = delete;
    tlv_data(tlv_data const&)               = delete;
    tlv_data(tlv_data &&)                   = delete;
    tlv_data& operator=(tlv_data const&)    = delete;
    tlv_data& operator=(tlv_data&&)         = delete;

    tlv_data(void const *tlv)
    : tlv_pointer(reinterpret_cast<uint8_t const*>(tlv))
    {
    }

    /**
     * Determine the length of the value data field within the TLV encoded
     * object.
     *
     * @return att::length_t The length of the value portion of the TLV encoded
     * data.
     * @note The length includes the type octect as well as the payload data
     * length octets. It does not include the length octect itself.
     */
    att::length_t length() const { return this->tlv_pointer[offset_length]; };

    /**
     * Determine the type the TLV encoded object.
     *
     * @return ble::gap::type The TLV object type.
     */
    ble::gap::type type() const {
        return static_cast<ble::gap::type>(this->tlv_pointer[offset_type]);
    };

    /// @return void const* The pointer to the data portion of the TLV object.
    void const* data() const { return &this->tlv_pointer[offset_data]; };

    uint8_t const* const tlv_pointer;

    uint8_t const* begin() const { return &this->tlv_pointer[0u]; };
    uint8_t const* end()   const { return &this->tlv_pointer[this->length()]; };
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
     */
    class iterator : public std::iterator<std::input_iterator_tag, tlv_data>
    {
    public:
        ~iterator()                             = default;
        iterator()                              = default;
        iterator(iterator const&)               = default;
        iterator(iterator &&)                   = default;
        iterator& operator=(iterator const&)    = default;
        iterator& operator=(iterator&&)         = default;

        iterator(void const* tlv)
        : tlv_pointer(reinterpret_cast<uint8_t const*>(tlv))
        {
        }

        /**
         * Pre-increment operator: ++iterator.
         * Move the iterator to the next instance of TLV data.
         * @return iterator& The iterator after incrementation.
         */
        iterator& operator++()
        {
            // The TLV pointer points to the length part of the TLV object.
            // The length contains the type octet and the data payload octets
            // but not the length octet itself.
            this->tlv_pointer += this->tlv_pointer[tlv_data::offset_length] + 1u;
            return *this;
        }

        /**
         * Post-increment operator: iterator++.
         * Move the iterator to the next instance of TLV data.
         * @return iterator& The iterator before incrementation.
         */
        iterator operator++(int)
        {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(iterator const& other) const
        {
            return (this->tlv_pointer == other.tlv_pointer);
        }

        bool operator!=(iterator const& other) const
        {
            return not (*this == other);
        }

        value_type operator*() const { return tlv_data(this->tlv_pointer); }

    private:
        uint8_t const* tlv_pointer;
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
     * @param data   The advertising data received from a scan.
     * @param length The length of the advertising data received from a scan.
     */
    advertising_data(void const *data, att::length_t length)
    {
        length = std::min(length, max_length);
        memcpy(this->data(), data, length);
        this->index_ = this->data() + length;
    }

    /** @{
     * In he peripheral role the push_back functions are used when forming
     * advertising data in the peripheral role using the tlv_encode functions.
     */
    void push_back(uint8_t value) { *this->index_++ = value; }
    void push_back(tlv_data const& tlv) {
        for (uint8_t data : tlv) { this->push_back(data); }
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
     * The dereferenced iterator type is tlv_data.
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

