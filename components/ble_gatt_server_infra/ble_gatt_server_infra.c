#include "ble_gatt_server_infra.h"
#include "gap_event_handler.h"
#include "gatt_event_handler.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "gatt_profile_registration.h"
#define COMPONENT_TAG "GATT_SERVER"

/// @brief This is called by the consumers of the library to initialize the server.
void initialize_ble_gatt_server(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bluetooth_config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bluetooth_config));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatt_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_set_device_name(ble_device_name));
    ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(500));

    // Register all profiles defined by the consumer of the platform
    // One at a time, because otherwise registering the characteristics and their descriptors
    // are getting mixed up:
    begin_registering_ble_profiles();

    // Set the advertising data, so the device starts advertising:
    set_advertising_data_and_scan_response(&ble_advertising_data, &ble_scan_response_data);
}


