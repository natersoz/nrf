/**
 * @file ble/gatt_write_ostream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gatt_write_ostream.h"
#include "ble/gatt_enum_types_strings.h"
#include "ble/gatt_descriptors.h"
#include "ble/uuid.h"

#include "stream.h"
#include "vwritef.h"

namespace ble
{
namespace gatt
{

static void write_padding(io::output_stream& os, size_t count, char pad_char = ' ')
{
    for (; count > 0; --count )
    {
        os.write(&pad_char, sizeof(pad_char));
    }
}

static void declaration_write(io::output_stream&            os,
                              ble::gatt::declaration const& decl)
{
    char buffer[ble::gatt::declaration::conversion_length];
    std::size_t const length =
        decl.to_chars(std::begin(buffer), std::end(buffer)).ptr - std::begin(buffer);
    os.write(buffer, length);
}

static void attribute_base_write(io::output_stream&          os,
                                 ble::gatt::attribute const& attribute)
{
    declaration_write(os, attribute.decl);
    writef(os, " data_length: %3u / %3u", attribute.data_length(),
                                          attribute.data_length_max());
}

static void descriptor_cccd_write(io::output_stream&     os,
                                  ble::gatt::cccd const& descriptor)
{
    write_padding(os, 4u);
    writef(os, "cccd: 0x%04x %c%c",
           descriptor.configuration_bits,
           (descriptor.notifications_enabled() ? 'n' : '-'),
           (descriptor.indications_enabled()   ? 'i' : '-'));
    write_padding(os, 34u);

    attribute_base_write(os, descriptor);
}

static void descriptor_cpfd_write(io::output_stream&     os,
                                  ble::gatt::cpfd const& descriptor)
{
    writef(os, "cpfd: format: 0x%04x, exponent: %u, units: %u",
           static_cast<uint16_t>(descriptor.format),
           static_cast<int>(descriptor.exponent),
           static_cast<int>(descriptor.units));

    attribute_base_write(os, descriptor);
}

static void descriptor_cud_write(io::output_stream&    os,
                                 ble::gatt::cud const& descriptor)
{
    writef(os, "cpfd: '");
    os.write(descriptor.user_string_ptr, descriptor.user_string_length);
    writef(os, "' ");
    attribute_base_write(os, descriptor);
}

static void characteristic_write(io::output_stream&               os,
                                 ble::gatt::characteristic const& characteristic)
{
    writef(os, "characteristic: ");
    if (characteristic.uuid.is_ble())
    {
        uint32_t const uuid_32 = characteristic.uuid.get_u32();
        writef(os, "%08x", uuid_32);

        ble::gatt::characteristic_type const characteristic_type =
            static_cast<ble::gatt::characteristic_type>(uuid_32);

        char const *char_type = ble::gatt::to_string(characteristic_type);
        std::size_t const char_length = strlen(char_type);
        std::size_t const char_width  = 28u;
        std::size_t const n_padding   = char_width - std::min(char_length,
                                                              char_width);
        write_padding(os, n_padding);
        os.write(char_type, char_length);
    }
    else
    {
        char buffer[ble::att::uuid::conversion_length];
        std::size_t const length = characteristic.uuid.to_chars(
            std::begin(buffer), std::end(buffer)).ptr - std::begin(buffer);
        os.write(buffer, length);
    }

    os.write(" ", 1u);

    attribute_base_write(os, characteristic);
    for (ble::gatt::attribute const &attribute : characteristic.descriptor_list)
    {
        os.write("\n", 1u);
        attribute_write(os, attribute);
    }
}

void attribute_write(io::output_stream& os, ble::gatt::attribute const& attribute)
{
    if (attribute.decl.attribute_type == ble::gatt::attribute_type::characteristic)
    {
        ble::gatt::characteristic const& characteristic =
            reinterpret_cast<ble::gatt::characteristic const&>(attribute);
        characteristic_write(os, characteristic);
    }
    else if (attribute.decl.attribute_type == ble::gatt::attribute_type::cccd)
    {
        ble::gatt::cccd const& descriptor =
            reinterpret_cast<ble::gatt::cccd const&>(attribute);
        descriptor_cccd_write(os, descriptor);
    }
    else if (attribute.decl.attribute_type == ble::gatt::attribute_type::cpfd)
    {
        ble::gatt::cpfd const& descriptor =
            reinterpret_cast<ble::gatt::cpfd const&>(attribute);
        descriptor_cpfd_write(os, descriptor);
    }
    else if (attribute.decl.attribute_type == ble::gatt::attribute_type::cud)
    {
        ble::gatt::cud const& descriptor =
            reinterpret_cast<ble::gatt::cud const&>(attribute);
        descriptor_cud_write(os, descriptor);
    }
    else
    {
    }
}

void service_write(io::output_stream& os, ble::gatt::service const& service)
{
    writef(os, "service:        ");
    if (service.uuid.is_ble())
    {
        uint32_t const uuid_32 = service.uuid.get_u32();
        writef(os, "%08x", uuid_32);

        ble::gatt::service_type const service_type =
            static_cast<ble::gatt::service_type>(uuid_32);

        char const *svc_type = ble::gatt::to_string(service_type);
        std::size_t const svc_length = strlen(svc_type);
        std::size_t const svc_width  = 28u;
        std::size_t const n_padding  = svc_width - std::min(svc_length,
                                                            svc_width);
        write_padding(os, n_padding);
        os.write(svc_type, svc_length);
    }
    else
    {
        char buffer[ble::att::uuid::conversion_length];
        std::size_t const length = service.uuid.to_chars(
            std::begin(buffer), std::end(buffer)).ptr - std::begin(buffer);
        os.write(buffer, length);
    }

    os.write(" ", 1u);
    declaration_write(os, service.decl);
#if 0
    for (ble::gatt::attribute const &attribute : service.characteristic_list)
    {
        os.write("\n", 1u);
        attribute_write(os, attribute);
    }
#endif
}

void service_container_write(io::output_stream&                  os,
                             ble::gatt::service_container const& service_container)
{
    for (auto const& node: service_container)
    {
        service_write(os, node);
        os.write("\n", 1u);
    }
}

}  // namespace gatt
}  // namespace ble
