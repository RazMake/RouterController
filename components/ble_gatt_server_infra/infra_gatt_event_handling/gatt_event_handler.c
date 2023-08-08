/// @file gap_event_handler.c
/// @brief The implementation of the callback invoked by the ESP infrastructure for all GAP events.
#include <string.h>
#include "gatt_event_handler.h"
#include "esp_log.h"
#define COMPONENT_TAG "GATT_EVENT_HANDLER"

// ------------------------------ Helper functions ----------------------------------------------------------------------------------------------------------------------
/// @brief This is a method that counts the handles needed for this profile.
/// @param profile A pointer to the profile whose handles to count.
/// @returns The calculated number of handles needed by this profile.
static uint16_t get_handles_count(ble_gatt_profile_t* profile)
{
    uint16_t handles_count = 1; // A handle is required by the service of this profile;

    // Add characteristic and descriptor handles, if there are any defined.
    ble_gatt_characteristic_t* characteristic;
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

/// @brief This method searches the next characteristic that is not registered.
/// @param profile The profile whose characteristics are to be registered.
/// @returns NULL = All characteristics of the specified profile were initialized, or the characteristic that must be initialized next.
static ble_gatt_characteristic_t* get_next_characteristic_to_register(ble_gatt_profile_t* profile)
{
    for (int c = 0; c<profile->characteristics_count; c++)
    {
        if (!profile->characteristics_table[c]->descriptors_adding_started)
        {
            return profile->characteristics_table[c];
        }
    }

    if (profile->characteristics_count > 0)
    {
        // This is just to keep logs clean. It is an error to have a profile with no characteristic at all, and that
        // is logged in the on_profile_service_created(..) event handler:
        ESP_LOGI(COMPONENT_TAG, "Finished registering all characteristics of profile with index=%d", profile->index);
    }
    return NULL;
}

/// @brief Starts adding the next characteristic of the specified profile, if there is any one that was not added yet.
/// @param profile The profile whose characteristic to add/register.
static void add_next_characteristic_if_any(ble_gatt_profile_t* profile)
{
    ble_gatt_characteristic_t* characteristic = get_next_characteristic_to_register(profile);
    if (!characteristic)
    {
        ESP_LOGI(COMPONENT_TAG, "Finished registering characteristics (%d) for profile with index=%d", profile->characteristics_count, profile->index);
        return;
    }

    ESP_LOGI(COMPONENT_TAG, "Adding characteristic with index=%d for profile with index=%d", characteristic->index, profile->index);
    ESP_ERROR_CHECK(esp_ble_gatts_add_char(
        profile->service_handle,
        &(characteristic->id),
        characteristic->permissions,
        characteristic->properties,
        &(characteristic->value),
        NULL));
}

/// @brief This method checks if the specified characteristic has all its descriptors registered yet or not.
///   A descriptor gets its 'handle' property set when it is registered, so we're looking for any descriptor with handle = 0.
/// @param characteristic The characteristic to check for unregistered (yet) descriptors.
/// @return True = there is at least one descriptor that is not registered yet, False = otherwise.
static bool has_characteristic_complete_descriptor_registration(ble_gatt_characteristic_t* characteristic)
{
    for (int d=0; d<characteristic->descriptors_count; d++)
    {
        if (characteristic->descriptors_table[d]->handle == 0)
        {
            return false;
        }
    }

    return true;
}

/// @brief This method gets the first characteristic in the specified profile that does not have all its descriptors registered.
/// @param profile The profile whose characteristics to look at.
/// @returns NULL, if all the characteristics in the profile have all their descriptors registered, or the first encountered characteristic
///    which does not have all its descriptors registered yet.
static ble_gatt_characteristic_t* get_characteristic_with_incomplete_descriptor_registration(ble_gatt_profile_t* profile)
{
    for (int c=0; c<profile->characteristics_count; c++)
    {
        if (!has_characteristic_complete_descriptor_registration(profile->characteristics_table[c]))
        {
            return profile->characteristics_table[c];
        }
    }

    return NULL;
}

// ------------------------------ Registration event handlers -----------------------------------------------------------------------------------------------------------
/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_REG_EVT (a new profile was registered and we
///   need to create the service for it).
///   This is also where we can associate the profile with the gatt_if (profile_selector) value so we can route the subsequent events correctly.
///   descriptors.
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_profile_registered(esp_gatt_if_t profile_selector, struct gatts_reg_evt_param param)
{
    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Registration failed for profile with index %d. Error code=%d", param.app_id, param.status);
        abort();
    }

    // The app_id value, is exactly the value passed initially to the esp_ble_gatts_app_register(..) call:
    // the index in the profiles table:
    if (param.app_id >= gatt_profiles_count)
    {
        ESP_LOGE(COMPONENT_TAG, "Registration failed. There is no profile with index %d. Valid indexes=0..%d", param.app_id, gatt_profiles_count);
        abort();
    }

    ble_gatt_profile_t* profile = gatt_profiles_table[param.app_id];
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
    profile->index = param.app_id;

    // Initialize all characteristics in this profile, before we proceed with creating everything:
    for (int c=0; c<profile->characteristics_count; c++)
    {
        profile->characteristics_table[c]->index = c;
        profile->characteristics_table[c]->profile_index = param.app_id;
    }

    ESP_LOGI(
        COMPONENT_TAG,
        "Profile with index %d is registered successfully with profile_selector=%d",
        param.app_id,
        profile_selector);

    ESP_LOGI(
        COMPONENT_TAG,
        "Creating the service for profile with index %d (with %d characteristics). Requesting %d handles",
        param.app_id,
        profile->characteristics_count,
        handles_count);
    esp_ble_gatts_create_service(profile_selector, &service_id, handles_count);
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_CREATE_EVT (for a new profile was created and its
///    characteristics must be added).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_profile_service_created(esp_gatt_if_t profile_selector, struct gatts_create_evt_param param)
{
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        // This should never happen because we associate the profile_selector with the profile definition in the on_profile_registered(..) event handler.
        // If it somehow does, we cannot continue because we don't know which profile is this event for.
        abort();
    }

    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Failed to create the service for profile with index=%d. Error code=%d", profile->index, param.status);
        abort();
    }

    // Now we know what the handle associated with this profile is, keep it so other places can use it.
    profile->service_handle = param.service_handle;
    ESP_LOGI(COMPONENT_TAG, "Successfully created the service for profile with index=%d, handle=0x%x. Retrieving the profile definition for it", profile->index, profile->service_handle);

    ESP_LOGI(
        COMPONENT_TAG,
        "Starting the service for profile with index=%d. Service handle=0x%x",
        profile->index,
        param.service_handle);
    ESP_ERROR_CHECK(esp_ble_gatts_start_service(param.service_handle));

    ESP_LOGI(
        COMPONENT_TAG,
        "Successfully created service for profile with index=%d. Begin creating its characteristics",
        profile->index);
    ble_gatt_characteristic_t* characteristic = get_next_characteristic_to_register(profile);
    if (!characteristic)
    {
        // Since we're just registering/creating this service, it is impossible that at this time we already have all the characteristics registered.
        // The only way we don't find any characteristic to register here is if the service does not have ANY, which is invalid according to the spec.
        ESP_LOGE(COMPONENT_TAG, "Profile with index=%d does not have any characteristics", profile->index);
        abort();
    }

    ESP_LOGI(COMPONENT_TAG, "Adding characteristic with index=%d for profile with index=%d", characteristic->index, profile->index);
    ESP_ERROR_CHECK(esp_ble_gatts_add_char(
        profile->service_handle,
        &(characteristic->id),
        characteristic->permissions,
        characteristic->properties,
        &(characteristic->value),
        NULL));
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_EVT (after it had added a characteristic to a profile).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_characteristic_created(esp_gatt_if_t profile_selector, struct gatts_add_char_evt_param param)
{
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        // This should never happen because we associate the profile_selector with the profile definition in the on_profile_registered(..) event handler.
        // If it somehow does, we cannot continue because we don't know which profile is this event for.
        abort();
    }
    ble_gatt_characteristic_t* characteristic = get_characteristic_by_uuid(profile, param.char_uuid);
    if (!characteristic)
    {
        abort();
    }

    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Failed to create characteristic with index=%d for profile with index=%d. Error code=%d", characteristic->index, profile->index, param.status);
        abort();
    }

    characteristic->handle = param.attr_handle;
    characteristic->descriptors_adding_started = true;

    ESP_LOGI(
        COMPONENT_TAG,
        "Successfully created characteristic with index=%d (for profile with index=%d), handle=0x%x. Adding its %d descriptors now",
        characteristic->index,
        profile->index,
        characteristic->handle,
        characteristic->descriptors_count);
    if (characteristic->descriptors_count > 0)
    {
        // Register descriptors for THIS characteristic, before continuing with adding other characteristics
        // (otheriwse the descriptors will be attached to the incorrect characteristic).
        // We can register all descriptors in one go, since they do not have any children, so there is no confusion.
        for(int d=0; d<characteristic->descriptors_count; d++)
        {
            ble_gatt_descriptor_t* descriptor = characteristic->descriptors_table[d];
            descriptor->index = d;
            descriptor->handle = 0;
            ESP_LOGI(COMPONENT_TAG, "Adding descriptor with index=%d for characteristic with index=%d in profile with index=%d", d, characteristic->index, profile->index);
            ESP_ERROR_CHECK(esp_ble_gatts_add_char_descr(
                profile->service_handle,
                &(descriptor->id),
                descriptor->permissions,
                &(descriptor->value),
                NULL));
        }
    }
    else
    {
        // If the characteristic has no descriptors, continue registering the next characteristic:
        add_next_characteristic_if_any(profile);
    }
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_DESCR_EVT  ()
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_characteristic_descriptor_added(esp_gatt_if_t profile_selector, struct gatts_add_char_descr_evt_param param)
{
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        // This should never happen because we associate the profile_selector with the profile definition in the on_profile_registered(..) event handler.
        // If it somehow does, we cannot continue because we don't know which profile is this event for.
        abort();
    }

    ble_gatt_characteristic_t* characteristic = get_characteristic_with_incomplete_descriptor_registration(profile);
    if (!characteristic)
    {
        // This should never happen, but just in case, I want to see it in the logs:
        ESP_LOGE(COMPONENT_TAG, "Failed to identify which characteristic descriptor belongs to");
        abort();
    }

    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(COMPONENT_TAG, "Failed to create descriptor for characteristic with index=%d in profile with index=%d. Error code=%d", characteristic->index, profile->index, param.status);
        abort();
    }

    ble_gatt_descriptor_t* descriptor = get_descriptor_by_uuid(characteristic, param.descr_uuid);
    if (!descriptor)
    {
        abort();
    }

    descriptor->handle = param.attr_handle;
    ESP_LOGI(
        COMPONENT_TAG,
        "Successfully registered descriptors with index=%d (for characteristic with index=%d from profile with index=%d), handle=0x%x",
        descriptor->index,
        characteristic->index,
        profile->index,
        descriptor->handle);

    if (has_characteristic_complete_descriptor_registration(characteristic))
    {
        ESP_LOGI(COMPONENT_TAG, "Successfully registered all descriptors for characteristic with index=%d from profile with index=%d", characteristic->index, profile->index);
        add_next_characteristic_if_any(profile);
    }
}

// ------------------------------ Read/Write event handlers -------------------------------------------------------------------------------------------------------------
/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_READ_EVT (the connected devices wants to read the respective
///    characteristic value)
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
static void on_characteristic_value_read(esp_gatt_if_t profile_selector, struct gatts_read_evt_param param)
{
    ESP_LOGI(
        COMPONENT_TAG,
        "Characteristic being read (conn_id %d, trans_id %" PRIu32 ", handle %d",
        param.conn_id,
        param.trans_id,
        param.handle);

    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        return;
    }

    ble_gatt_characteristic_t* characteristic = get_characteristic_by_handle(profile, param.handle);
    esp_gatt_rsp_t response;
    memset(&response, 0, sizeof(esp_gatt_rsp_t));

    response.handle = characteristic->handle,
    response.attr_value.len = characteristic->value.attr_len;
    for (int i=0; i<characteristic->value.attr_len; i++)
    {
        response.attr_value.value[i] = characteristic->value.attr_value[i];
    }

    esp_ble_gatts_send_response(profile_selector, param.conn_id, param.trans_id, ESP_GATT_OK, &response);
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
        case ESP_GATTS_READ_EVT: on_characteristic_value_read(profile_selector, param->read); return;
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
