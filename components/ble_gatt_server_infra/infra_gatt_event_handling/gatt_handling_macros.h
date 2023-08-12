#pragma once
#include "esp_log.h"

#if defined(__cplusplus) && (__cplusplus >  201703L)
    #define ASSERT_STATUS_OK(status, errorMessage, ...) if (status != ESP_GATT_OK) { ESP_LOGE(COMPONENT_TAG, errorMessage, __VA_OPT__(,) __VA_ARGS__); abort(); }
    #define ASSERT_VALID_PROFILE_INDEX(index, errorMessage, ...) if (index >= gatt_profiles_count) { ESP_LOGE(COMPONENT_TAG, errorMessage, __VA_OPT__(,) __VA_ARGS__); abort(); }
    #define ASSERT_EXPECTED_PROFILE_BEING_REGISTERED(index, errorMessage, ...) if (index != profile_index) { ESP_LOGE(COMPONENT_TAG, errorMessage, __VA_OPT__(,) __VA_ARGS__); abort(); }
    #define ASSERT_NOT_NULL(obj, errorMessage, ...) if (!obj) { ESP_LOGE(COMPONENT_TAG, errorMessage, __VA_OPT__(,) __VA_ARGS__); abort(); }
#else
    /// @brief Checks if the status is success, and if it is not, logs the specified message as an error and aborts the program
    #define ASSERT_STATUS_OK(status, errorMessage, ...) if (status != ESP_GATT_OK) { ESP_LOGE(COMPONENT_TAG, errorMessage, ##__VA_ARGS__); abort(); }

    /// @brief Checks if the profile index comming in through the parameter is valid (within the bounds of the gatt_profiles_table array)
    #define ASSERT_VALID_PROFILE_INDEX(index, errorMessage, ...) if (index >= gatt_profiles_count) { ESP_LOGE(COMPONENT_TAG, errorMessage, ##__VA_ARGS__); abort(); }

    /// @brief Checks if the characteristic index is valid (within the bounds of the profile->characteristics_table array)
    #define ASSERT_VALID_CHARACTERISTIC_INDEX(profile, index, errorMessage, ...) if (index >= profile->characteristics_count) { ESP_LOGE(COMPONENT_TAG, errorMessage, ##__VA_ARGS__); abort(); }

    /// @brief Checks if we start registration of a profile before we finished registering the previous
    #define ASSERT_EXPECTED_PROFILE_BEING_REGISTERED(index, errorMessage, ...) if (index != profile_index) { ESP_LOGE(COMPONENT_TAG, errorMessage, ##__VA_ARGS__); abort(); }

    /// @brief Checks if the specified value is NOT NULL, and if it is aborts the program
    #define ASSERT_NOT_NULL(obj, errorMessage, ...) if (!obj) { ESP_LOGE(COMPONENT_TAG, errorMessage, ##__VA_ARGS__); abort(); }
#endif

/// @brief Checks if the specified value is NOT NULL, and if it is aborts the program
#define ASSERT_PROFILE_HAS_AT_LEAST_ONE_CHARACTERISTIC(profile) if (profile->characteristics_count == 0) { ESP_LOGE(COMPONENT_TAG, "Profile with index=%d is incorrectly defined: It does not have any characteristics", profile->index); abort(); }
