#pragma once
#include "esp_gatts_api.h"

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_READ_EVT (the connected devices wants to read a
///    characteristic or descriptor value)
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_value_read(esp_gatt_if_t profile_selector, struct gatts_read_evt_param param);
