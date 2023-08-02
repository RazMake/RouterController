/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include "gatt_event_handler.h"
#include "esp_log.h"
#define COMPONENT_TAG "GATT_EVENT_HANDLER"

/// @brief This method is registered as callback with the ESP infrastructure to be called when GAP events are fired.
///    This performs the common tasks (which do not depend on a specific profile, or apply to all profiles)
/// @param event The received event.
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event. This structure has a different field for each type of event.
void gatt_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t profile_selector, esp_ble_gatts_cb_param_t *param)
{
    switch(event)
    {
        // This event is fired once we call esp_ble_gatts_app_register(app_id) to actually create the service than
        // handles the profile. This is also where we can associate the profile with the gatt_if value so we can
        // route the subsequent events correctly.
        case ESP_GATTS_REG_EVT:
            if (param->reg.status != ESP_GATT_OK)
            {
                ESP_LOGE(COMPONENT_TAG, "Registration failed for profile %d. Status: %d", param->reg.app_id, param->reg.status);
                return;
            }

            // The app_id value, is exactly the value passed initially to the esp_ble_gatts_app_register(..) call:
            // the index in the profiles table:
            if (param->reg.app_id >= gatt_profiles_count )
            {
                ESP_LOGE(COMPONENT_TAG, "Registration failed. The profile app_id (%d) is invalid", param->reg.app_id);
                return;
            }

            // gatt_profiles_table[param->reg.app_id]->profile_selector = profile_selector;
            // ESP_LOGI(
            //     COMPONENT_TAG,
            //     "Profile %d, associated with 'profile_selector' %d. Creating the service for this profile, requesting %d handles.",
            //     param->reg.app_id,
            //     profile_selector,
            //     gatt_profiles_table[param->reg.app_id]->profile_handles_count);
            //  esp_ble_gatts_create_service(profile_selector, &(gatt_profiles_table[param->reg.app_id]->service_id), gatt_profiles_table[param->reg.app_id]->profile_handles_count);
            return;

        // This event is fired once the ESP infrastructure finishes creating the service corresponding to a profile.
        // This is the place to create the characteristics of the service.
        case ESP_GATTS_CREATE_EVT:
            if (param->create.status != ESP_GATT_OK)
            {
                ESP_LOGE(COMPONENT_TAG, "Service creation failed for profile_selector %d. Status: %d", profile_selector, param->reg.status);
                return;
            }

            ESP_LOGI(COMPONENT_TAG, "Service creation succeeded for profile_selector %d. Status: %d", profile_selector, param->create.status);
            // struct gatt_profile_definition* profile = get_profile_by_selector(profile_selector);
            // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gatts_start_service(param->create.service_handle));

            // if (profile)
            // {
            //     if (profile->create_profile_characteristics)
            //     {
            //         profile->create_profile_characteristics(param->create.service_handle);
            //     }
            //     else
            //     {
            //         ESP_LOGE(COMPONENT_TAG, "Profile with profile_selector=%d does not have the create_profile_characteristics callback set", profile_selector);
            //     }
            // }
            return;

        // This event is fired when the "central device" (phone, computer, etc.) disconnects from this device:
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
        if (profile_selector == ESP_GATT_IF_NONE || gatt_profiles_table[i]->profile_selector == profile_selector)
        {
            if (gatt_profiles_table[i]->gatt_event_handler)
            {
                gatt_profiles_table[i]->gatt_event_handler(event, param);
            }
            else
            {
                ESP_LOGE(COMPONENT_TAG, "Profile %d does not have a 'gatt_event_handler' defined", i);
            }
        }
    }
}