#pragma once
#include "esp_bt_defs.h"
#include "ble_gatt_profile.h"

/// @brief This method compares two UUIDs and returns a value indicating whether they are equal or not.
/// @param uuid1 One of the UUIDs to compare.
/// @param uuid2 The other UUID to compare.
/// @return True = the UUIDs are equal, False = otherwise.
bool are_uuids_equal(esp_bt_uuid_t uuid1, esp_bt_uuid_t uuid2);

/// @brief This method selects (from the gatt_profile_table) the profile matching the specified gatts_if value.
/// @param profile_selector The value provided by the infrastructure to identify which profile is an event for.
/// @return The profile definition that matches the specified 'gatts_if' or NULL if nothing matched.
ble_gatt_profile_t* get_profile_by_selector(esp_gatt_if_t selector);

/// @brief This method selects the characteristic from a profile matching the specified UUID,
/// @param profile The profile whose characteristic we're looking up.
/// @param target_chararacteristic_uuid The UUID of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID value, or NULL if none matched.
ble_gatt_characteristic_t* get_characteristic_by_uuid(ble_gatt_profile_t* profile, esp_bt_uuid_t target_characteristic_uuid);

/// @brief This method selects the characteristic from a profile matching the specified characteristic handle (assigned by the ESP infrastructure),
/// @param profile The profile whose characteristic we're looking up.
/// @param target_characteristic_handle The handle of the characteristic to find and return.
/// @return The characteristic definition that matches the specified UUID values.
ble_gatt_characteristic_t* get_characteristic_by_handle(ble_gatt_profile_t* profile, uint16_t target_characteristic_handle);

/// @brief This method selects the descriptor with the specified UUID from the given characteristic.
/// @param characteristic The characteristic whose descriptors we're looking up.
/// @param target_descriptor_uuid The handle of the descriptor to find and return.
/// @return The descriptor definition that matches the specified UUID value.
ble_gatt_descriptor_t* get_descriptor_by_uuid(ble_gatt_characteristic_t* characteristic, esp_bt_uuid_t target_descriptor_uuid);
