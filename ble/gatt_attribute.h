/**
 * @file ble/gatt_attribute.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_declaration.h"

#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

/**
 * @struct attribute
 * Consists of:
 * - A declaration, which contains:
 *   - A set of properties.
 *   - A handle (of the declaration).
 *   - The UUID 0x2803, attribute_types::characteristic.
 * - A handle.
 * - A uuid.
 * - Data which can be manipulated according to the properties.
 */
struct attribute
{
    virtual ~attribute()                    = default;

    attribute()                             = delete;
    attribute(attribute const&)             = delete;
    attribute(attribute &&)                 = delete;
    attribute& operator=(attribute const&)  = delete;
    attribute& operator=(attribute&&)       = delete;

    attribute(attribute_type attr_type, uint16_t prop_bits) :
        decl(attr_type, prop_bits)
    {
    }

    attribute(uint32_t uuid_32, attribute_type attr_type, uint16_t prop_bits) :
        decl(attr_type, prop_bits)
    {
    }

    declaration     decl;

    /**
     * Write data to the attribute. The base implementation behavior is
     * to write to data_pointer() with the proper length and offset.
     * Classes inherited from ble::gatt::attribute can override this to
     * be notified of the write operation and perform pre or post processing
     * of the data and then call their super class to complete the write.
     *
     * @param write_type The type of write. @see ble::att::op_code
     * @param offset     The offset into the data pointer to begin writing.
     * @param length     The length of the write.
     * @param data       The data to write to the attribute.
     *
     * @return ble::att::length_t The number of octects (bytes) written.
     * @retval truncated length if data_length_max() is exceeded.
     * @retval 0u If the operation is not permitted.
     */
    virtual ble::att::length_t write(ble::att::op_code    write_type,
                                     ble::att::length_t   offset,
                                     ble::att::length_t   length,
                                     void const*          data);

    /**
     * Provide const access to the attribute data.
     * @note This method is virtual and should be overridden by the data type
     * of the attribute being exposed. The non-const does not and should not
     * need to be overridden by class which inherit from attribute.
     */
    virtual void const* data_pointer() const { return nullptr; }

    /**
     * Provide non-const access to the attribute data;
     * @todo This needs to properly deal with the case where the data contained is const.
     * @note This method is non-virtual intentionally.
     */
    void* data_pointer();

    virtual att::length_t data_length()     const { return 0u; }
    virtual att::length_t data_length_max() const { return this->data_length(); }

    bool data_length_is_variable() const {
        return this->data_length() != this->data_length_max();
    }

    /**
     * All atributes, which include characterisitics and descriptors are
     * held within some other container:
     * A service contains characteristics,
     * A characteristic contains descriptors.
     * This list hook enables containment.
     */
    using list_hook_type = boost::intrusive::list_member_hook<
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        >;

    list_hook_type hook_;

    /**
     * This attribute base class does not have a list.
     * However, forward declare the list type here so that it can be easily
     * used by inherited classes.
     */
    using list_type =
        boost::intrusive::list<
            attribute,
            boost::intrusive::constant_time_size<false>,
            boost::intrusive::member_hook<
                attribute, list_hook_type, &attribute::hook_>
        >;
};

} // namespace gatt
} // namespace ble
