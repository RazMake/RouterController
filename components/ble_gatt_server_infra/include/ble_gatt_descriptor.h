/// @file ble_gatt_descriptor.h
/// @brief The type definition for a GATT characteristic descriptor.
///
///   [PRIVATE] This is not intended to be included directly, but it can, it contains public types.
///       It is included automatically with ble_gatt_server_infra.h (which defines the library contracts).
#pragma once
#include "esp_gatts_api.h"

typedef struct
{
    /// @brief This is a unique identifier of the descriptor.
    ///    This is needed when registering the descriptor.
    ///
    /// Example of 16 bit UUID                   |   Example of 32 bit UUID                        |   Example of 128 bit UUID
    ///  .id =                                   |     .id =                                       |     .id =
	///      {                                   |         {                                       |         {
    ///        .len = ESP_UUID_LEN_16,           |            .len = ESP_UUID_LEN_32,              |            .len = ESP_UUID_LEN_128,
    ///        .uuid = { .uuid16 = 0xFF01, },    |            .uuid = { .uuid32 = 0xFFFFFF01, },   |            .uuid = { .uuid128 = {0x18,0x04,0x09,0xcc,0xbb,0x5b,0x4e,0x2e,0xbe,0xb1,0x0e,0x7e,0x9a,0x14,0x29,0x99} },
    ///      };                                  |         };                                      |         };
    esp_bt_uuid_t id;

    /// @brief This is the index in the descriptors table of the characteristics.
    ///
    /// This is set by the infrastructure when the descriptor is added/registered
    /// (which means the users do not need to wory about setting it, and can rely on it to have the right value after the profile is initialized).
    uint16_t index;

    /// @brief The handle assigned by the ESP infrastructure to this characteristic.
    ///
    /// Note: When this is set (not zero) it means the characteristic was successfully added.
    uint16_t handle;

    /// @brief The value of the descriptor.
    esp_attr_value_t value;

    /// @brief The descriptor access permissions.
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
} ble_gatt_descriptor_t;
