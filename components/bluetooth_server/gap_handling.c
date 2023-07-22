#include "gap_handling.h"
#include "esp_log.h"
#include "ble_advertising_params.h"

#define COMPONENT_TAG "GAP_HANDLER"

// ??
#define scan_rsp_config_flag (1 << 1)
// ??
#define adv_config_flag (1 << 0)
// ??
static uint8_t adv_config_done = 0;

// This handler is invoked every time a new GAP event is received by the device
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
#ifdef CONFIG_SET_RAW_ADV_DATA
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~adv_config_flag);
            if (adv_config_done==0)
            {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~scan_rsp_config_flag);
            if (adv_config_done==0)
            {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
#else

        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~adv_config_flag);
            if (adv_config_done == 0)
            {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~scan_rsp_config_flag);
            if (adv_config_done == 0)
            {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
#endif

        // This event is fired after the device once the attempt to start advertising has been made
        // it indicates sucess (advertising is going on) or failure (the device did not start advertising)
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(COMPONENT_TAG, "Advertising start failed: %d", param->adv_start_cmpl.status);
            }
            else
            {
                ESP_LOGI(COMPONENT_TAG, "Advertising started");
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(COMPONENT_TAG, "Advertising stop failed: %d", param->adv_stop_cmpl.status);
            }
            else
            {
                ESP_LOGI(COMPONENT_TAG, "Advertising stopped");
            }
            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(
                COMPONENT_TAG,
                "Update connection params status = %d, min = %f (ms), max = %f (ms), conn interval = %f (ms), latency = %f (ms), timeout = %f (ms)",
                param->update_conn_params.status,
                param->update_conn_params.min_int * 0.625,
                param->update_conn_params.max_int * 0.625,
                param->update_conn_params.conn_int * 0.625,
                param->update_conn_params.latency * 0.625,
                param->update_conn_params.timeout * 0.625);
            break;

        default: break;
    }
}