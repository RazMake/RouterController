#pragma once
#include "esp_gatts_api.h"

/// @brief This method registers the profiles in the order they're defined in the gatt_profiles_table,
///   one by one, so the registration of the characteristics and descriptors does not get mixed up.
///   This is necesary because the registration is spread out over multiple events and the infrastructure
///   does not provide sufficient identification of what is the parent of each object being registered.
void register_ble_profiles(void);

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_REG_EVT (a new profile was registered and we
///   need to create the service for it).
///   This is also where we can associate the profile with the gatt_if (profile_selector) value so we can route the subsequent events correctly.
///   descriptors.
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_profile_registered(esp_gatt_if_t profile_selector, struct gatts_reg_evt_param param);

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_CREATE_EVT (for a new profile was created and its
///    characteristics must be added).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_profile_service_created(esp_gatt_if_t profile_selector, struct gatts_create_evt_param param);

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_EVT (after it had added a characteristic to a profile).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_characteristic_created(esp_gatt_if_t profile_selector, struct gatts_add_char_evt_param param);

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_DESCR_EVT  ()
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_characteristic_descriptor_added(esp_gatt_if_t profile_selector, struct gatts_add_char_descr_evt_param param);