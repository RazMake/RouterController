#pragma once

#include "esp_gap_ble_api.h"

#define DEVICE_NAME "Router Controller"

// This structure contains the data that is advertised to the client.
// Calling esp_ble_gap_start_advertising() is making the device actually start advertising.
// Documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html
static esp_ble_adv_params_t adv_params =
{
    // Minimum advertising interval for
    //       undirected
    //          and
    //       low duty cycle directed advertising
    // Range: 0x20-0x4000
    // Default value = 0x800 (1.28 second) => value multiplier = 0.625 milliseconds
    // Which means the range is 20 ms to 10.24 sec.
    .adv_int_min = 0x20, // 20 ms

    // Maximum advertising interval for
    //       undirected
    //          and
    //       low duty cycle directed advertising
    // Same default and value multiplier as adv_int_min
    .adv_int_max = 0x40, // 40 ms
    
    // Advertising type (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv418esp_ble_adv_type_t)
    // ADV_TYPE_IND = generic, not directed to a particular central device and connectable advertisment.
    // ADV_TYPE_DIRECT_IND_HIGH
    // ADV_TYPE_SCAN_IND
    // ADV_TYPE_NONCONN_IND
    // ADV_TYPE_DIRECT_IND_LOW
    .adv_type = ADV_TYPE_IND,

    // Owner bluetooth device address type (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_bt_defs.html#_CPPv419esp_ble_addr_type_t)
    // BLE_WL_ADDR_TYPE_PUBLIC
    // BLE_WL_ADDR_TYPE_RANDOM
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,

    // Advertising channel mask (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv421esp_ble_adv_channel_t)
    // ADV_CHNL_37
    // ADV_CHNL_38
    // ADV_CHNL_39
    // ADV_CHNL_ALL
    .channel_map = ADV_CHNL_ALL,
    
    // Advertising filter policy (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_filter_t)
    // ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = Allow both scan req from White List devices only and connection req from anyone.
    // ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = Allow both scan req from White List devices only and connection req from anyone.
    // ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = Allow both scan req from anyone and connection req from White List devices only.
    // ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = Allow scan and connection requests from White List devices only.
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};