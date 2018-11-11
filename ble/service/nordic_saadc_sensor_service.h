/**
 * @file nordic_saadc_sensor_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/service/adc_sensor_service.h"
#include <cstdint>

namespace nordic
{

static constexpr uint8_t const saadc_input_channel_count  = 8u;

using adc_sensor_acquisition =
    ble::service::custom::adc_sensor_acquisition<int16_t>;
using saadc_samples_characteristic =
    ble::service::custom::adc_samples_characteristic<int16_t, saadc_input_channel_count>;
using saadc_enable_characteristic =
    ble::service::custom::adc_enable_characteristic<saadc_input_channel_count>;

using saadc_sensor_service = ble::service::custom::adc_sensor_service;

} // namespace nordic

