/**
 * @file ble/advertising.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/advertising.h"
#include "ble/gap_types.h"

#include "ble/nordic_advertising.h"

namespace ble
{

advertising::advertising()
{
}

advertising::advertising(uint16_t interval)
{
    this->advertising_params.interval = interval;
}

void advertising::start()
{
}

void advertising::stop()
{
}

}
