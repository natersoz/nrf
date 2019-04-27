/**
 * @file gatt_enum_types.h
 * Enumerated values in this file are copied from:
 * https://www.bluetooth.com/specifications/gatt
 */

#pragma once
#include <cstdint>

namespace ble
{
namespace gatt
{

/**
 * @enum ble::gatt::attribute_type
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * Section 3.4 Summary of GATT Profile Attribute types,
 * Table 3.18: Summary of GATT Profile Attribute types
 */
enum class attribute_type : uint16_t
{
    primary_service                             = 0x2800,   ///< org.bluetooth.attribute.gatt.primary_service_declaration
    secondary_service                           = 0x2801,   ///< org.bluetooth.attribute.gatt.secondary_service_declaration
    include                                     = 0x2802,   ///< org.bluetooth.attribute.gatt.include_declaration
    characteristic                              = 0x2803,   ///< org.bluetooth.attribute.gatt.characteristic_declaration
    characteristic_extended_properties          = 0x2900,   ///< org.bluetooth.descriptor.gatt.characteristic_extended_properties
    characteristic_user_description             = 0x2901,   ///< org.bluetooth.descriptor.gatt.characteristic_user_description
    cud                                         = 0x2901,
    client_characteristic_configuration         = 0x2902,   ///< org.bluetooth.descriptor.gatt.client_characteristic_configuration
    cccd                                        = 0x2902,
    server_characteristic_configuration         = 0x2903,   ///< org.bluetooth.descriptor.gatt.server_characteristic_configuration
    sccd                                        = 0x2903,
    characteristic_presentation_format          = 0x2904,   ///< org.bluetooth.descriptor.gatt.characteristic_presentation_format
    cpfd                                        = 0x2904,
    characteristic_aggregate_format             = 0x2905,   ///< org.bluetooth.descriptor.gatt.characteristic_aggregate_format

    undefined                                   = 0x0000,   ///< Not part of the bluetooth specification.
                                                            ///< Used to signify that the attribute type has not yet been determined.
};

/**
 * @enum ble::gatt::descriptor
 * https://www.bluetooth.com/specifications/gatt/descriptors
 *
 * @note There is overlap with the attribute_types uuids above.
 * @todo Decide if [0x2900 : 0x2905] belong here, up there, or both.
 * Or perhaps they get combined?
 */
enum class descriptor_type: uint16_t
{
    characteristic_extended_properties          = 0x2900,   ///< org.bluetooth.descriptor.gatt.characteristic_extended_properties
    characteristic_user_description             = 0x2901,   ///< org.bluetooth.descriptor.gatt.characteristic_user_description
    client_characteristic_configuration         = 0x2902,   ///< org.bluetooth.descriptor.gatt.client_characteristic_configuration
    cccd                                        = 0x2902,
    server_characteristic_configuration         = 0x2903,   ///< org.bluetooth.descriptor.gatt.server_characteristic_configuration
    sccd                                        = 0x2903,   ///< org.bluetooth.descriptor.gatt.server_characteristic_configuration
    characteristic_presentation_format          = 0x2904,   ///< org.bluetooth.descriptor.gatt.characteristic_presentation_format
    cpfd                                        = 0x2904,   ///< org.bluetooth.descriptor.gatt.characteristic_presentation_format
    characteristic_aggregate_format             = 0x2905,   ///< org.bluetooth.descriptor.gatt.characteristic_aggregate_format
    valid_range                                 = 0x2906,   ///< org.bluetooth.descriptor.valid_range
    external_report_reference                   = 0x2907,   ///< org.bluetooth.descriptor.external_report_reference
    report_reference                            = 0x2908,   ///< org.bluetooth.descriptor.report_reference
    number_of_digitals                          = 0x2909,   ///< org.bluetooth.descriptor.number_of_digitals
    value_trigger_setting                       = 0x290A,   ///< org.bluetooth.descriptor.value_trigger_setting
    environmental_sensing_configuration         = 0x290B,   ///< org.bluetooth.descriptor.es_configuration
    environmental_sensing_measurement           = 0x290C,   ///< org.bluetooth.descriptor.es_measurement
    environmental_sensing_trigger_setting       = 0x290D,   ///< org.bluetooth.descriptor.es_trigger_setting
    time_trigger_setting                        = 0x290E,   ///< org.bluetooth.descriptor.time_trigger_setting
};

/**
 * @enum ble::gatt::services
 * https://www.bluetooth.com/specifications/gatt/services
 */
enum class service_type: uint16_t
{
    generic_access                              = 0x1800,   ///< org.bluetooth.service.generic_access
    generic_attribute                           = 0x1801,   ///< org.bluetooth.service.generic_attribute
    immediate_alert                             = 0x1802,   ///< org.bluetooth.service.immediate_alert
    link_loss                                   = 0x1803,   ///< org.bluetooth.service.link_loss
    tx_power                                    = 0x1804,   ///< org.bluetooth.service.tx_power
    current_time_service                        = 0x1805,   ///< org.bluetooth.service.current_time
    reference_time_update_service               = 0x1806,   ///< org.bluetooth.service.reference_time_update
    next_dst_change_service                     = 0x1807,   ///< org.bluetooth.service.next_dst_change
    glucose                                     = 0x1808,   ///< org.bluetooth.service.glucose
    health_thermometer                          = 0x1809,   ///< org.bluetooth.service.health_thermometer
    device_information                          = 0x180a,   ///< org.bluetooth.service.device_information
    heart_rate                                  = 0x180d,   ///< org.bluetooth.service.heart_rate
    phone_alert_status_service                  = 0x180e,   ///< org.bluetooth.service.phone_alert_status
    battery_service                             = 0x180f,   ///< org.bluetooth.service.battery_service
    blood_pressure                              = 0x1810,   ///< org.bluetooth.service.blood_pressure
    alert_notification_service                  = 0x1811,   ///< org.bluetooth.service.alert_notification
    human_interface_device                      = 0x1812,   ///< org.bluetooth.service.human_interface_device
    scan_parameters                             = 0x1813,   ///< org.bluetooth.service.scan_parameters
    running_speed_and_cadence                   = 0x1814,   ///< org.bluetooth.service.running_speed_and_cadence
    automation_io                               = 0x1815,   ///< org.bluetooth.service.automation_io
    cycling_speed_and_cadence                   = 0x1816,   ///< org.bluetooth.service.cycling_speed_and_cadence
    cycling_power                               = 0x1818,   ///< org.bluetooth.service.cycling_power
    location_and_navigation                     = 0x1819,   ///< org.bluetooth.service.location_and_navigation
    environmental_sensing                       = 0x181a,   ///< org.bluetooth.service.environmental_sensing
    body_composition                            = 0x181b,   ///< org.bluetooth.service.body_composition
    user_data                                   = 0x181c,   ///< org.bluetooth.service.user_data
    weight_scale                                = 0x181d,   ///< org.bluetooth.service.weight_scale
    bond_management_service                     = 0x181e,   ///< org.bluetooth.service.bond_management
    continuous_glucose_monitoring               = 0x181f,   ///< org.bluetooth.service.continuous_glucose_monitoring
    internet_protocol_support_service           = 0x1820,   ///< org.bluetooth.service.internet_protocol_support
    indoor_positioning                          = 0x1821,   ///< org.bluetooth.service.indoor_positioning
    pulse_oximeter_service                      = 0x1822,   ///< org.bluetooth.service.pulse_oximeter
    http_proxy                                  = 0x1823,   ///< org.bluetooth.service.http_proxy
    transport_discovery                         = 0x1824,   ///< org.bluetooth.service.transport_discovery
    object_transfer_service                     = 0x1825,   ///< org.bluetooth.service.object_transfer
    fitness_machine                             = 0x1826,   ///< org.bluetooth.service.fitness_machine
    mesh_provisioning_service                   = 0x1827,   ///< org.bluetooth.service.mesh_provisioning
    mesh_proxy_service                          = 0x1828,   ///< org.bluetooth.service.mesh_proxy
    reconnection_configuration                  = 0x1829,   ///< org.bluetooth.service.reconnection_configuration
}; // enum ble::gatt::services

/**
 * @enum ble::gatt::characteristics
 * https://www.bluetooth.com/specifications/gatt/characteristics
 */
enum class characteristic_type: uint16_t
{
    device_name                                 = 0x2a00,   ///< org.bluetooth.characteristic.gap.device_name
    appearance                                  = 0x2a01,   ///< org.bluetooth.characteristic.gap.appearance
    peripheral_privacy_flag                     = 0x2a02,   ///< org.bluetooth.characteristic.gap.peripheral_privacy_flag
    reconnection_address                        = 0x2a03,   ///< org.bluetooth.characteristic.gap.reconnection_address
    peripheral_preferred_connection_parameters  = 0x2a04,   ///< org.bluetooth.characteristic.gap.peripheral_preferred_connection_parameters
    ppcp                                        = 0x2a04,
    service_changed                             = 0x2a05,   ///< org.bluetooth.characteristic.gatt.service_changed
    alert_level                                 = 0x2a06,   ///< org.bluetooth.characteristic.alert_level
    tx_power_level                              = 0x2a07,   ///< org.bluetooth.characteristic.tx_power_level
    date_time                                   = 0x2a08,   ///< org.bluetooth.characteristic.date_time
    day_of_week                                 = 0x2a09,   ///< org.bluetooth.characteristic.day_of_week
    day_date_time                               = 0x2a0a,   ///< org.bluetooth.characteristic.day_date_time
    exact_time_100                              = 0x2a0b,   ///< org.bluetooth.characteristic.exact_time_100
    exact_time_256                              = 0x2a0c,   ///< org.bluetooth.characteristic.exact_time_256
    dst_offset                                  = 0x2a0d,   ///< org.bluetooth.characteristic.dst_offset
    time_zone                                   = 0x2a0e,   ///< org.bluetooth.characteristic.time_zone
    local_time_information                      = 0x2a0f,   ///< org.bluetooth.characteristic.local_time_information
    secondary_time_zone                         = 0x2a10,   ///< org.bluetooth.characteristic.secondary_time_zone
    time_with_dst                               = 0x2a11,   ///< org.bluetooth.characteristic.time_with_dst
    time_accuracy                               = 0x2a12,   ///< org.bluetooth.characteristic.time_accuracy
    time_source                                 = 0x2a13,   ///< org.bluetooth.characteristic.time_source
    reference_time_information                  = 0x2a14,   ///< org.bluetooth.characteristic.reference_time_information
    time_broadcast                              = 0x2a15,   ///< org.bluetooth.characteristic.time_broadcast
    time_update_control_point                   = 0x2a16,   ///< org.bluetooth.characteristic.time_update_control_point
    time_update_state                           = 0x2a17,   ///< org.bluetooth.characteristic.time_update_state
    glucose_measurement                         = 0x2a18,   ///< org.bluetooth.characteristic.glucose_measurement
    battery_level                               = 0x2a19,   ///< org.bluetooth.characteristic.battery_level
    battery_power_state                         = 0x2a1a,   ///< org.bluetooth.characteristic.battery_power_state
    battery_level_state                         = 0x2a1b,   ///< org.bluetooth.characteristic.battery_level_state
    temperature_measurement                     = 0x2a1c,   ///< org.bluetooth.characteristic.temperature_measurement
    temperature_type                            = 0x2a1d,   ///< org.bluetooth.characteristic.temperature_type
    intermediate_temperature                    = 0x2a1e,   ///< org.bluetooth.characteristic.intermediate_temperature
    temperature_celsius                         = 0x2a1f,   ///< org.bluetooth.characteristic.temperature_celsius
    temperature_fahrenheit                      = 0x2a20,   ///< org.bluetooth.characteristic.temperature_fahrenheit
    measurement_interval                        = 0x2a21,   ///< org.bluetooth.characteristic.measurement_interval
    boot_keyboard_input_report                  = 0x2a22,   ///< org.bluetooth.characteristic.boot_keyboard_input_report
    system_id                                   = 0x2a23,   ///< org.bluetooth.characteristic.system_id
    model_number_string                         = 0x2a24,   ///< org.bluetooth.characteristic.model_number_string
    serial_number_string                        = 0x2a25,   ///< org.bluetooth.characteristic.serial_number_string
    firmware_revision_string                    = 0x2a26,   ///< org.bluetooth.characteristic.firmware_revision_string
    hardware_revision_string                    = 0x2a27,   ///< org.bluetooth.characteristic.hardware_revision_string
    software_revision_string                    = 0x2a28,   ///< org.bluetooth.characteristic.software_revision_string
    manufacturer_name_string                    = 0x2a29,   ///< org.bluetooth.characteristic.manufacturer_name_string
    ieee_11073_20601_regulatory_cert_list       = 0x2a2a,   ///< org.bluetooth.characteristic.ieee_11073-20601_regulatory_certification_data_list
    current_time                                = 0x2a2b,   ///< org.bluetooth.characteristic.current_time
    magnetic_declination                        = 0x2a2c,   ///< org.bluetooth.characteristic.magnetic_declination
    position_2d                                 = 0x2a2f,   ///< org.bluetooth.characteristic.position_2d
    position_3d                                 = 0x2a30,   ///< org.bluetooth.characteristic.position_3d
    scan_refresh                                = 0x2a31,   ///< org.bluetooth.characteristic.scan_refresh
    boot_keyboard_output_report                 = 0x2a32,   ///< org.bluetooth.characteristic.boot_keyboard_output_report
    boot_mouse_input_report                     = 0x2a33,   ///< org.bluetooth.characteristic.boot_mouse_input_report
    glucose_measurement_context                 = 0x2a34,   ///< org.bluetooth.characteristic.glucose_measurement_context
    blood_pressure_measurement                  = 0x2a35,   ///< org.bluetooth.characteristic.blood_pressure_measurement
    intermediate_cuff_pressure                  = 0x2a36,   ///< org.bluetooth.characteristic.intermediate_cuff_pressure
    heart_rate_measurement                      = 0x2a37,   ///< org.bluetooth.characteristic.heart_rate_measurement
    body_sensor_location                        = 0x2a38,   ///< org.bluetooth.characteristic.body_sensor_location
    heart_rate_control_point                    = 0x2a39,   ///< org.bluetooth.characteristic.heart_rate_control_point
    removable                                   = 0x2a3a,   ///< org.bluetooth.characteristic.removable
    service_required                            = 0x2a3b,   ///< org.bluetooth.characteristic.service_required
    scientific_temperature_celsius              = 0x2a3c,   ///< org.bluetooth.characteristic.scientific_temperature_celsius
    string                                      = 0x2a3d,   ///< org.bluetooth.characteristic.string
    network_availability                        = 0x2a3e,   ///< org.bluetooth.characteristic.network_availability
    alert_status                                = 0x2a3f,   ///< org.bluetooth.characteristic.alert_status
    ringer_control_point                        = 0x2a40,   ///< org.bluetooth.characteristic.ringer_control_point
    ringer_setting                              = 0x2a41,   ///< org.bluetooth.characteristic.ringer_setting
    alert_category_id_bit_mask                  = 0x2a42,   ///< org.bluetooth.characteristic.alert_category_id_bit_mask
    alert_category_id                           = 0x2a43,   ///< org.bluetooth.characteristic.alert_category_id
    alert_notification_control_point            = 0x2a44,   ///< org.bluetooth.characteristic.alert_notification_control_point
    unread_alert_status                         = 0x2a45,   ///< org.bluetooth.characteristic.unread_alert_status
    new_alert                                   = 0x2a46,   ///< org.bluetooth.characteristic.new_alert
    supported_new_alert_category                = 0x2a47,   ///< org.bluetooth.characteristic.supported_new_alert_category
    supported_unread_alert_category             = 0x2a48,   ///< org.bluetooth.characteristic.supported_unread_alert_category
    blood_pressure_feature                      = 0x2a49,   ///< org.bluetooth.characteristic.blood_pressure_feature
    hid_information                             = 0x2a4a,   ///< org.bluetooth.characteristic.hid_information
    report_map                                  = 0x2a4b,   ///< org.bluetooth.characteristic.report_map
    hid_control_point                           = 0x2a4c,   ///< org.bluetooth.characteristic.hid_control_point
    report                                      = 0x2a4d,   ///< org.bluetooth.characteristic.report
    protocol_mode                               = 0x2a4e,   ///< org.bluetooth.characteristic.protocol_mode
    scan_interval_window                        = 0x2a4f,   ///< org.bluetooth.characteristic.scan_interval_window
    pnp_id                                      = 0x2a50,   ///< org.bluetooth.characteristic.pnp_id
    glucose_feature                             = 0x2a51,   ///< org.bluetooth.characteristic.glucose_feature
    record_access_control_point                 = 0x2a52,   ///< org.bluetooth.characteristic.record_access_control_point
    rsc_measurement                             = 0x2a53,   ///< org.bluetooth.characteristic.rsc_measurement
    rsc_feature                                 = 0x2a54,   ///< org.bluetooth.characteristic.rsc_feature
    sc_control_point                            = 0x2a55,   ///< org.bluetooth.characteristic.sc_control_point
    digital                                     = 0x2a56,   ///< org.bluetooth.characteristic.digital
    digital_output                              = 0x2a57,   ///< org.bluetooth.characteristic.digital_output
    analog                                      = 0x2a58,   ///< org.bluetooth.characteristic.analog
    analog_output                               = 0x2a59,   ///< org.bluetooth.characteristic.analog_output
    aggregate                                   = 0x2a5a,   ///< org.bluetooth.characteristic.aggregate
    csc_measurement                             = 0x2a5b,   ///< org.bluetooth.characteristic.csc_measurement
    csc_feature                                 = 0x2a5c,   ///< org.bluetooth.characteristic.csc_feature
    sensor_location                             = 0x2a5d,   ///< org.bluetooth.characteristic.sensor_location
    plx_spot_check_measurement                  = 0x2a5e,   ///< org.bluetooth.characteristic.plx_spot_check_measurement
    plx_continuous_measurement_characteristic   = 0x2a5f,   ///< org.bluetooth.characteristic.plx_continuous_measurement
    plx_features                                = 0x2a60,   ///< org.bluetooth.characteristic.plx_features
    pulse_oximetry_control_point                = 0x2a62,   ///< org.bluetooth.characteristic.pulse_oximetry_control_point
    cycling_power_measurement                   = 0x2a63,   ///< org.bluetooth.characteristic.cycling_power_measurement
    cycling_power_vector                        = 0x2a64,   ///< org.bluetooth.characteristic.cycling_power_vector
    cycling_power_feature                       = 0x2a65,   ///< org.bluetooth.characteristic.cycling_power_feature
    cycling_power_control_point                 = 0x2a66,   ///< org.bluetooth.characteristic.cycling_power_control_point
    location_and_speed_characteristic           = 0x2a67,   ///< org.bluetooth.characteristic.location_and_speed
    navigation                                  = 0x2a68,   ///< org.bluetooth.characteristic.navigation
    position_quality                            = 0x2a69,   ///< org.bluetooth.characteristic.position_quality
    ln_feature                                  = 0x2a6a,   ///< org.bluetooth.characteristic.ln_feature
    ln_control_point                            = 0x2a6b,   ///< org.bluetooth.characteristic.ln_control_point
    elevation                                   = 0x2a6c,   ///< org.bluetooth.characteristic.elevation
    pressure                                    = 0x2a6d,   ///< org.bluetooth.characteristic.pressure
    temperature                                 = 0x2a6e,   ///< org.bluetooth.characteristic.temperature
    humidity                                    = 0x2a6f,   ///< org.bluetooth.characteristic.humidity
    true_wind_speed                             = 0x2a70,   ///< org.bluetooth.characteristic.true_wind_speed
    true_wind_direction                         = 0x2a71,   ///< org.bluetooth.characteristic.true_wind_direction
    apparent_wind_speed                         = 0x2a72,   ///< org.bluetooth.characteristic.apparent_wind_speed
    apparent_wind_direction                     = 0x2a73,   ///< org.bluetooth.characteristic.apparent_wind_direction
    gust_factor                                 = 0x2a74,   ///< org.bluetooth.characteristic.gust_factor
    pollen_concentration                        = 0x2a75,   ///< org.bluetooth.characteristic.pollen_concentration
    uv_index                                    = 0x2a76,   ///< org.bluetooth.characteristic.uv_index
    irradiance                                  = 0x2a77,   ///< org.bluetooth.characteristic.irradiance
    rainfall                                    = 0x2a78,   ///< org.bluetooth.characteristic.rainfall
    wind_chill                                  = 0x2a79,   ///< org.bluetooth.characteristic.wind_chill
    heat_index                                  = 0x2a7a,   ///< org.bluetooth.characteristic.heat_index
    dew_point                                   = 0x2a7b,   ///< org.bluetooth.characteristic.dew_point
    descriptor_value_changed                    = 0x2a7d,   ///< org.bluetooth.characteristic.descriptor_value_changed
    aerobic_heart_rate_lower_limit              = 0x2A7E,   ///< org.bluetooth.characteristic.aerobic_heart_rate_lower_limit
    aerobic_threshold                           = 0x2A7F,   ///< org.bluetooth.characteristic.aerobic_threshold
    age                                         = 0x2a80,   ///< org.bluetooth.characteristic.age
    anaerobic_heart_rate_lower_limit            = 0x2a81,   ///< org.bluetooth.characteristic.anaerobic_heart_rate_lower_limit
    anaerobic_heart_rate_upper_limit            = 0x2a82,   ///< org.bluetooth.characteristic.anaerobic_heart_rate_upper_limit
    anaerobic_threshold                         = 0x2a83,   ///< org.bluetooth.characteristic.anaerobic_threshold
    aerobic_heart_rate_upper_limit              = 0x2A84,   ///< org.bluetooth.characteristic.aerobic_heart_rate_upper_limit
    date_of_birth                               = 0x2a85,   ///< org.bluetooth.characteristic.date_of_birth
    date_of_threshold_assessment                = 0x2a86,   ///< org.bluetooth.characteristic.date_of_threshold_assessment
    email_address                               = 0x2a87,   ///< org.bluetooth.characteristic.email_address
    fat_burn_heart_rate_lower_limit             = 0x2a88,   ///< org.bluetooth.characteristic.fat_burn_heart_rate_lower_limit
    fat_burn_heart_rate_upper_limit             = 0x2a89,   ///< org.bluetooth.characteristic.fat_burn_heart_rate_upper_limit
    first_name                                  = 0x2a8a,   ///< org.bluetooth.characteristic.first_name
    five_zone_heart_rate_limits                 = 0x2a8b,   ///< org.bluetooth.characteristic.five_zone_heart_rate_limits
    gender                                      = 0x2a8c,   ///< org.bluetooth.characteristic.gender
    heart_rate_max                              = 0x2a8d,   ///< org.bluetooth.characteristic.heart_rate_max
    height                                      = 0x2a8e,   ///< org.bluetooth.characteristic.height
    hip_circumference                           = 0x2a8f,   ///< org.bluetooth.characteristic.hip_circumference
    last_name                                   = 0x2a90,   ///< org.bluetooth.characteristic.last_name
    maximum_recommended_heart_rate              = 0x2a91,   ///< org.bluetooth.characteristic.maximum_recommended_heart_rate
    resting_heart_rate                          = 0x2a92,   ///< org.bluetooth.characteristic.resting_heart_rate
    sport_type_aerobic_and_anaerobic_thresholds = 0x2a93,   ///< org.bluetooth.characteristic.sport_type_for_aerobic_and_anaerobic_thresholds
    three_zone_heart_rate_limits                = 0x2a94,   ///< org.bluetooth.characteristic.three_zone_heart_rate_limits
    two_zone_heart_rate_limit                   = 0x2a95,   ///< org.bluetooth.characteristic.two_zone_heart_rate_limit
    vo2_max                                     = 0x2a96,   ///< org.bluetooth.characteristic.vo2_max
    waist_circumference                         = 0x2a97,   ///< org.bluetooth.characteristic.waist_circumference
    weight                                      = 0x2a98,   ///< org.bluetooth.characteristic.weight
    database_change_increment                   = 0x2a99,   ///< org.bluetooth.characteristic.database_change_increment
    user_index                                  = 0x2a9a,   ///< org.bluetooth.characteristic.user_index
    body_composition_feature                    = 0x2a9b,   ///< org.bluetooth.characteristic.body_composition_feature
    body_composition_measurement                = 0x2a9c,   ///< org.bluetooth.characteristic.body_composition_measurement
    weight_measurement                          = 0x2a9d,   ///< org.bluetooth.characteristic.weight_measurement
    weight_scale_feature                        = 0x2a9e,   ///< org.bluetooth.characteristic.weight_scale_feature
    user_control_point                          = 0x2a9f,   ///< org.bluetooth.characteristic.user_control_point
    magnetic_flux_density_2d                    = 0x2aa0,   ///< org.bluetooth.characteristic.Magnetic_flux_density_2D
    magnetic_flux_density_3d                    = 0x2aa1,   ///< org.bluetooth.characteristic.Magnetic_flux_density_3D
    language                                    = 0x2aa2,   ///< org.bluetooth.characteristic.language
    barometric_pressure_trend                   = 0x2aa3,   ///< org.bluetooth.characteristic.barometric_pressure_trend
    bond_management_control_point               = 0x2aa4,   ///< org.bluetooth.characteristic.bond_management_control_point
    bond_management_features                    = 0x2aa5,   ///< org.bluetooth.characteristic.bond_management_feature
    central_address_resolution                  = 0x2aa6,   ///< org.bluetooth.characteristic.gap.central_address_resolution
    cgm_measurement                             = 0x2aa7,   ///< org.bluetooth.characteristic.cgm_measurement
    cgm_feature                                 = 0x2aa8,   ///< org.bluetooth.characteristic.cgm_feature
    cgm_status                                  = 0x2aa9,   ///< org.bluetooth.characteristic.cgm_status
    cgm_session_start_time                      = 0x2aaa,   ///< org.bluetooth.characteristic.cgm_session_start_time
    cgm_session_run_time                        = 0x2aab,   ///< org.bluetooth.characteristic.cgm_session_run_time
    cgm_specific_ops_control_point              = 0x2aac,   ///< org.bluetooth.characteristic.cgm_specific_ops_control_point
    indoor_positioning_configuration            = 0x2aad,   ///< org.bluetooth.characteristic.indoor_positioning_configuration
    latitude                                    = 0x2aae,   ///< org.bluetooth.characteristic.latitude
    longitude                                   = 0x2aaf,   ///< org.bluetooth.characteristic.Longitude
    local_north_coordinate                      = 0x2ab0,   ///< org.bluetooth.characteristic.local_north_coordinate
    local_east_coordinate                       = 0x2ab1,   ///< org.bluetooth.characteristic.local_east_coordinate
    floor_number                                = 0x2ab2,   ///< org.bluetooth.characteristic.floor_number
    altitude                                    = 0x2ab3,   ///< org.bluetooth.characteristic.altitude
    uncertainty                                 = 0x2ab4,   ///< org.bluetooth.characteristic.uncertainty
    location_name                               = 0x2ab5,   ///< org.bluetooth.characteristic.location_name
    uri                                         = 0x2ab6,   ///< org.bluetooth.characteristic.uri
    http_headers                                = 0x2ab7,   ///< org.bluetooth.characteristic.http_headers
    http_status_code                            = 0x2ab8,   ///< org.bluetooth.characteristic.http_status_code
    http_entity_body                            = 0x2ab9,   ///< org.bluetooth.characteristic.http_entity_body
    http_control_point                          = 0x2aba,   ///< org.bluetooth.characteristic.http_control_point
    https_security                              = 0x2abb,   ///< org.bluetooth.characteristic.https_security
    tds_control_point                           = 0x2abc,   ///< org.bluetooth.characteristic.tds_control_point
    ots_feature                                 = 0x2abd,   ///< org.bluetooth.characteristic.ots_feature
    object_name                                 = 0x2abe,   ///< org.bluetooth.characteristic.object_name
    object_type                                 = 0x2abf,   ///< org.bluetooth.characteristic.object_type
    object_size                                 = 0x2ac0,   ///< org.bluetooth.characteristic.object_size
    object_first_created                        = 0x2ac1,   ///< org.bluetooth.characteristic.object_first_created
    object_last_modified                        = 0x2ac2,   ///< org.bluetooth.characteristic.object_last_modified
    object_id                                   = 0x2ac3,   ///< org.bluetooth.characteristic.object_id
    object_properties                           = 0x2ac4,   ///< org.bluetooth.characteristic.object_properties
    object_action_control_point                 = 0x2ac5,   ///< org.bluetooth.characteristic.object_action_control_point
    object_list_control_point                   = 0x2ac6,   ///< org.bluetooth.characteristic.object_list_control_point
    object_list_filter                          = 0x2ac7,   ///< org.bluetooth.characteristic.object_list_filter
    object_changed                              = 0x2ac8,   ///< org.bluetooth.characteristic.object_changed
    resolvable_private_address_only             = 0x2ac9,   ///< org.bluetooth.characteristic.resolvable_private_address_only
    fitness_machine_feature                     = 0x2acc,   ///< org.bluetooth.characteristic.fitness_machine_feature
    treadmill_data                              = 0x2acd,   ///< org.bluetooth.characteristic.treadmill_data
    cross_trainer_data                          = 0x2ace,   ///< org.bluetooth.characteristic.cross_trainer_data
    step_climber_data                           = 0x2acf,   ///< org.bluetooth.characteristic.step_climber_data
    stair_climber_data                          = 0x2ad0,   ///< org.bluetooth.characteristic.stair_climber_data
    rower_data                                  = 0x2ad1,   ///< org.bluetooth.characteristic.rower_data
    indoor_bike_data                            = 0x2ad2,   ///< org.bluetooth.characteristic.indoor_bike_data
    training_status                             = 0x2ad3,   ///< org.bluetooth.characteristic.training_status
    supported_speed_range                       = 0x2ad4,   ///< org.bluetooth.characteristic.supported_speed_range
    supported_inclination_range                 = 0x2ad5,   ///< org.bluetooth.characteristic.supported_inclination_range
    supported_resistance_level_range            = 0x2ad6,   ///< org.bluetooth.characteristic.supported_resistance_level_range
    supported_heart_rate_range                  = 0x2ad7,   ///< org.bluetooth.characteristic.supported_heart_rate_range
    supported_power_range                       = 0x2ad8,   ///< org.bluetooth.characteristic.supported_power_range
    fitness_machine_control_point               = 0x2ad9,   ///< org.bluetooth.characteristic.fitness_machine_control_point
    fitness_machine_status                      = 0x2ada,   ///< org.bluetooth.characteristic.fitness_machine_status
    rc_feature                                  = 0x2b1d,   ///< org.bluetooth.characteristic.rc_feature
    rc_settings                                 = 0x2b1e,   ///< org.bluetooth.characteristic.rc_settings
    reconnection_configuration_control_point    = 0x2b1f,   ///< org.bluetooth.characteristic.reconnection_configuration_control_point
}; // enum ble::gatt::characteristics

/**
 * @enum ble::gatt::units
 */
enum class units_type: uint16_t
{
    unitless                                    = 0x2700,   ///< org.bluetooth.unit.unitless
    metre                                       = 0x2701,   ///< org.bluetooth.unit.length.metre
    kilogram                                    = 0x2702,   ///< org.bluetooth.unit.mass.kilogram
    second                                      = 0x2703,   ///< org.bluetooth.unit.time.second
    ampere                                      = 0x2704,   ///< org.bluetooth.unit.electric_current.ampere
    kelvin                                      = 0x2705,   ///< org.bluetooth.unit.thermodynamic_temperature.kelvin
    mole                                        = 0x2706,   ///< org.bluetooth.unit.amount_of_substance.mole
    candela                                     = 0x2707,   ///< org.bluetooth.unit.luminous_intensity.candela
    square_metres                               = 0x2710,   ///< org.bluetooth.unit.area.square_metres
    cubic_metres                                = 0x2711,   ///< org.bluetooth.unit.volume.cubic_metres
    metres_per_second                           = 0x2712,   ///< org.bluetooth.unit.velocity.metres_per_second
    metres_per_second_squared                   = 0x2713,   ///< org.bluetooth.unit.acceleration.metres_per_second_squared
    reciprocal_metre                            = 0x2714,   ///< org.bluetooth.unit.wavenumber.reciprocal_metre
    kilogram_per_cubic_metre                    = 0x2715,   ///< org.bluetooth.unit.density.kilogram_per_cubic_metre
    kilogram_per_square_metre                   = 0x2716,   ///< org.bluetooth.unit.surface_density.kilogram_per_square_metre
    cubic_metre_per_kilogram                    = 0x2717,   ///< org.bluetooth.unit.specific_volume.cubic_metre_per_kilogram
    ampere_per_square_metre                     = 0x2718,   ///< org.bluetooth.unit.current_density.ampere_per_square_metre
    ampere_per_metre                            = 0x2719,   ///< org.bluetooth.unit.magnetic_field_strength.ampere_per_metre
    mole_per_cubic_metre                        = 0x271A,   ///< org.bluetooth.unit.amount_concentration.mole_per_cubic_metre
    kilogram_per_cubic_metre_conentration       = 0x271B,   ///< org.bluetooth.unit.mass_concentration.kilogram_per_cubic_metre
    candela_per_square_metre                    = 0x271C,   ///< org.bluetooth.unit.luminance.candela_per_square_metre
    refractive_index                            = 0x271D,   ///< org.bluetooth.unit.refractive_index
    relative_permeability                       = 0x271E,   ///< org.bluetooth.unit.relative_permeability
    radian                                      = 0x2720,   ///< org.bluetooth.unit.plane_angle.radian
    steradian                                   = 0x2721,   ///< org.bluetooth.unit.solid_angle.steradian
    hertz                                       = 0x2722,   ///< org.bluetooth.unit.frequency.hertz
    newton                                      = 0x2723,   ///< org.bluetooth.unit.force.newton
    pascal                                      = 0x2724,   ///< org.bluetooth.unit.pressure.pascal
    joule                                       = 0x2725,   ///< org.bluetooth.unit.energy.joule
    watt                                        = 0x2726,   ///< org.bluetooth.unit.power.watt
    coulomb                                     = 0x2727,   ///< org.bluetooth.unit.electric_charge.coulomb
    volt                                        = 0x2728,   ///< org.bluetooth.unit.electric_potential_difference.volt
    farad                                       = 0x2729,   ///< org.bluetooth.unit.capacitance.farad
    ohm                                         = 0x272A,   ///< org.bluetooth.unit.electric_resistance.ohm
    siemens                                     = 0x272B,   ///< org.bluetooth.unit.electric_conductance.siemens
    weber                                       = 0x272C,   ///< org.bluetooth.unit.magnetic_flux.weber
    tesla                                       = 0x272D,   ///< org.bluetooth.unit.magnetic_flux_density.tesla
    henry                                       = 0x272E,   ///< org.bluetooth.unit.inductance.henry
    degree_Celsius                              = 0x272F,   ///< org.bluetooth.unit.thermodynamic_temperature.degree_celsius
    lumen                                       = 0x2730,   ///< org.bluetooth.unit.luminous_flux.lumen
    lux                                         = 0x2731,   ///< org.bluetooth.unit.illuminance.lux
    becquerel                                   = 0x2732,   ///< org.bluetooth.unit.activity_referred_to_a_radionuclide.becquerel
    gray                                        = 0x2733,   ///< org.bluetooth.unit.absorbed_dose.gray
    sievert                                     = 0x2734,   ///< org.bluetooth.unit.dose_equivalent.sievert
    katal                                       = 0x2735,   ///< org.bluetooth.unit.catalytic_activity.katal
    pascal_second                               = 0x2740,   ///< org.bluetooth.unit.dynamic_viscosity.pascal_second
    newton_metre                                = 0x2741,   ///< org.bluetooth.unit.moment_of_force.newton_metre
    newton_per_metre                            = 0x2742,   ///< org.bluetooth.unit.surface_tension.newton_per_metre
    radian_per_second                           = 0x2743,   ///< org.bluetooth.unit.angular_velocity.radian_per_second
    radian_per_second_squared                   = 0x2744,   ///< org.bluetooth.unit.angular_acceleration.radian_per_second_squared
    watt_per_square_metre_heat_flux             = 0x2745,   ///< org.bluetooth.unit.heat_flux_density.watt_per_square_metre
    joule_per_kelvin                            = 0x2746,   ///< org.bluetooth.unit.heat_capacity.joule_per_kelvin
    joule_per_kilogram_kelvin                   = 0x2747,   ///< org.bluetooth.unit.specific_heat_capacity.joule_per_kilogram_kelvin
    joule_per_kilogram                          = 0x2748,   ///< org.bluetooth.unit.specific_energy.joule_per_kilogram
    watt_per_metre_kelvin                       = 0x2749,   ///< org.bluetooth.unit.thermal_conductivity.watt_per_metre_kelvin
    joule_per_cubic_metre                       = 0x274A,   ///< org.bluetooth.unit.energy_density.joule_per_cubic_metre
    volt_per_metre                              = 0x274B,   ///< org.bluetooth.unit.electric_field_strength.volt_per_metre
    coulomb_per_cubic_metre                     = 0x274C,   ///< org.bluetooth.unit.electric_charge_density.coulomb_per_cubic_metre
    coulomb_per_square_metre                    = 0x274D,   ///< org.bluetooth.unit.surface_charge_density.coulomb_per_square_metre
    coulomb_per_square_metre_electic_flux       = 0x274E,   ///< org.bluetooth.unit.electric_flux_density.coulomb_per_square_metre
    farad_per_metre                             = 0x274F,   ///< org.bluetooth.unit.permittivity.farad_per_metre
    henry_per_metre                             = 0x2750,   ///< org.bluetooth.unit.permeability.henry_per_metre
    joule_per_mole                              = 0x2751,   ///< org.bluetooth.unit.molar_energy.joule_per_mole
    joule_per_mole_kelvin                       = 0x2752,   ///< org.bluetooth.unit.molar_entropy.joule_per_mole_kelvin
    coulomb_per_kilogram                        = 0x2753,   ///< org.bluetooth.unit.exposure.coulomb_per_kilogram
    gray_per_second                             = 0x2754,   ///< org.bluetooth.unit.absorbed_dose_rate.gray_per_second
    watt_per_steradian                          = 0x2755,   ///< org.bluetooth.unit.radiant_intensity.watt_per_steradian
    watt_per_square_metre_steradian             = 0x2756,   ///< org.bluetooth.unit.radiance.watt_per_square_metre_steradian
    katal_per_cubic_metre                       = 0x2757,   ///< org.bluetooth.unit.catalytic_activity_concentration.katal_per_cubic_metre
    minute                                      = 0x2760,   ///< org.bluetooth.unit.time.minute
    hour                                        = 0x2761,   ///< org.bluetooth.unit.time.hour
    day                                         = 0x2762,   ///< org.bluetooth.unit.time.day
    degree                                      = 0x2763,   ///< org.bluetooth.unit.plane_angle.degree
    plane_angle_minute                          = 0x2764,   ///< org.bluetooth.unit.plane_angle.minute
    plane_angle_second                          = 0x2765,   ///< org.bluetooth.unit.plane_angle.second
    hectare                                     = 0x2766,   ///< org.bluetooth.unit.area.hectare
    litre                                       = 0x2767,   ///< org.bluetooth.unit.volume.litre
    tonne                                       = 0x2768,   ///< org.bluetooth.unit.mass.tonne
    bar                                         = 0x2780,   ///< org.bluetooth.unit.pressure.bar
    millimetre_of_mercury                       = 0x2781,   ///< org.bluetooth.unit.pressure.millimetre_of_mercury
    ngstrm                                      = 0x2782,   ///< org.bluetooth.unit.length.ngstrm
    nautical_mile                               = 0x2783,   ///< org.bluetooth.unit.length.nautical_mile
    barn                                        = 0x2784,   ///< org.bluetooth.unit.area.barn
    knot                                        = 0x2785,   ///< org.bluetooth.unit.velocity.knot
    neper                                       = 0x2786,   ///< org.bluetooth.unit.logarithmic_radio_quantity.neper
    bel                                         = 0x2787,   ///< org.bluetooth.unit.logarithmic_radio_quantity.bel
    yard                                        = 0x27A0,   ///< org.bluetooth.unit.length.yard
    parsec                                      = 0x27A1,   ///< org.bluetooth.unit.length.parsec
    inch                                        = 0x27A2,   ///< org.bluetooth.unit.length.inch
    foot                                        = 0x27A3,   ///< org.bluetooth.unit.length.foot
    mile                                        = 0x27A4,   ///< org.bluetooth.unit.length.mile
    pound_force_per_square_inch                 = 0x27A5,   ///< org.bluetooth.unit.pressure.pound_force_per_square_inch
    kilometre_per_hour                          = 0x27A6,   ///< org.bluetooth.unit.velocity.kilometre_per_hour
    mile_per_hour                               = 0x27A7,   ///< org.bluetooth.unit.velocity.mile_per_hour
    revolution_per_minute                       = 0x27A8,   ///< org.bluetooth.unit.angular_velocity.revolution_per_minute
    gram_calorie                                = 0x27A9,   ///< org.bluetooth.unit.energy.gram_calorie
    kilogram_calorie                            = 0x27AA,   ///< org.bluetooth.unit.energy.kilogram_calorie
    kilowatt_hour                               = 0x27AB,   ///< org.bluetooth.unit.energy.kilowatt_hour
    degree_Fahrenheit                           = 0x27AC,   ///< org.bluetooth.unit.thermodynamic_temperature.degree_fahrenheit
    percentage                                  = 0x27AD,   ///< org.bluetooth.unit.percentage
    per_mille                                   = 0x27AE,   ///< org.bluetooth.unit.per_mille
    beats_per_minute                            = 0x27AF,   ///< org.bluetooth.unit.period.beats_per_minute
    ampere_hours                                = 0x27B0,   ///< org.bluetooth.unit.electric_charge.ampere_hours
    milligram_per_decilitre                     = 0x27B1,   ///< org.bluetooth.unit.mass_density.milligram_per_decilitre
    millimole_per_litre                         = 0x27B2,   ///< org.bluetooth.unit.mass_density.millimole_per_litre
    year                                        = 0x27B3,   ///< org.bluetooth.unit.time.year
    month                                       = 0x27B4,   ///< org.bluetooth.unit.time.month
    count_per_cubic_metre                       = 0x27B5,   ///< org.bluetooth.unit.concentration.count_per_cubic_metre
    watt_per_square_metre_irradiance            = 0x27B6,   ///< org.bluetooth.unit.irradiance.watt_per_square_metre
    per_kilogram_per_minute                     = 0x27B7,   ///< org.bluetooth.unit.transfer_rate.milliliter_per_kilogram_per_minute
    pound                                       = 0x27B8,   ///< org.bluetooth.unit.mass.pound
    metabolic_equivalent                        = 0x27B9,   ///< org.bluetooth.unit.metabolic_equivalent
    steps_per_minute                            = 0x27BA,   ///< org.bluetooth.unit.step_per_minute
    strokes_per_minute                          = 0x27BC,   ///< org.bluetooth.unit.stroke_per_minute
    kilometre_per_minute                        = 0x27BD,   ///< org.bluetooth.unit.velocity.kilometer_per_minute
    lumen_per_watt                              = 0x27BE,   ///< org.bluetooth.unit.luminous_efficacy.lumen_per_watt
    lumen_hour                                  = 0x27BF,   ///< org.bluetooth.unit.luminous_energy.lumen_hour
    lux_hour                                    = 0x27C0,   ///< org.bluetooth.unit.luminous_exposure.lux_hour
    gram_per_second                             = 0x27C1,   ///< org.bluetooth.unit.mass_flow.gram_per_second
    litre_per_second                            = 0x27C2    ///< org.bluetooth.unit.volume_flow.litre_per_second
}; // enum class ble::gatt::units

/**
 * @enum class format
 *
 * @see Bluetooth Core Specification 5.0, Volume 3, Part G
 * 3.3.3.5.2 Format, Table 3.16: Characteristic Format types
 */
enum class format
{
    rfu                     = 0x00,     ///< Reserved for Future Use
    boolean                 = 0x01,     ///< unsigned 1-bit; 0 = false, 1 = true
    two_bit                 = 0x02,     ///< unsigned 2-bit integer
    nibble                  = 0x03,     ///< unsigned 4-bit integer
    uint8                   = 0x04,     ///< unsigned 8-bit integer
    uint12                  = 0x05,     ///< unsigned 12-bit integer
    uint16                  = 0x06,     ///< unsigned 16-bit integer
    uint24                  = 0x07,     ///< unsigned 24-bit integer
    uint32                  = 0x08,     ///< unsigned 32-bit integer
    uint48                  = 0x09,     ///< unsigned 48-bit integer
    uint64                  = 0x0A,     ///< unsigned 64-bit integer
    uint128                 = 0x0B,     ///< unsigned 128-bit integer
    sint8                   = 0x0C,     ///< signed 8-bit integer
    sint12                  = 0x0D,     ///< signed 12-bit integer
    sint16                  = 0x0E,     ///< signed 16-bit integer
    sint24                  = 0x0F,     ///< signed 24-bit integer
    sint32                  = 0x10,     ///< signed 32-bit integer
    sint48                  = 0x11,     ///< signed 48-bit integer
    sint64                  = 0x12,     ///< signed 64-bit integer
    sint128                 = 0x13,     ///< signed 128-bit integer
    float32                 = 0x14,     ///< IEEE-754 32-bit floating point
    float64                 = 0x15,     ///< IEEE-754 64-bit floating point
    SFLOAT                  = 0x16,     ///< IEEE-11073 16-bit SFLOAT
    FLOAT                   = 0x17,     ///< IEEE-11073 32-bit FLOAT
    duint16                 = 0x18,     ///< IEEE-20601 format
    utf8s                   = 0x19,     ///< UTF-8 string
    utf16s                  = 0x1A,     ///< UTF-16 string
    opaque_struct           = 0x1B,     ///< Opaque structure
}; // enum class format

} // namespace gatt
} // namespace ble

