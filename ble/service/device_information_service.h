/**
 * @file device_information_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * @see https://www.bluetooth.com/specifications/gatt
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_enum_types.h"
#include "int_to_string.h"

#include <array>
#include <algorithm>

namespace ble
{
namespace service
{

/**
 * @see https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_power_state.xml
 *
 * @todo Consider adding the Characteristic Presentation Format utf-8
 * to this characteristic.
 */
template<size_t string_length>
class serial_number_string: public gatt::characteristic
{
public:
    virtual ~serial_number_string() override                      = default;

    serial_number_string(serial_number_string const&)             = delete;
    serial_number_string(serial_number_string &&)                 = delete;
    serial_number_string& operator=(serial_number_string const&)  = delete;
    serial_number_string& operator=(serial_number_string&&)       = delete;

    serial_number_string(void const* data, std::size_t length) :
        gatt::characteristic(gatt::characteristic_type::serial_number_string,
                             gatt::properties::read),
        serial_number({0u})
    {
        // Use of this ctor requires an even number of serial bytes since we
        // are converting binray data to a string of its nybbles.
        static_assert(string_length % 2u == 0u);

        std::size_t const copy_length = std::min(string_length / 2u, length);

        uint8_t const* data_iter = reinterpret_cast<uint8_t const*>(data);
        uint8_t const* data_eptr = data_iter + copy_length;
        auto sn_iter = serial_number.begin();
        while (data_iter < data_eptr)
        {
            bool const upper_case = true;
            *sn_iter++ = nybble_to_char((*data_iter >> 4u) & 0x0Fu, upper_case);
            *sn_iter++ = nybble_to_char((*data_iter >> 0u) & 0x0Fu, upper_case);
            ++data_iter;
        }
    }

    serial_number_string(char const* data) :
        gatt::characteristic(gatt::characteristic_type::serial_number_string,
                             gatt::properties::read),
        serial_number({0u})
    {
        for (auto& sn_iter : this->serial_number)
        {
            if (*data == 0) { break; }
            sn_iter = *data++;
        }
    }

    virtual void const* data_pointer() const override {
        return this->serial_number.data();
    }

    virtual att::length_t data_length() const override {
        return this->serial_number.size();
    }

    std::array<uint8_t, string_length> serial_number;
};

/**
 * @class device_information_service
 * https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=244369
 *
 * @note The characteristics must be added by the code author.
 */
class device_information_service: public gatt::service
{
public:
    virtual ~device_information_service() override                            = default;

    device_information_service(device_information_service const&)             = delete;
    device_information_service(device_information_service &&)                 = delete;
    device_information_service& operator=(device_information_service const&)  = delete;
    device_information_service& operator=(device_information_service&&)       = delete;

    /**
     * Construct the Device Information Service as a primary service.
     */
    device_information_service():
        service(gatt::service_type::device_information,
                gatt::attribute_type::primary_service)
    {
    }
};



} // namespace service
} // namespace ble

