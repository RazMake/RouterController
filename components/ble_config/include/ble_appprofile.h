#pragma once

#include "esp_gatts_api.h"

// This structure encapsulates all properties of a bluetooth BLE GATT profile
struct ble_appprofile
{
    // The callback invoked whenever a request/event for this profile is received by the server.
    esp_gatts_cb_t profile_callback;

    // ?
    uint16_t gatts_if;

    // ?
    uint16_t app_id;

    // ?
    uint16_t conn_id;

    // ?
    uint16_t service_handle;

    // ?
    esp_gatt_srvc_id_t service_id;

    // ?
    uint16_t char_handle;

    // ?
    esp_bt_uuid_t char_uuid;

    // ?
    esp_gatt_perm_t perm;

    // ?
    esp_gatt_char_prop_t property;

    // ?
    uint16_t descr_handle;

    // ?
    esp_bt_uuid_t descr_uuid;
};
