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

    ESP_LOGE(COMPONENT_TAG, "No profile found with profile_selector=%d", profile_selector);
    return NULL;
}

/// @brief This method selects the characteristic from a profile matching the specified UUID,
/// @param profile The profile whose characteristic we're looking up.
/// @param target_chararacteristic_uuid The UUID of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID value, or NULL if none matched.
struct gatt_characteristic_definition* get_characteristic_by_uuid(struct gatt_profile_definition* profile, esp_bt_uuid_t target_characteristic_uuid)
{
    struct gatt_characteristic_definition* profile_characteristic;
    for (int c = 0; c < profile->characteristics_count; c++)
    {
        profile_characteristic = profile->characteristics_table[c];
        if (profile_characteristic->id.len != target_characteristic_uuid.len)
        {
            continue;
        }

        bool characteristic_found = true;
        switch(target_characteristic_uuid.len)
        {
            case ESP_UUID_LEN_16: characteristic_found = profile_characteristic->id.uuid.uuid16 == target_characteristic_uuid.uuid.uuid16; break;
            case ESP_UUID_LEN_32: characteristic_found = profile_characteristic->id.uuid.uuid32 == target_characteristic_uuid.uuid.uuid32; break;
            case ESP_UUID_LEN_128:
                for (int i=0; i<target_characteristic_uuid.len; i++)
                {
                    if (profile_characteristic->id.uuid.uuid128[i] != target_characteristic_uuid.uuid.uuid128[i])
                    {
                        characteristic_found = false;
                        break;
                    }
                }
                break;

            default:
                ESP_LOGI(
                    COMPONENT_TAG,
                    "Characteristic with unexpected UUID length (%d) looked up in profile with index=%d",
                    target_characteristic_uuid.len,
                    profile->index);
                characteristic_found = false;
                break;
        }

        if (characteristic_found)
        {
            return profile_characteristic;
        }
    }

    ESP_LOGE(COMPONENT_TAG, "No characteristic found with the specified UUID in profile with index=%d", profile->index);
    return NULL;
}

/// @brief This method selects the characteristic from a profile matching the specified characteristic handle (assigned by the ESP infrastructure),
/// @param profile The profile whose characteristic we're looking up.
/// @param target_characteristic_handle The handle of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID values.
struct gatt_characteristic_definition* get_characteristic_by_handle(struct gatt_profile_definition* profile, uint16_t target_characteristic_handle)
{
    struct gatt_characteristic_definition* profile_characteristic;
    for (int c = 0; c < profile->characteristics_count; c++)
    {
        profile_characteristic = profile->characteristics_table[c];
        if (profile_characteristic->handle == target_characteristic_handle)
        {
            return profile_characteristic;
        }
    }

    ESP_LOGE(COMPONENT_TAG, "No characteristic with handle=0x%x was found in profile with index=%d", target_characteristic_handle, profile->index);
    return NULL;
}

/// @brief This method selects the descriptor with the specified UUID from the given characteristic.
/// @param characteristic The characteristic whose descriptors we're looking up.
/// @param target_descriptor_uuid The handle of the descriptor to find and return.
/// @return The descriptor definition that matches the specified UUID value.
struct gatt_characteristic_descriptor_definition* get_descriptor_by_uuid(struct gatt_characteristic_definition* characteristic, esp_bt_uuid_t target_descriptor_uuid)
{
    struct gatt_characteristic_descriptor_definition* descriptor;
    for (int d = 0; d < characteristic->descriptors_count; d++)
    {
        descriptor = characteristic->descriptors_table[d];
        if (descriptor->id.len != target_descriptor_uuid.len)
        {
            continue;
        }

        bool descriptor_found = true;
        switch(target_descriptor_uuid.len)
        {
            case ESP_UUID_LEN_16: descriptor_found = descriptor->id.uuid.uuid16 == target_descriptor_uuid.uuid.uuid16; break;
            case ESP_UUID_LEN_32: descriptor_found = descriptor->id.uuid.uuid32 == target_descriptor_uuid.uuid.uuid32; break;
            case ESP_UUID_LEN_128:
                for (int i=0; i<target_descriptor_uuid.len; i++)
                {
                    if (descriptor->id.uuid.uuid128[i] != target_descriptor_uuid.uuid.uuid128[i])
                    {
                        descriptor_found = false;
                        break;
                    }
                }
                break;

            default:
                ESP_LOGI(
                    COMPONENT_TAG,
                    "Descriptor with unexpected UUID length (%d) looked up in characteristic with index=%d from profile with index=%d",
                    target_descriptor_uuid.len,
                    characteristic->index,
                    characteristic->profile_index);
                descriptor_found = false;
                break;
        }

        if (descriptor_found)
        {
            return descriptor;
        }
    }

    ESP_LOGE(COMPONENT_TAG, "No descriptor was found with the specified UUID in characteristic with index=%d, from profile with index=%d", characteristic->index, characteristic->profile_index);
    return NULL;
}
