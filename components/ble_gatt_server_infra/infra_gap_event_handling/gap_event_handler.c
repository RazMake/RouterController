/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include "gap_event_handler.h"
#include "esp_log.h"
#define COMPONENT_TAG "GAP_EVENT_HANDLER"

/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
/// @param event The received event.
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            on_advertising_data_set(param->adv_data_cmpl.status);
            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            on_scan_response_set(param->scan_rsp_set.status);
            break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(COMPONENT_TAG, "Advertising start failed: %d", param->adv_start_cmpl.status);
            }
            else
            {
                ESP_LOGI(COMPONENT_TAG, "Advertising started succesfully");
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(COMPONENT_TAG, "Advertising stop failed: %d", param->adv_stop_cmpl.status);
            }
            else
            {
                ESP_LOGI(COMPONENT_TAG, "Advertising stopped successfully");
            }
            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(COMPONENT_TAG, 
                "Update connection parameters status: %d, min interval: %f ms, max interval: %f ms, connection interval: %f ms, latency = %f ms, timeout: %f ms",
                    param->update_conn_params.status,
                    param->update_conn_params.min_int * 0.625,
                    param->update_conn_params.max_int * 0.625,
                    param->update_conn_params.conn_int * 0.625,
                    param->update_conn_params.latency * 0.625,
                    param->update_conn_params.timeout * 0.625);
            break;

        default:
            break;
    }
}