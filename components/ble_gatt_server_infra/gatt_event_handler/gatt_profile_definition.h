/// @file gatt_profile_definition.h
/// @brief The type definition for a GATT profile
///
///   [PRIVATE] This is not intended to be included directly, but it can, it contains public types.
///       It is included automatically with ble_gatt_server_infra.h (which defines the library contracts).
#pragma once
#include "esp_gatts_api.h"

/// @brief This type encapsulates the information needed for performing the work for a single GATT profile.
///
/// The consumers of the library must crete a table with at least one record for one profile, for the device
/// to accept connections over BLE.
struct gatts_profile_definition
{
    /// @brief The callback method to be invoked to handle GATT events for for this specific profile.
    ///   This must be set when the profile is defined.
    esp_gatts_cb_t gatt_event_handler;

    /// @brief This is an identifier associated by the ESP infrastructure with this profile when it is registered
    ///    on the device. It is going to be used in the generic GATT event handler to determine which profile is
    ///    the event for.
    ///    This is provided by the ESP infrastructure and set on the profile when handling the 'ESP_GATTS_REG_EVT' event.
    ///    This should be initialized with ESP_GATT_IF_NONE value when de profile is defined.
    uint16_t gatts_if;

    /// @brief The number of handles for the current profile.
    ///   This must be set when the profile is defined.
    uint16_t profile_handles_count;  

    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};
