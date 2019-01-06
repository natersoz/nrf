/**
 * @file custom_uuid.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "custom_uuid.h"

namespace ble
{
namespace service
{
namespace custom
{

/**
 * The Bluetooth LE Custom Base UUID
 * 0000-CCCC-SSSS-494C-86C6-052628E7D83F
 *
 * @note 16-bit uuid service value will be set in
 *       bytes [4:5] in big-endian order.
 * @note 16-bit uuid characteristic value will set set in
 *       bytes [2:3] in big-endian order.
 * @note Bytes [0:1] must remain zero in order for Nordic GATT clients to
 *       peform service discovery.
 */
constexpr boost::uuids::uuid const uuid_base
{{
    0x00, 0x00,		// Must be set to zero for Nordic GATTC to work.
    0x00, 0x00,		// Increment for characteristics within services.
    0x00, 0x00,		// Increment for each service.
    0x49, 0x4C,     // Fixed ...
    0x86, 0xC6,
    0x05, 0x26, 0x28, 0xE7, 0xD8, 0x3F
}};

static void uuid_set_service(ble::att::uuid& uuid, services service)
{
    uint16_t const service_u16 = static_cast<uint16_t>(service);
    uint8_t *uuid_ptr = uuid.begin() + 4u;
    *uuid_ptr++ = static_cast<uint8_t>(service_u16 >> 16u);
    *uuid_ptr++ = static_cast<uint8_t>(service_u16 >>  0u);
}

static void uuid_set_characteristic(ble::att::uuid& uuid,
                                    characteristics characteristic)
{
    uint16_t const characterisit_u16 = static_cast<uint16_t>(characteristic);
    uint8_t *uuid_ptr = uuid.begin() + 2u;
    *uuid_ptr++ = static_cast<uint8_t>(characterisit_u16 >> 16u);
    *uuid_ptr++ = static_cast<uint8_t>(characterisit_u16 >>  0u);
}

ble::att::uuid& uuid_service(ble::att::uuid& uuid, services service)
{
    uuid = uuid_base;
    uuid_set_service(uuid, service);
    return uuid;
}

ble::att::uuid uuid_service(services service)
{
    ble::att::uuid uuid(uuid_base);
    uuid_set_service(uuid, service);
    return uuid;
}

ble::att::uuid& uuid_characteristic(ble::att::uuid& uuid,
                                    services        service,
                                    characteristics characteristic)
{
    uuid = uuid_base;
    uuid_set_service(uuid, service),
    uuid_set_characteristic(uuid, characteristic);
    return uuid;
}

ble::att::uuid uuid_characteristic(services        service,
                                   characteristics characteristic)
{
    ble::att::uuid uuid(uuid_base);
    uuid_set_service(uuid, service),
    uuid_set_characteristic(uuid, characteristic);
    return uuid;
}

} // namespace custom
} // namespace service
} // namespace ble
