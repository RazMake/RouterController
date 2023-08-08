/// @file infra_advertising.h
/// @brief Contract for the advertising submodule
///
///   [PRIVATE] This is not indended to be directly included by the consumers of the library.
///
///   This file contains the contracts fo the submodule that handles configuring the advertising packet
///   and the scan response packet.
///
///   Note:
///   The functionality in this module depends on the variables defined in the "BLE_ADVERTISING" section
///   in the public include (ble_gatt_server_infra.h)
#pragma once

/// @brief This is invoked automatically by the ESP infrastructure once the advertising data is set.
/// @param setStatus A value indicating whether the set opearation completed successfully or not.
void on_advertising_data_set(esp_bt_status_t setStatus);

/// @brief This is invoked automatically by the ESP infrastructure once the scan response data is set.
/// @param setStats A value indicating whether the set opearation completed successfully or not.
void on_scan_response_set(esp_bt_status_t setStatus);

/// @brief
///   This method configures the advertising data as well as the scan response data for the device,
///   which must happen before any of the start_* methods are called.
/// @param adv_data The advertising data to be set (leave NULL if only the scan_rsp_data is to be set).
/// @param scan_rsp_data The scan response data to be set (leave NULL if only the adv_data is to be set).
void set_advertising_data_and_scan_response(esp_ble_adv_data_t *adv_data, esp_ble_adv_data_t *scan_rsp_data);
