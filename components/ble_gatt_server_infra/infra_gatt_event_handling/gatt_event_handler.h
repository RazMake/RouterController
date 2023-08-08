/// @file gatt_event_handler.h
/// @brief Definition of the GATT event handler as required by ESP infrastructure.
///
///   [PRIVATE] This is not indended to be directly included by the consumers of the library.
#include "esp_gatts_api.h"
#include "ble_gatt_server_infra.h"
#include "infra_advertising.h"

/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
/// @param event The received event.
/// @param profile_selector A value assigned by the infrastructure to to a user defined profile, which is used to
///   identify the profile an event is targeted at.
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gatt_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t profile_selector, esp_ble_gatts_cb_param_t *param);