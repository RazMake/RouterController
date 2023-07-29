/// @file ble_gatt_server_infra.h
/// @brief Contract for the BLE GATT server infrastructure module.
///
///   [PUBLIC] This is indended to be directly included by the consumers of the library and so it
///   contains all the definitions that are necessary for the consumers.
#pragma once

#include "esp_gap_ble_api.h"

// ---------------------------------------------------------------------------------------------
// -- Section: BLE_ADVERTISING                                                               ---
// ---------------------------------------------------------------------------------------------
extern uint8_t serviceUUIDsCount;
extern uint8_t *advertised_serviceUUIDs;

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

/// @brief This is called by the consumers of the library to initialize the server.
void initialize_ble_gatt_server(void);