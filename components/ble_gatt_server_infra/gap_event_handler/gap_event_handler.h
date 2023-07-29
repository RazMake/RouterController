/// @file gap_event_handler.h
/// @brief Definition of the GAP event handler as required by ESP infrastructure.
///
///   [PRIVATE] This is not indended to be directly included by the consumers of the library.
#include "esp_gap_ble_api.h"
#include "ble_gatt_server_infra.h"
#include "ble_advertising_infra.h"

/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
/// @param event The received event.
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);