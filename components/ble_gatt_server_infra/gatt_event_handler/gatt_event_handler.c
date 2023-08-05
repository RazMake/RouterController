/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include "gatt_event_handler.h"
#include "esp_log.h"
#define COMPONENT_TAG "GATT_EVENT_HANDLER"

/// @brief This is a method that counts the handles needed for this profile.
/// @param profile A pointer to the profile whose handles to count.
/// @returns The calculated number of handles needed by this profile.
static uint16_t get_handles_count(struct gatt_profile_definition* profile)
{
    uint16_t handles_count = 1; // A handle is required by the service of this profile;

    // Add characteristic and descriptor handles, if there are any defined.
    struct gatt_characteristic_definition* characteristic;
    for (int c = 0; c< profile->characteristics_count; c++)
    {
        characteristic = profile->characteristics_table[c];
        handles_count++; // Add the characteristic handle
        handles_count++; // Add the characteristic value handle

        // And then add the descriptors handles
        handles_count = handles_count + characteristic->descriptors_count;
    }

    return handles_count;
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_DISCONNECT_EVT (the central device disconnected).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_central_device_disconnected(esp_gatt_if_t profile_selector, struct gatts_disconnect_evt_param param)
{
    ESP_LOGI(COMPONENT_TAG, "Device disconnect reason 0x%x. Restarting advertising", param.reason);
    esp_ble_gap_start_advertising(&ble_advertising_parameters);
}

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

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_REG_EVT (for a new profile was registered).
///   This is also where we can associate the profile with the gatt_if (profile_selector) value so we can route the subsequent events correctly.
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_profile_registered(esp_gatt_if_t profile_selector, struct gatts_reg_evt_param param)
{
    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Registration failed for profile %d. Status: %d", param.app_id, param.status);
        return;
    }

    // The app_id value, is exactly the value passed initially to the esp_ble_gatts_app_register(..) call:
    // the index in the profiles table:
    if (param.app_id >= gatt_profiles_count)
    {
        ESP_LOGE(COMPONENT_TAG, "Registration failed. The profile app_id (%d) is invalid", param.app_id);
        return;
    }

    struct gatt_profile_definition* profile = gatt_profiles_table[param.app_id];
    esp_gatt_srvc_id_t service_id =
    {
        .is_primary = true, // I have simplified the definition a bit and considering all services to be primary since I had no case of imbricated services.
        .id =
        {
            .inst_id = 0, // Another simplification: I only have services with unique ids (no two services are allowed to have the same id).
            .uuid = profile->id,
        }
    };
    uint16_t handles_count = get_handles_count(profile);
    profile->profile_selector = profile_selector;
    profile->index = param.app_id; // This could be set by the user, but we also have all the information to set it here, so making this automatic.
    ESP_LOGI(
        COMPONENT_TAG,
        "Profile %d, associated with 'profile_selector' %d. Creating the service for this profile, requesting %d handles (with %d characteristics)",
        param.app_id,
        profile_selector,
        handles_count,
        profile->characteristics_count);
        esp_ble_gatts_create_service(profile_selector, &service_id, handles_count);
    return;
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_CREATE_EVT (for a new profile was created and its
///    characteristics must be added).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_profile_service_created(esp_gatt_if_t profile_selector, struct gatts_create_evt_param param)
{
    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Service creation failed for profile_selector %d. Status: %d", profile_selector, param.status);
        return;
    }

    ESP_LOGI(COMPONENT_TAG, "Service creation succeeded for profile_selector %d. Status: %d", profile_selector, param.status);
    struct gatt_profile_definition* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        return;
    }

    profile->handle = param.service_handle; // Now we know what the handle associated with this profile is, keep it so other places can use it.
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gatts_start_service(param.service_handle));
    if (profile->characteristics_count == 0)
    {
        ESP_LOGE(COMPONENT_TAG, "Profile %d (profile_selector=%d) does not have any characteristics", profile->index, profile->profile_selector);
        return;
    }

    struct gatt_characteristic_definition* characteristic;
    for (int c = 0; c < profile->characteristics_count; c++)
    {
        characteristic = profile->characteristics_table[c];
        if (!characteristic)
        {
            ESP_LOGE(COMPONENT_TAG, "Characteristic was NULL at position %d, for profile %d (selector=%d)", c, profile->index, profile->profile_selector);
            continue;
        }

        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gatts_add_char(
            profile->handle,
            &(characteristic->id),
            characteristic->permissions,
            characteristic->properties,
            &(characteristic->value),
            NULL));
    }
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_EVT (after it had added a characteristic to a profile).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_characteristic_created(esp_gatt_if_t profile_selector, struct gatts_add_char_evt_param param)
{
    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Characteristic creation failed for profile_selector %d. Status: %d", profile_selector, param.status);
        return;
    }

    struct gatt_profile_definition* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        return;
    }

    struct gatt_characteristic_definition* characteristic = get_characteristic_by_uuid(profile, param.char_uuid);
    if (!characteristic)
    {
        return;
    }

    characteristic->handle = param.attr_handle;
}

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
