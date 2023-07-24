#include "ble_config.h"
#include "esp_gatts_api.h"
#include "esp_log.h"

#define COMPONENT_TAG "ROUTERCONTROLLER_TAG"

// This is the callback invoked whenever an event for this profile (router controller) comes in.
// the method is static because it is local to this module, no other module needs to access it.
static void routercontroller_profile_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    // switch (event)
    // {
    // case ESP_GATTS_REG_EVT:
    //      ESP_LOGI(COMPONENT_TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
    //      gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
    //      gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
    //      gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
    //      gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_TEST_A;

    //      esp_ble_gap_set_device_name(TEST_DEVICE_NAME);

    //     //config adv data
    //     esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
    //     if (ret){
    //         ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
    //     }
    //     adv_config_done |= adv_config_flag;
    //     //config scan response data
    //     ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
    //     if (ret){
    //         ESP_LOGE(GATTS_TAG, "config scan response data failed, error code = %x", ret);
    //     }
    //     adv_config_done |= scan_rsp_config_flag;
}

// The profile which contains all the router controller functionality
struct ble_appprofile routercontroller_profile = 
{
    // The callback method that will be used to handle events for this profile
    .profile_callback = routercontroller_profile_callback,

    // ??
    .gatts_if = ESP_GATT_IF_NONE,
};

