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

    switch(event)
    {
        case ESP_GATTS_REG_EVT:
            if (param->reg.status != ESP_GATT_OK)
            {
                ESP_LOGE(COMPONENT_TAG, "Registration failed for profile %d. Status: %d", param->reg.app_id, param->reg.status);
                return;
            }

            if (param->reg.app_id < 0 || param->reg.app_id >= gatt_profiles_count )
            {
                ESP_LOGE(COMPONENT_TAG, "Registration failed. The profile app_id (%d) is invalid", param->reg.app_id);
                return;
            }

            gatt_profiles_table[param->reg.app_id]->gatts_if = gatts_if;
            ESP_LOGI(
                COMPONENT_TAG,
                "Profile %d, associated with 'gatts_if' %d. Creating the service for this profile, requesting %d handles.",
                param->reg.app_id,
                gatts_if,
                gatt_profiles_table[param->reg.app_id]->profile_handles_count);
            // esp_ble_gatts_create_service(gatts_if, &(gatt_profiles_table[param->reg.app_id]->service_id), gatt_profiles_table[param->reg.app_id]->profile_handles_count);
            return;

        case ESP_GATTS_CREATE_EVT:
            // ESP_LOGI(COMPONENT_TAG, "Created service %d. Status: %d, Service Id: %d", param->create.service_handle, param->create.status, param->create.service_id.id.uuid);
            // struct gatt_profile_definition* profile = select_profile_by_gatts_if(gatts_if);
            // if (profile->create_characteristics)
            // {
            //     profile->create_characteristics(param->create);
            // }

            break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(COMPONENT_TAG, "Device disconnect reason 0x%x. Restarting advertising", param->disconnect.reason);
            esp_ble_gap_start_advertising(&ble_advertising_parameters);
            return;

        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(COMPONENT_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
            if (param->conf.status != ESP_GATT_OK)
            {
                esp_log_buffer_hex(COMPONENT_TAG, param->conf.value, param->conf.len);
            }
            return;

        default:
            break;

    }
 
    // Otherwise, find the profile by its gatts_if value, and invoke its specific event handler:
    // Note: The ESP_GATT_IF_NONE in the event parameter, means the event applies to all profiles.
    for (int i = 0; i < gatt_profiles_count; i++)
    {
        if (gatts_if == ESP_GATT_IF_NONE || gatt_profiles_table[i]->gatts_if == gatts_if)
        {
            if (gatt_profiles_table[i]->gatt_event_handler)
            {
                gatt_profiles_table[i]->gatt_event_handler(event, gatts_if, param);
            }
            else
            {
                ESP_LOGE(COMPONENT_TAG, "Profile %d does not have a 'gatt_event_handler' defined", i);
            }
        }
    }
}