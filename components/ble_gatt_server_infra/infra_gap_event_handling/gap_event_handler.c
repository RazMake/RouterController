/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include "gap_event_handler.h"
#include "esp_log.h"
#define COMPONENT_TAG "GAP_EVENT_HANDLER"

/// @brief This event is fired whenever the advertising status has 
/// @param isStart True the advertising start, False the advertising stop.
/// @param status A value indicating the status code of the operation.
static void on_advertising_status_change(bool isStart, esp_bt_status_t status)
{
    if (status != ESP_BT_STATUS_SUCCESS)
    {
        ESP_LOGE(COMPONENT_TAG, "BLE advertising %s failed. Error code: %d", isStart ? "start" : "stop", status);
    }
    else
    {
        ESP_LOGI(COMPONENT_TAG, "BLE advertising %s successfully", isStart ? "started" : "stopped");
    }
}

/// @brief This event is fired whenever a request to update the connection parameters is received.
/// @param param The event parameters.
static void on_connection_parameters_update(struct ble_update_conn_params_evt_param param)
{
    ESP_LOGI(COMPONENT_TAG, 
        "Update connection parameters status: %d, min interval: %f ms, max interval: %f ms, connection interval: %f ms, latency = %f ms, timeout: %f ms",
            param.status,
            param.min_int * 0.625,
            param.max_int * 0.625,
            param.conn_int * 0.625,
            param.latency * 0.625,
            param.timeout * 0.625);
}

/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
/// @param event The received event.
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: on_advertising_data_set(param->adv_data_cmpl.status); break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT: on_scan_response_set(param->scan_rsp_set.status); break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: on_advertising_status_change(true, param->adv_start_cmpl.status); break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: on_advertising_status_change(false, param->adv_stop_cmpl.status); break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: on_connection_parameters_update(param->update_conn_params); break;
        default: break;
    }
}