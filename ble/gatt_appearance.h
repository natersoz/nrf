/**
 * @file gatt_appearance.h
 * Enumerated values in this file are copied from:
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml
 */

#pragma once
#include <cstdint>

namespace ble
{
namespace gatt
{

/**
 * @enum apearance_type : uint16_t
 * The external device appearance.
 * The values are composed of a category (10-bits) and sub-categories (6-bits).
 *
 * As an example, thermometer_ear consists of (generic_thermometer | 1u).
 * These values can be used 'as is'.
 */
enum class appearance : uint16_t
{
    unknown                                 =    0, // None
    generic_phone                           =   64, // Generic category
    generic_computer                        =  128, // Generic category
    generic_watch                           =  192, // Generic category
    watch_sports_watch                      =  193, // Watch subtype
    generic_clock                           =  256, // Generic category
    generic_display                         =  320, // Generic category
    generic_remote_control                  =  384, // Generic category
    generic_eye_glasses                     =  448, // Generic category
    generic_tag                             =  512, // Generic category
    generic_keyring                         =  576, // Generic category
    generic_media_player                    =  640, // Generic category
    generic_barcode_scanner                 =  704, // Generic category
    generic_thermometer                     =  768, // Generic category
    thermometer_ear                         =  769, // Thermometer subtype
    generic_heart_rate_sensor               =  832, // Generic category
    heart_rate_sensor_heart_rate_belt       =  833, // Heart Rate Sensor subtype
    generic_blood_pressure                  =  896, // Generic category
    blood_pressure_arm                      =  897, // Blood Pressure subtype
    blood_pressure_wrist                    =  898, // Blood Pressure subtype
    human_interface_device                  =  960, // HID Generic
    hid                                     =  960,
    hid_keyboard                            =  961, // HID subtype
    hid_mouse                               =  962, // HID subtype
    hid_joystick                            =  963, // HID subtype
    hid_gamepad                             =  964, // HID subtype
    hid_digitizer_tablet                    =  965, // HID subtype
    hid_card_reader                         =  966, // HID subtype
    hid_digital_pen                         =  967, // HID subtype
    hid_barcode_scanner                     =  968, // HID subtype
    generic_glucose_meter                   = 1024, // Generic category
    generic_running_walking_sensor          = 1088, // Generic category
    running_walking_sensor_in_shoe          = 1089, // Running Walking Sensor subtype
    running_walking_sensor_on_shoe          = 1090, // Running Walking Sensor subtype
    running_walking_sensor_on_hip           = 1091, // Running Walking Sensor subtype
    generic_cycling                         = 1152, // Generic category
    cycling_cycling_computer                = 1153, // Cycling subtype
    cycling_speed_sensor                    = 1154, // Cycling subtype
    cycling_cadence_sensor                  = 1155, // Cycling subtype
    cycling_power_sensor                    = 1156, // Cycling subtype
    cycling_speed_and_cadence_sensor        = 1157, // Cycling: Speed and Cadence Sens // Cycling subtype
    generic_pulse_oximeter                  = 3136, // pulse oximeter generic category
    pulse_oximeter_fingertip                = 3137, // pulse oximeter subtype
    pulse_oximeter_wrist_worn               = 3138, // pulse oximeter subtype
    generic_weight_scale                    = 3200, // weight scale generic category
    generic_personal_mobility_device        = 3264, // personal mobility device
    personal_mobility_powered_wheelchair    = 3265, // personal mobility device
    personal_mobility_scooter               = 3266, // personal mobility device
    generic_continuous_glucose_monitor      = 3328, // continuous glucose monitor
    generic_insulin_pump                    = 3392, // insulin pump
    insulin_pump_durable_pump               = 3393, // insulin pump
    insulin_pump_patch_pump                 = 3396, // insulin pump
    insulin_pen                             = 3400, // insulin pump
    generic_medication_delivery             = 3456, // medication delivery
    generic_outdoor_sports_activity         = 5184, // outdoor sports activity generic category
    location_display_device                 = 5185, // outdoor sports activity subtype
    location_and_navigation_display_device  = 5186, // outdoor sports activity subtype
    location_pod                            = 5187, // outdoor sports activity subtype
    location_and_navigation_pod             = 5188, // outdoor sports activity subtype
    generic_environmental_sensor            = 5696, // environmental Sensor Generic Category
};

} // namespace gatt
} // namespace ble

