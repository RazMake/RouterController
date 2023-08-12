#include "ble_gatt_server_infra.h"
#include "esp_log.h"
#define COMPONENT_TAG "BLE_UTILS"

/// @brief This method compares two UUIDs and returns a value indicating whether they are equal or not.
/// @param uuid1 One of the UUIDs to compare.
/// @param uuid2 The other UUID to compare.
/// @return True = the UUIDs are equal, False = otherwise.
bool are_uuids_equal(esp_bt_uuid_t uuid1, esp_bt_uuid_t uuid2)
{
    if (uuid1.len != uuid2.len)
    {
        return false;
    }

    switch(uuid1.len)
    {
        case ESP_UUID_LEN_16: return uuid1.uuid.uuid16 == uuid2.uuid.uuid16;
        case ESP_UUID_LEN_32: return uuid1.uuid.uuid32 == uuid2.uuid.uuid32;
        case ESP_UUID_LEN_128:
            for (int i=0; i<uuid1.len; i++)
            {
                if (uuid1.uuid.uuid128[i] != uuid2.uuid.uuid128[i])
                {
                    return false;
                }
            }
            break;
        default: ESP_LOGE(COMPONENT_TAG, "Unexpected UUID length: %d", uuid1.len); abort(); break;
    }

    return true;
}

/// @brief This method selects (from the gatt_profile_table) the profile matching the specified gatts_if value.
/// @param profile_selector The value provided by the infrastructure to identify which profile is an event for.
/// @return The profile definition that matches the specified 'gatts_if' or NULL if nothing matched.
ble_gatt_profile_t* get_profile_by_selector(esp_gatt_if_t selector)
{
    for (int i = 0; i < gatt_profiles_count; i++)
    {
        if (gatt_profiles_table[i]->profile_selector == selector)
        {
            return gatt_profiles_table[i];
        }
    }

    ESP_LOGE(COMPONENT_TAG, "No profile found with profile_selector=%d", selector);
    return NULL;
}

/// @brief This method selects the characteristic from a profile matching the specified UUID,
/// @param profile The profile whose characteristic we're looking up.
/// @param target_chararacteristic_uuid The UUID of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID value, or NULL if none matched.
ble_gatt_characteristic_t* get_characteristic_by_uuid(ble_gatt_profile_t* profile, esp_bt_uuid_t target_characteristic_uuid)
{
    ble_gatt_characteristic_t* characteristic;
    for (int c = 0; c < profile->characteristics_count; c++)
    {
        characteristic = profile->characteristics_table[c];
        if (are_uuids_equal(characteristic->id, target_characteristic_uuid))
        {
            return characteristic;
        }
    }

    return NULL;
}

/// @brief This method selects the characteristic from a profile matching the specified characteristic handle (assigned by the ESP infrastructure),
/// @param profile The profile whose characteristic we're looking up.
/// @param target_characteristic_handle The handle of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID values.
ble_gatt_characteristic_t* get_characteristic_by_handle(ble_gatt_profile_t* profile, uint16_t target_characteristic_handle)
{
    ble_gatt_characteristic_t* characteristic;
    for (int c = 0; c < profile->characteristics_count; c++)
    {
        characteristic = profile->characteristics_table[c];
        if (characteristic->handle == target_characteristic_handle)
        {
            return characteristic;
        }
    }

    return NULL;
}

/// @brief This method selects the descriptor with the specified UUID from the given characteristic.
/// @param characteristic The characteristic whose descriptors we're looking up.
/// @param target_descriptor_uuid The handle of the descriptor to find and return.
/// @return The descriptor definition that matches the specified UUID value.
ble_gatt_descriptor_t* get_descriptor_by_uuid(ble_gatt_characteristic_t* characteristic, esp_bt_uuid_t target_descriptor_uuid)
{
    ble_gatt_descriptor_t* descriptor;
    for (int d = 0; d < characteristic->descriptors_count; d++)
    {
        descriptor = characteristic->descriptors_table[d];
        if (are_uuids_equal(descriptor->id, target_descriptor_uuid))
        {
            return descriptor;
        }
    }

    return NULL;
}
