/// @file gatt_profile_definition.h
/// @brief The type definition for a GATT profile
///
///   [PRIVATE] This is not intended to be included directly, but it can, it contains public types.
///       It is included automatically with ble_gatt_server_infra.h (which defines the library contracts).
#pragma once
#include "esp_gatts_api.h"

/// @brief The callback signature for the method that creates the profile characteristics. All profiles must provide one.
/// @param service_handle The ESP infrastructure assigned handle for the service supporting the profile.
///          This is needed for adding the characteristic(s).
typedef void (* create_profile_characteristics_t)(uint16_t service_handle);

/// @brief This type encapsulates the information needed for performing the work for a single GATT profile.
///
/// The consumers of the library must crete a table with at least one record for one profile, for the device
/// to accept connections over BLE.
struct gatt_profile_definition
{
    /// @brief The callback method to be invoked to handle GATT events for for this specific profile.
    ///   This must be set when the profile is defined.
    esp_gatts_cb_t gatt_event_handler;

    /// @brief This is an identifier associated by the ESP infrastructure with this profile when it is registered
    ///    on the device. It is going to be used in the generic GATT event handler to determine which profile is
    ///    the event for.
    ///    This is provided by the ESP infrastructure and set on the profile when handling the 'ESP_GATTS_REG_EVT' event.
    ///    This should be initialized with ESP_GATT_IF_NONE value when de profile is defined.
    uint16_t profile_selector;

    /// @brief Each profile must declare the number of handles it will need in total.
    ///   Service handle, characteristic handles, characteristic value handles, characteristic descriptor handles
    uint16_t profile_handles_count;  

    /// @brief This defines the service Id.
    ///    It has two properties:
    ///       - is_primary: This indicates if this is a primary service (or a sub-service of a primary one)
    ///       - id: This is the actual identity of the service that serves this profile. It has two properties:
    ///            - uuid.uuid: The identity of the service (which is a UUID)
    ///            - uuid.len: The size of the UUID (use: ESP_UUID_LEN_16)
    ///    Note: There are some other properties of the id, bu they don't matter.
    esp_gatt_srvc_id_t service_id;

    /// @brief This delegate is invoked by the generic GATT event handler (for ESP_GATTS_CREATE_EVT event) to
    ///    allow this profile to create its characteristics.
    create_profile_characteristics_t create_profile_characteristics;
};
