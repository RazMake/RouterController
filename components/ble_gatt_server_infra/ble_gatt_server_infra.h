/// @file ble_gatt_server_infra.h
/// @brief Contract for the BLE GATT server infrastructure module.
///
///   [PUBLIC] This is indended to be directly included by the consumers of the library and so it
///   contains all the definitions that are necessary for the consumers.
#pragma once

#include "esp_gap_ble_api.h"
#include "gatt_profile_definition.h"
#include "gatt_characteristic_definition.h"
#include "gatt_characteristic_descriptor_definition.h"

// ---------------------------------------------------------------------------------------------
// -- Section: BLE_ADVERTISING                                                               ---
// ---------------------------------------------------------------------------------------------

/// @brief The name of the device, which will be present in the advertising data.
extern const char* ble_device_name;

/// @brief The consumers of the infrastructure must set this variable to the desired advertising
///    parameters for the device.
///
/// Documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_params_t
/// Note: The infrastructure will call esp_ble_gap_start_advertising() to starting the advertising process.
extern esp_ble_adv_params_t ble_advertising_parameters;

/// @brief The consumes of the infrastructure must set this variable to define the desired advertising
///    packet details for the curent device.
///
/// About advertising:
//       https://www.youtube.com/watch?v=KEXRiOYCHCo
// The advertising process: https://academy.nordicsemi.com/topic/advertising-process/
extern esp_ble_adv_data_t ble_advertising_data;

/// @brief This has to be set by the consumers of the library to define the scan response packet.
extern esp_ble_adv_data_t ble_scan_response_data;

// ---------------------------------------------------------------------------------------------
// -- Section: BLE_GATT_PROFILES                                                             ---
// ---------------------------------------------------------------------------------------------

/// @brief The number of GATT profiles defined for this device.
extern const uint8_t gatt_profiles_count;

/// @brief The table containing all the GATT profiles supported by the device.
///
/// Note: This is defined as an array of pointers because I could not find a way to initialize it
///   nicely with instaces of the structs defined in separate files (not inline).
extern struct gatt_profile_definition *gatt_profiles_table[];

/// @brief This method selects (from the gatt_profile_table) the profile matching the specified profile_selector value,
///   wich is assigned when event ESP_GATTS_REG_EVT is handled (in the gatt_event_handler method).
/// @param profile_selector The value provided by the infrastructure to identify which profile is an event for.
/// @return The profile definition that matches the specified 'gatts_if' or NULL if nothing matched.
struct gatt_profile_definition* get_profile_by_selector(esp_gatt_if_t profile_selector);

/// @brief This method selects the characteristic from a profile matching the specified UUID,
/// @param profile The profile whose characteristic we're looking up.
/// @param target_chararacteristic_uuid The UUID of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID values.
struct gatt_characteristic_definition* get_characteristic_by_uuid(struct gatt_profile_definition* profile, esp_bt_uuid_t target_characteristic_uuid);

/// @brief This method selects the characteristic from a profile matching the specified characteristic handle (assigned by the ESP infrastructure),
/// @param profile The profile whose characteristic we're looking up.
/// @param target_characteristic_handle The handle of the characteristic to find and return.
/// @return The characteristic definition that matches the specified handle value.
struct gatt_characteristic_definition* get_characteristic_by_handle(struct gatt_profile_definition* profile, uint16_t target_characteristic_handle);

/// @brief This method selects the descriptor with the specified UUID from the given characteristic.
/// @param characteristic The characteristic whose descriptors we're looking up.
/// @param target_descriptor_uuid The handle of the descriptor to find and return.
/// @return The descriptor definition that matches the specified UUID value.
struct gatt_characteristic_descriptor_definition* get_descriptor_by_uuid(struct gatt_characteristic_definition* characteristic, esp_bt_uuid_t target_descriptor_uuid);

// ---------------------------------------------------------------------------------------------
// -- Section: BLE_SERVER_INITIALIZATION                                                     ---
// ---------------------------------------------------------------------------------------------

/// @brief This is called by the consumers of the library to initialize the server.
void initialize_ble_gatt_server(void);