/// @file gatt_characteristic_definition.h
/// @brief The type definition for a GATT profile characteristic (essentially the attributes container within a profile)
///
///   [PRIVATE] This is not intended to be included directly, but it can, it contains public types.
///       It is included automatically with ble_gatt_server_infra.h (which defines the library contracts).
#pragma once
#include "esp_gatts_api.h"

struct gatt_characteristic_definition
{
    /// @brief This is a unique identifier of the characteristic.
    ///    This is needed when registering the characteristic.
    ///
    /// Example of 16 bit UUID                   |   Example of 32 bit UUID                        |   Example of 128 bit UUID
    ///  .id =                                   |     .id =                                       |     .id =
	///      {                                   |         {                                       |         {
    ///        .len = ESP_UUID_LEN_16,           |            .len = ESP_UUID_LEN_32,              |            .len = ESP_UUID_LEN_128,
    ///        .uuid = { .uuid16 = 0xFF01, },    |            .uuid = { .uuid32 = 0xFFFFFF01, },   |            .uuid = { .uuid128 = {0x18,0x04,0x09,0xcc,0xbb,0x5b,0x4e,0x2e,0xbe,0xb1,0x0e,0x7e,0x9a,0x14,0x29,0x99} },
    ///      };                                  |         };                                      |         };
    esp_bt_uuid_t id;

    /// @brief This is the index in the characteristics table of the profile.
    ///
    /// This is set by the infrastructure when the characteristic registration succeeds
    /// (which means the users do not need to wory about setting it, and can rely on it to have the right value after the profile is initialized).
    uint16_t index;

    /// @brief This is the index of the profile that owns this characteristic.
    ///
    /// This is set by the infrastructure when the characteristic registration succeeds
    /// (which means the users do not need to wory about setting it, and can rely on it to have the right value after the profile is initialized).
    uint16_t profile_index;

    /// @brief The handle given by the ESP infrastructure to this specific characteristic.
    ///
    /// This is set by the infrastructure when the service backing the profile is created (in ESP_GATTS_ADD_CHAR_EVT).
    uint16_t handle;

    /// @brief The characteristic properties:
    ///   The value can be a combination of the following:
    ///      ESP_GATT_CHAR_PROP_BIT_BROADCAST
    ///      ESP_GATT_CHAR_PROP_BIT_READ
    ///      ESP_GATT_CHAR_PROP_BIT_WRITE_NR
    ///      ESP_GATT_CHAR_PROP_BIT_WRITE
    ///      ESP_GATT_CHAR_PROP_BIT_NOTIFY
    ///      ESP_GATT_CHAR_PROP_BIT_INDICATE
    ///      ESP_GATT_CHAR_PROP_BIT_AUTH
    ///      ESP_GATT_CHAR_PROP_BIT_EXT_PROP
    esp_gatt_char_prop_t properties;

    /// @brief The caracteristic permissions. Should match the properties.
    ///   The value can be a combination of the following:
    ///      ESP_GATT_PERM_READ
    ///      ESP_GATT_PERM_READ_ENCRYPTED
    ///      ESP_GATT_PERM_READ_ENC_MITM
    ///      ESP_GATT_PERM_WRITE
    ///      ESP_GATT_PERM_WRITE_ENCRYPTED
    ///      ESP_GATT_PERM_WRITE_ENC_MITM
    ///      ESP_GATT_PERM_WRITE_SIGNED
    ///      ESP_GATT_PERM_WRITE_SIGNED_MITM
    ///      ESP_GATT_PERM_READ_AUTHORIZATION
    ///      ESP_GATT_PERM_WRITE_AUTHORIZATION
    ///      ESP_GATT_PERM_ENCRYPT_KEY_SIZE(keysize)
    esp_gatt_perm_t permissions;

    /// @brief The value of the characteristic.
    esp_attr_value_t value;

    /// @brief This is the number of descriptors of this characterstic.
    ///   The characteristic can have 0 or more descriptors.
    uint16_t descriptors_count;

    /// @brief This is a flag indicating whether wether we started adding the descriptors of this charateristic or not.
    ///   Once we start adding the descriptors, we can tell which one was added and which one was not by whih one has
    ///   it's handle property set or not.
    bool descriptors_adding_started;

    /// @brief This is the table of descriptors of this characteristic.
    ///   The characteristic can have 0 or more descriptors.
    struct gatt_characteristic_descriptor_definition* descriptors_table[];
};
