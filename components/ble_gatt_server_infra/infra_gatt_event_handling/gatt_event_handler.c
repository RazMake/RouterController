/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include <string.h>
#include "gatt_event_handler.h"
#include "gatt_profile_registration.h"
#include "gatt_read.h"
#include "esp_log.h"
#define COMPONENT_TAG "GATT_EVENT_HANDLER"

// ------------------------------ Read/Write event handlers -------------------------------------------------------------------------------------------------------------

/// @brief This method is called when ESP infrastructure to indicate the success or failure of a call to esp_ble_gatts_send_indicate(..)
///   if the call was made with need_confirm = false, this event will be called immediately and show the success or failure of the send
///   if the call was made with need_confirm = true, this event is fired when the either the paired device responded or the command
///   timed out waiting for a response.
///   
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_indicate_sent(esp_gatt_if_t profile_selector, struct gatts_conf_evt_param param)
{
    ESP_LOGI(COMPONENT_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param.status, param.handle);
    if (param.status != ESP_GATT_OK)
    {
        esp_log_buffer_hex(COMPONENT_TAG, param.value, param.len);
    }
}

// ------------------------------ Misc event handlers -------------------------------------------------------------------------------------------------------------------
/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_DISCONNECT_EVT (the central device disconnected).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_central_device_disconnected(esp_gatt_if_t profile_selector, struct gatts_disconnect_evt_param param)
{
    ESP_LOGI(COMPONENT_TAG, "Device disconnect reason %d. Restarting advertising", param.reason);
    esp_ble_gap_start_advertising(&ble_advertising_parameters);
}

// ------------------------------ THE ROOT EVENT HANDLER (which invokes all the others) ---------------------------------------------------------------------------------
/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
///    This performs the common tasks (which do not depend on a specific profile, or apply to all profiles)
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gatt_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t profile_selector, esp_ble_gatts_cb_param_t *param)
{
    switch(event)
    {
        case ESP_GATTS_REG_EVT: on_profile_registered(profile_selector, param->reg); return;
        case ESP_GATTS_CREATE_EVT: on_profile_service_created(profile_selector, param->create); return;
        case ESP_GATTS_DISCONNECT_EVT: on_central_device_disconnected(profile_selector, param->disconnect); return;
        case ESP_GATTS_CONF_EVT: on_indicate_sent(profile_selector, param->conf); return;
        case ESP_GATTS_ADD_CHAR_EVT: on_characteristic_created(profile_selector, param->add_char); return;
        case ESP_GATTS_ADD_CHAR_DESCR_EVT: on_characteristic_descriptor_added(profile_selector, param->add_char_descr); return;
        case ESP_GATTS_READ_EVT: on_value_read(profile_selector, param->read); return;
        default: break;
    }
 
    // Otherwise, find the profile by its gatts_if value, and invoke its specific event handler:
    // Note: The ESP_GATT_IF_NONE in the event parameter, means the event applies to all profiles.
    for (int i = 0; i < gatt_profiles_count; i++)
    {
        if (profile_selector == ESP_GATT_IF_NONE || gatt_profiles_table[i]->profile_selector == profile_selector)
        {
            if (gatt_profiles_table[i]->gatt_event_handler)
            {
                gatt_profiles_table[i]->gatt_event_handler(event, param);
            }
            else
            {
                ESP_LOGE(COMPONENT_TAG, "Profile %d does not have a 'profile_selector' defined", i);
            }
        }
    }
}
