#include "ble_gatt_server_infra.h"
#include "gap_event_handler.h"
#include "gatt_event_handler.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
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

    // Register all the profiles defined by the consumer:
    for (int i=0; i < gatt_profiles_count; i++)
    {
        ESP_ERROR_CHECK(esp_ble_gatts_app_register(i));
    }

    // Set the advertising data, so the device starts advertising:
    set_advertising_data_and_scan_response(&ble_advertising_data, &ble_scan_response_data);
}

/// @brief This method selects (from the gatt_profile_table) the profile matching the specified gatts_if value.
/// @param profile_selector The value provided by the infrastructure to identify which profile is an event for.
/// @return The profile definition that matches the specified 'gatts_if' or NULL if nothing matched.
struct gatt_profile_definition* get_profile_by_selector(esp_gatt_if_t profile_selector)
{
    for (int i = 0; i < gatt_profiles_count; i++)
    {
        if (gatt_profiles_table[i]->profile_selector == profile_selector)
        {
            return gatt_profiles_table[i];
        }
    }

    ESP_LOGE(COMPONENT_TAG, "No profile matched 'profile_selector' %d", profile_selector);
    return NULL;
}
