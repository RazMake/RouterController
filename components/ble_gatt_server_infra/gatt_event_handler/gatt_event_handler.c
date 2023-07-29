/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include "gatt_event_handler.h"
#include "esp_log.h"
#define COMPONENT_TAG "GATT_EVENT_HANDLER"

/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
/// @param event The received event.
/// @param gatts_if ??
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gatt_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    // /* If event is register event, store the gatts_if for each profile */
    // if (event == ESP_GATTS_REG_EVT) {
    //     if (param->reg.status == ESP_GATT_OK) {
    //         gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
    //     } else {
    //         ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d",
    //                 param->reg.app_id,
    //                 param->reg.status);
    //         return;
    //     }
    // }

    // /* If the gatts_if equal to profile A, call profile A cb handler,
    //  * so here call each profile's callback */
    // do {
    //     int idx;
    //     for (idx = 0; idx < PROFILE_NUM; idx++) {
    //         if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
    //                 gatts_if == gl_profile_tab[idx].gatts_if) {
    //             if (gl_profile_tab[idx].gatts_cb) {
    //                 gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
    //             }
    //         }
    //     }
    // } while (0);
}