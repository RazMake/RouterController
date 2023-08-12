/// @file infra_advertising.c
/// @brief The machinery for setting and initiating advertising of the device.
///
///   Note:
///   The functionality in this module depends on the variables defined in the "BLE_ADVERTISING" section
///   in the public include (ble_gatt_server_infra.h)
#include "ble_gatt_server_infra.h"
#include "infra_advertising.h"
#include "esp_gap_ble_api.h"
#include "esp_log.h"
#define COMPONENT_TAG "BLE_ADVERTISING"

/// @brief 
///   This is used to track whether we have sent or not each of the advertising packetes:
///   the advertising data or the scan response.
static uint8_t advertising_configured_bits = 0; // Initialy no response is sent.

/// @brief Bit 0 in advertising_configured_bits is used to track whether we have advertising data packet set and ready to be sent.
#define advertising_data_configured_bit 0x01

/// @brief Bit 1 in advertising_configured_bits is used to track whether we have scan response data packet set and ready to be sent.
#define scan_response_configured_bit 0x02

/// @brief [PRIVATE] This mehtod is called by the on_*_set() methods to start advertising.
static void start_advertising(void)
{
    // Then start advertising with the current parameters, but only if there isn't another bit still set,
    // to avoid starting advertising multiple times.
    if (advertising_configured_bits == 0)
    {
        ESP_LOGI(COMPONENT_TAG, "Request start advertising");
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gap_start_advertising(&ble_advertising_parameters));
    }
    else
    {
        ESP_LOGI(COMPONENT_TAG, "Not ready to start advertising, waiting for more 'complete' events: %d", advertising_configured_bits);
    }
}

/// @brief This is invoked automatically by the ESP infrastructure once the advertising data is set.
/// @param setStatus A value indicating whether the set opearation completed successfully or not.
void on_advertising_data_set(esp_bt_status_t setStatus)
{
    if (setStatus != ESP_BT_STATUS_SUCCESS)
    {
        ESP_LOGE(COMPONENT_TAG, "Setting advertising data failed: %d. Will not start advertising", setStatus);
    }
    else
    {
        // Reset the advertising data set flag, so we have to set it again next time.
        advertising_configured_bits &= (~advertising_data_configured_bit);
        start_advertising();
    }
}

/// @brief This is invoked automatically by the ESP infrastructure once the scan response data is set.
/// @param setStats A value indicating whether the set opearation completed successfully or not.
void on_scan_response_set(esp_bt_status_t setStatus)
{
    if (setStatus != ESP_BT_STATUS_SUCCESS)
    {
        ESP_LOGE(COMPONENT_TAG, "Setting scan response data failed: %d. Will not start advertising", setStatus);
    }
    else
    {
        // Reset the scan response set flag, so we have to set it again next time.
        advertising_configured_bits &= (~scan_response_configured_bit);
        start_advertising();
    }
}

/// @brief
///   This method configures the advertising data as well as the scan response data for the device,
///   which must happen before any of the start_* methods are called.
/// @param adv_data The advertising data to be set (leave NULL if only the scan_rsp_data is to be set).
/// @param scan_rsp_data The scan response data to be set (leave NULL if only the adv_data is to be set).
void set_advertising_data_and_scan_response(esp_ble_adv_data_t *adv_data, esp_ble_adv_data_t *scan_rsp_data)
{
    if (adv_data != NULL)
    {
        ESP_LOGI(COMPONENT_TAG, "Setting advertising data");
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gap_config_adv_data(adv_data));
        advertising_configured_bits |= advertising_data_configured_bit;
    }

    if (scan_rsp_data != NULL)
    {
        ESP_LOGI(COMPONENT_TAG, "Setting scan response data");
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gap_config_adv_data(scan_rsp_data));
        advertising_configured_bits |= scan_response_configured_bit;
    }
}