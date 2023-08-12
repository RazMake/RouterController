#pragma once
#include "esp_gatts_api.h"

#define begin_registering_ble_profiles() register_ble_profile(0)
/// @brief This method initiates registeration for the profile with the specified index with the BLE runtime infrastructure.
/// @param index The index of the profile to start registering.
///
/// @remarks
///   This is necessary because registering a profile is a multi-step asynchronous operation and the BLE runtime infrastructure
///   is not well enough written (it does not provide the necessary information in the events fired after each step completes).
///   To mitigate this situation we register each profile and its characteristics one at a time, so we implicitly can know
///   which profile and which characteristic we're currently registering (the problem is when registering descriptors for characteristics).
void register_ble_profile(uint8_t index);

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
