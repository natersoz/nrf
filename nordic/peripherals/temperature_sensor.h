/**
 * @file temperature_sensor.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>

/**
 * The temperature sensor notification prototype.
 * @param The temperature in 0.25 C units
 * @param The user supplied context which was passed in to the
 *         temperature_sensor_take_measurement() function.
 */
typedef void (* temperature_sensor_event_handler_t)(int32_t temperature_Cx4,
                                                    void* context);
/**
 * Take a temperature measurement.
 * When the measurement is complete the user supplied event handler
 * will be called.
 *
 * @param handler The user supplied temperature event handler.
 * @param context A user supplied context.
 *
 * @return bool true if the temperature measurement is started successfully..
 *              false if there was a temperature measurement pending; i.e. busy.
 *              false if the handler parameter is null.
 */
bool temperature_sensor_take_measurement(
    temperature_sensor_event_handler_t  handler,
    void*                               context);

