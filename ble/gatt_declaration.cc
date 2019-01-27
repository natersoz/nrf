/**
 * @file ble/gatt_declaration.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gatt_declaration.h"
#include "int_to_string.h"
#include "std_error.h"

namespace ble
{
namespace gatt
{

std::to_chars_result properties::to_chars(char *begin, char *end) const
{
    if (end - begin < 6)
    {
        return std::to_chars_result{begin, std::errc::value_too_large};
    }

    *begin++ = '0';
    *begin++ = 'x';

    uint16_t const props = this->get();
    std::size_t const count = int_to_hex(begin, end - begin, props, 4u);
    begin += count;

    if (end - begin < 10)       // We need 10 more characters to complete
    {                           // the conversion.
        return std::to_chars_result {begin, std::errc::value_too_large};
    }

    *begin++ = ' ';

    uint16_t const all_write_types =
        ble::gatt::properties::write_without_response   |
        ble::gatt::properties::write                    |
        ble::gatt::properties::write_with_signature     |
        ble::gatt::properties::write_reliable           |
        ble::gatt::properties::write_aux                ;

    *begin++ = ((props & ble::gatt::properties::broadcast)? 'b' : '-');
    *begin++ = ((props & ble::gatt::properties::read)?      'r' : '-');
    *begin++ = ((props & all_write_types)?                  'w' : '-');
    *begin++ = ((props & ble::gatt::properties::notify)?    'n' : '-');
    *begin++ = ((props & ble::gatt::properties::indicate)?  'i' : '-');

    if (props & ble::gatt::properties::write_without_response)  { *begin++ = 'N'; }
    if (props & ble::gatt::properties::write_with_signature)    { *begin++ = 'S'; }
    if (props & ble::gatt::properties::write_reliable)          { *begin++ = 'R'; }
    if (props & ble::gatt::properties::write_aux)               { *begin++ = 'A'; }

    // If there is room, write the null terminator.
    if (end - begin > 1) { *begin++ = 0; }

    return std::to_chars_result {begin, errc_success};
}

std::to_chars_result declaration::to_chars(char *begin, char *end) const
{
    using signed_size_t = std::make_signed<std::size_t>::type;
    if (end - begin < static_cast<signed_size_t>(conversion_length))
    {
        return std::to_chars_result{begin, std::errc::value_too_large};
    }

    char const type_prefix[] = "type: 0x";
    memcpy(begin, type_prefix, sizeof(type_prefix));
    begin += std::size(type_prefix) - 1u;       // Overwrite null terminator.

    uint16_t const attr_type = static_cast<uint16_t>(this->attribute_type);
    std::size_t const count = int_to_hex(begin, end - begin, attr_type, 4u);
    begin += count;

    char const properties_prefix[] = " props: ";
    memcpy(begin, properties_prefix, sizeof(properties_prefix));
    begin += std::size(properties_prefix) - 1u; // Overwrite null terminator.
    return this->properties.to_chars(begin, end);
}

} // namespace gatt
} // namespace ble

