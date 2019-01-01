/**
 * @file ble/gap_connection.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gap_connection.h"
#include "ble/profile_connectable.h"

namespace ble
{
namespace gap
{
    void connection::connect(uint16_t                   connection_handle,
                             ble::gap::address const&   peer_address,
                             uint8_t                    peer_address_id)
    {
        this->set_handle(connection_handle);
        this->get_connecteable()->connection().get_negotiation_state().clear_all_pending();
    }

    void connection::disconnect(uint16_t                connection_handle,
                                ble::hci::error_code    error_code)
    {
        this->set_handle(ble::gap::invalid_handle);
        this->get_connecteable()->connection().get_negotiation_state().clear_all_pending();
    }

} // namespace gap
} // namespace ble
