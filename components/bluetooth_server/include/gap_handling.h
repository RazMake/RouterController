#pragma once

#include "esp_gap_ble_api.h"

void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);