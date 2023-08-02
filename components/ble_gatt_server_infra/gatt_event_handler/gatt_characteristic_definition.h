/// @file gatt_characteristic_definition.h
/// @brief The type definition for a GATT profile
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
};
