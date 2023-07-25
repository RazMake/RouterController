#pragma once

#include "esp_gap_ble_api.h"
#include "ble_appprofile.h"

#define DEVICE_NAME "ROUTER_CONTROLLER"

// The Service UUID specific to this device.
extern uint8_t serviceUUID[];

// The  advertising data emitted by this device.
extern esp_ble_adv_data_t ble_advertising_data;

// This structure contains the data that is advertised to the client.
// Calling esp_ble_gap_start_advertising() is making the device actually start advertising.
// Documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html
extern esp_ble_adv_params_t ble_advertising_params;

// The application profile which contains all the router control operations
extern struct ble_appprofile routercontroller_profile;

void set_scanresponse_configured();
void set_advertisingdata_configured();
bool should_start_advertising();