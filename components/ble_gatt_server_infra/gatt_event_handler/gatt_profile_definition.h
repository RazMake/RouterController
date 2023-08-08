/// @file gatt_profile_definition.h
/// @brief The type definition for a GATT profile
///
///   [PRIVATE] This is not intended to be included directly, but it can, it contains public types.
///       It is included automatically with ble_gatt_server_infra.h (which defines the library contracts).
#pragma once
#include "esp_gatts_api.h"
#include "gatt_characteristic_definition.h"

/// @brief The callback signature for the method that creates the profile characteristics. All profiles must provide one.
/// @param event The event to be handled. This drives which part of the param structure contains the interesting data.
/// @param param The parameters corresponding to the event.
typedef void (* gatts_profile_handler_t)(esp_gatts_cb_event_t event, esp_ble_gatts_cb_param_t *param);

/// @brief This type encapsulates the information needed for performing the work for a single GATT profile.
///
/// The consumers of the library must crete a table with at least one record for one profile, for the device
/// to accept connections over BLE.
typedef struct
 {
    /// @brief This is a unique identifier of the profile.
    ///    This is needed when registering the profile (aka. service).
    ///
    /// Example of 16 bit UUID                   |   Example of 32 bit UUID                        |   Example of 128 bit UUID
    ///  .id =                                   |     .id =                                       |     .id =
	///      {                                   |         {                                       |         {
    ///        .len = ESP_UUID_LEN_16,           |            .len = ESP_UUID_LEN_32,              |            .len = ESP_UUID_LEN_128,
    ///        .uuid = { .uuid16 = 0xFF01, },    |            .uuid = { .uuid32 = 0xFFFFFF01, },   |            .uuid = { .uuid128 = {0x18,0x04,0x09,0xcc,0xbb,0x5b,0x4e,0x2e,0xbe,0xb1,0x0e,0x7e,0x9a,0x14,0x29,0x99} },
    ///      };                                  |         };                                      |         };
    esp_bt_uuid_t id;

    /// @brief This is an identifier associated by the ESP infrastructure with this profile when it is registered
    ///    on the device. It is going to be used in the generic GATT event handler to determine which profile is
    ///    the event for.
    ///    This is provided by the ESP infrastructure and set on the profile when handling the 'ESP_GATTS_REG_EVT' event.
    ///    This should be initialized with ESP_GATT_IF_NONE value when de profile is defined.
    uint16_t profile_selector;

    /// @brief This is the index in the profiles table where this profile has been added.
    ///
    /// This is set by the infrastructure when the profile registration succeeds
    /// (which means the users do not need to wory about setting it, and can rely on it to have the right value after the profile is initialized).
    uint16_t index;

    /// @brief The handle given by the ESP infrastructure to this specific profile.
    ///
    /// This is set by the infrastructure when the service backing the profile is created (in ESP_GATTS_CREATE_EVT).
    uint16_t service_handle;

    /// @brief The callback method to be invoked to handle GATT events for for this specific profile.
    ///   This must be set when the profile is defined.
    gatts_profile_handler_t gatt_event_handler;

    /// @brief This keeps the count of characteristics in this profile.
    uint8_t characteristics_count;

    /// @brief This defines the list of characteristics for this profile.
    ble_gatt_characteristic_t* characteristics_table[];
} ble_gatt_profile_t;
