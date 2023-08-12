#include "gatt_profile_registration.h"
#include "ble_gatt_server_infra.h"

#include "esp_log.h"
#define COMPONENT_TAG "GATT_PROFILE_REGISTRATION"

/// @brief The index of the profile being currently registered.
///   When this is greater than the gatt_profiles_count value, it means we're done registering profiles,
///   and the device is ready to accept clients.
static uint8_t profile_index = 0;

/// @brief The index of the characteristic being currently registered.
///   When this is greater than the profile's characteristics_count it means we finished adding all the
///   characteristics of this profile.
static uint8_t characteristic_index = 0;

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

/// @brief This method checks if the specified characteristic has all its descriptors registered yet or not.
///   A descriptor gets its 'handle' property set when it is registered, so we're looking for any descriptor with handle = 0.
/// @param characteristic The characteristic to check for unregistered (yet) descriptors.
/// @return True = there is at least one descriptor that is not registered yet, False = otherwise.
static bool are_all_descriptors_registered(ble_gatt_characteristic_t* characteristic)
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

/// @brief Checks if we finished adding characteristics and if we did not adds the next one in the list.
static void add_next_characteristic(void)
{
    ble_gatt_profile_t* profile = gatt_profiles_table[profile_index];
    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
    if (!characteristic)
    {
        ESP_LOGI(COMPONENT_TAG, "Finished registering characteristics (%d) for profile with index=%d", profile->characteristics_count, profile->index);
        return;
    }

    ESP_LOGI(COMPONENT_TAG, "Adding characteristic with index=%d for profile with index=%d", characteristic_index, profile->index);
    ESP_ERROR_CHECK(esp_ble_gatts_add_char(
        profile->service_handle,
        &(characteristic->id),
        characteristic->permissions,
        characteristic->properties,
        &(characteristic->value),
        NULL));
}

/// @brief This method registers the profiles in the order they're defined in the gatt_profiles_table,
///   one by one, so the registration of the characteristics and descriptors does not get mixed up.
///   This is necesary because the registration is spread out over multiple events and the infrastructure
///   does not provide sufficient identification of what is the parent of each object being registered.
void register_ble_profiles(void)
{
    if (profile_index >= gatt_profiles_count)
    {
        ESP_LOGI(COMPONENT_TAG, "All %d profiles have been registered successfully", gatt_profiles_count);
        return;
    }

    ESP_LOGI(COMPONENT_TAG, "Begin profile %d registration", profile_index);
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(profile_index)); // Next time the method is called, it will be for the next profile in the table.
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_REG_EVT (a new profile was registered and we
///   need to create the service for it).
///   This is also where we can associate the profile with the gatt_if (profile_selector) value so we can route the subsequent events correctly.
///   descriptors.
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_profile_registered(esp_gatt_if_t profile_selector, struct gatts_reg_evt_param param)
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

    if (param.app_id != profile_index)
    {
        // This should never happen. If it does, it likely indicates a coding error.
        ESP_LOGE(
            COMPONENT_TAG,
            "Registration check failed. The index of the profile being registered is %d but the event arguments point to profile with index %d",
            profile_index,
            param.app_id);
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
void on_profile_service_created(esp_gatt_if_t profile_selector, struct gatts_create_evt_param param)
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
    ESP_LOGI(COMPONENT_TAG, "Successfully created the service for profile with index=%d, handle=0x%x", profile->index, profile->service_handle);

    ESP_LOGI(
        COMPONENT_TAG,
        "Starting the service for profile with index=%d. Service handle=0x%x",
        profile->index,
        param.service_handle);
    ESP_ERROR_CHECK(esp_ble_gatts_start_service(param.service_handle));

    ESP_LOGI(
        COMPONENT_TAG,
        "Successfully created service for profile with index=%d. Begin adding its characteristics",
        profile->index);

    if (profile->characteristics_count == 0)
    {
        // Since we're just registering/creating this service, it is impossible that at this time we already have all the characteristics registered.
        // The only way we don't find any characteristic to register here is if the service does not have ANY, which is invalid according to the spec.
        ESP_LOGE(COMPONENT_TAG, "Profile with index=%d does not have any characteristics", profile->index);
        abort();
    }

    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
    characteristic->index = characteristic_index;
    ESP_LOGI(COMPONENT_TAG, "Adding characteristic with index=%d for profile with index=%d", characteristic_index, profile->index);
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
void on_characteristic_created(esp_gatt_if_t profile_selector, struct gatts_add_char_evt_param param)
{
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        // This should never happen because we associate the profile_selector with the profile definition in the on_profile_registered(..) event handler.
        // If it somehow does, we cannot continue because we don't know which profile is this event for.
        abort();
    }

    if (characteristic_index >= profile->characteristics_count)
    {
        ESP_LOGE(
            COMPONENT_TAG,
            "Failed to create characteristic for profile with index=%d. There is no characteristic with index=%d (valid index: 0..%d)",
            profile->index,
            characteristic_index,
            profile->characteristics_count);
            abort();
    }

    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
    if (!are_uuids_equal(characteristic->id, param.char_uuid))
    {
        ESP_LOGE(
            COMPONENT_TAG,
            "Failed to create characteristic for profile with index=%d. The value in param.char_uuid does not match characteristic currently beig added (index=%d)",
            profile->index,
            characteristic_index);
            abort();
    }

    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(
            COMPONENT_TAG,
            "Failed to create characteristic with index=%d for profile with index=%d. Error code=%d",
            characteristic->index,
            profile->index,
            param.status);
        abort();
    }

    characteristic->handle = param.attr_handle;
    characteristic->index = characteristic_index;
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
        characteristic_index++;
        add_next_characteristic();
    }
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_DESCR_EVT  ()
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_characteristic_descriptor_added(esp_gatt_if_t profile_selector, struct gatts_add_char_descr_evt_param param)
{
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    if (!profile)
    {
        // This should never happen because we associate the profile_selector with the profile definition in the on_profile_registered(..) event handler.
        // If it somehow does, we cannot continue because we don't know which profile is this event for.
        abort();
    }

    if (characteristic_index >= profile->characteristics_count)
    {
        ESP_LOGE(
            COMPONENT_TAG,
            "Failed to add characteristic descriptor for profile with index=%d. There is no characteristic with index=%d (valid index: 0..%d)",
            profile->index,
            characteristic_index,
            profile->characteristics_count);
            abort();
    }

    if (param.status != ESP_GATT_OK)
    {
        ESP_LOGE(
            COMPONENT_TAG,
            "Failed to create characteristic with index=%d for profile with index=%d. Error code=%d",
            characteristic_index,
            profile->index,
            param.status);
        abort();
    }

    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
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

    if (are_all_descriptors_registered(characteristic))
    {
        ESP_LOGI(COMPONENT_TAG, "Successfully registered all descriptors for characteristic with index=%d from profile with index=%d", characteristic->index, profile->index);
        add_next_characteristic();
    }
}

