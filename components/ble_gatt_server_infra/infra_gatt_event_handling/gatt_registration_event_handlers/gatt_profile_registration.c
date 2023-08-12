#include "gatt_profile_registration.h"
#include "ble_gatt_server_infra.h"
#include "gatt_handling_macros.h"
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

#define begin_adding_characteristics() add_characteristic(0);
/// @brief Checks if we finished adding characteristics and if we did not adds the next one in the list.
static void add_characteristic(uint8_t index)
{
    ASSERT_VALID_PROFILE_INDEX(
        profile_index,
        "Registration failed for characteristic with index %d: Invalid parent profile index provided: %d",
        index,
        profile_index);
    ble_gatt_profile_t* profile = gatt_profiles_table[profile_index];
    ASSERT_NOT_NULL(profile, "Registration failed for characteristic with index %d: The parent profile (index=%d) is NULL", index, profile_index);

    if (index == profile->characteristics_count)
    {
        ESP_LOGI(COMPONENT_TAG, "Profile with index=%d registration completed successfully", profile->index);
        return;
    }

    characteristic_index = index;
    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
    characteristic->index = characteristic_index;
    ASSERT_NOT_NULL(characteristic, "Registration failed for characteristic with index %d: The parent profile (index=%d) has NULL for this characteristic", index, profile->index);

    ESP_LOGI(COMPONENT_TAG, "Adding characteristic with index=%d for profile with index=%d", characteristic_index, profile->index);
    ESP_ERROR_CHECK(esp_ble_gatts_add_char(
        profile->service_handle,
        &(characteristic->id),
        characteristic->permissions,
        characteristic->properties,
        &(characteristic->value),
        NULL));
}

/// @brief This method initiates registration for the profile with the specified index with the BLE runtime infrastructure.
/// @param index The index of the profile to start registering.
///
/// @remarks
///   This is necessary because registering a profile is a multi-step asynchronous operation and the BLE runtime infrastructure
///   is not well enough written (it does not provide the necessary information in the events fired after each step completes).
///   To mitigate this situation we register each profile and its characteristics one at a time, so we implicitly can know
///   which profile and which characteristic we're currently registering (the problem is when registering descriptors for characteristics).
void register_ble_profile(uint8_t index)
{
    if (profile_index >= gatt_profiles_count)
    {
        ESP_LOGI(COMPONENT_TAG, "All %d profiles have been registered successfully", gatt_profiles_count);
        return;
    }

    profile_index = index; // Remember which profile we're currently registering
    ESP_LOGI(COMPONENT_TAG, "Begin registration for profile with index=%d", profile_index);
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(profile_index));
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_REG_EVT (a new profile was registered and we
///   need to create the service for it).
///   This is also where we can associate the profile with the gatt_if (profile_selector) value so we can route the subsequent events correctly.
///   descriptors.
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_profile_registered(esp_gatt_if_t profile_selector, struct gatts_reg_evt_param param)
{
    ASSERT_STATUS_OK(param.status, "Registration failed for profile with index %d: Error code=%d", param.app_id, param.status);
    ASSERT_VALID_PROFILE_INDEX(
        param.app_id,
        "Registration failed for profile with index %d: There is no profile with that index (valid indexes=0..%d). Current profile_index=%d",
        param.app_id,
        gatt_profiles_count,
        profile_index);
    ASSERT_EXPECTED_PROFILE_BEING_REGISTERED(
        param.app_id,
        "Registration failed for profile with index %d: Another profile (with index=%d) was being registered",
        param.app_id,
        profile_index);

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
    ASSERT_STATUS_OK(param.status, "Service creation failed for profile with index %d: Error code=%d", profile_index, param.status);
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    ASSERT_NOT_NULL(profile, "Service creation failed for profile with index %d: There is no profile with selector=%d", profile_index, profile_selector);
    ASSERT_PROFILE_HAS_AT_LEAST_ONE_CHARACTERISTIC(profile);

    // Now we know what the handle associated with this profile is, keep it so other places can use it.
    profile->service_handle = param.service_handle;
    ESP_LOGI(COMPONENT_TAG, "Successfully created the service for profile with index=%d, handle=0x%x", profile->index, profile->service_handle);

    ESP_LOGI(COMPONENT_TAG, "Starting the service for profile with index=%d. Service handle=0x%x", profile->index, param.service_handle);
    ESP_ERROR_CHECK(esp_ble_gatts_start_service(param.service_handle));
    ESP_LOGI(COMPONENT_TAG, "Successfully started service for profile with index=%d. Begin adding its characteristics", profile->index);

    begin_adding_characteristics();
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_EVT (after it had added a characteristic to a profile).
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_characteristic_created(esp_gatt_if_t profile_selector, struct gatts_add_char_evt_param param)
{
    ASSERT_STATUS_OK(param.status, "Adding characteristic with index %d to profile with index %d failed: Error code=%d", characteristic_index, profile_index, param.status);
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    ASSERT_NOT_NULL(
        profile,
        "Adding characteristic with index %d to profile with index %d failed: There is no profile with selector=%d",
        characteristic_index,
        profile_index,
        profile_selector);
    ASSERT_VALID_CHARACTERISTIC_INDEX(
        profile,
        characteristic_index,
        "Adding characteristic with index %d to profile with index %d failed: There is no characteristic with this index (valid index:0..%d)",
        characteristic_index,
        profile_index,
        profile->characteristics_count);
    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
    ASSERT_NOT_NULL(
        characteristic,
        "Adding characteristic with index %d to profile with index %d failed: The parent profile has NULL for this characteristic in the characteristics_table",
        characteristic_index,
        profile_index);

    if (!are_uuids_equal(characteristic->id, param.char_uuid))
    {
        ESP_LOGE(
            COMPONENT_TAG,
            "Adding characteristic with index %d to profile with index %d failed:  The value in param.char_uuid does not match characteristic currently beig added",
            characteristic_index,
            profile_index);
            abort();
    }

    characteristic->handle = param.attr_handle;
    if (characteristic->descriptors_count > 0)
    {
        ESP_LOGI(
            COMPONENT_TAG,
            "Successfully added characteristic with index=%d (to profile with index=%d), handle=0x%x. Adding its %d descriptors now",
            characteristic->index,
            profile->index,
            characteristic->handle,
            characteristic->descriptors_count);
        // Register descriptors for THIS characteristic, before continuing with adding other characteristics
        // (otheriwse the descriptors will be attached to the incorrect characteristic).
        // We can register all descriptors in one go, since they do not have any children, so there is no confusion.
        for(int d=0; d<characteristic->descriptors_count; d++)
        {
            ble_gatt_descriptor_t* descriptor = characteristic->descriptors_table[d];
            descriptor->index = d;
            ESP_LOGI(
                COMPONENT_TAG,
                "Adding descriptor with index=%d for characteristic with index=%d in profile with index=%d",
                d,
                characteristic->index,
                profile->index);
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
        ESP_LOGI(
            COMPONENT_TAG,
            "Successfully created characteristic with index=%d (for profile with index=%d), handle=0x%x. It has no descriptors",
            characteristic->index,
            profile->index,
            characteristic->handle);

        // If the characteristic has no descriptors, continue registering the next characteristic:
        add_characteristic(characteristic_index + 1);
    }
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_ADD_CHAR_DESCR_EVT  ()
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_characteristic_descriptor_added(esp_gatt_if_t profile_selector, struct gatts_add_char_descr_evt_param param)
{
    ASSERT_STATUS_OK(
        param.status,
        "Descriptor adding to characteristic with index %d failed for profile with index %d: Error code=%d",
        characteristic_index,
        profile_index,
        param.status);
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    ASSERT_NOT_NULL(
        profile,
        "Descriptor adding to characteristic with index %d failed for profile with index %d: There is no profile with selector=%d",
        characteristic_index,
        profile_index,
        profile_selector);
    ASSERT_VALID_CHARACTERISTIC_INDEX(
        profile,
        characteristic_index,
        "Descriptor adding to characteristic with index %d failed for profile with index %d: There is no characteristic with this index (valid index:0..%d)",
        characteristic_index,
        profile_index,
        profile->characteristics_count);
    ble_gatt_characteristic_t* characteristic = profile->characteristics_table[characteristic_index];
    ASSERT_NOT_NULL(
        characteristic,
        "Descriptor adding to characteristic with index %d failed for profile with index %d: The parent profile has NULL for this characteristic in the characteristics_table",
        characteristic_index,
        profile_index);

    ble_gatt_descriptor_t* descriptor = get_descriptor_by_uuid(characteristic, param.descr_uuid);
    ASSERT_NOT_NULL(
        descriptor,
        "Descriptor adding to characteristic with index %d failed for profile with index %d: No descriptor matched the currently being added UUID",
        characteristic_index,
        profile_index);

    descriptor->handle = param.attr_handle;
    ESP_LOGI(
        COMPONENT_TAG,
        "Successfully added descriptor with index=%d (for characteristic with index=%d in profile with index=%d), handle=0x%x",
        descriptor->index,
        characteristic->index,
        profile->index,
        descriptor->handle);

    if (are_all_descriptors_registered(characteristic))
    {
        ESP_LOGI(COMPONENT_TAG, "Successfully added all descriptors for characteristic with index=%d from profile with index=%d", characteristic->index, profile->index);
        add_characteristic(characteristic_index + 1);
    }
}
