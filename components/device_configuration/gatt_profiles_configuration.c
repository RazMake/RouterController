/// @file ble_advertising_configuration.c
/// @brief This file contains the configuration values for BLE advertising
///   It fills in the types required by the GATT server library.
#include "ble_gatt_server_infra.h"
#include "router_controller_profile.h"

/// @brief The number of GATT profiles defined for this device.
const uint8_t gatt_profiles_count = 1;

/// @brief The table containing all the GATT profiles supported by the device.
///
/// Note: This is defined as an array of pointers because I could not find a way to initialize it
///   nicely with instaces of the structs defined in separate files (not inline).
struct gatt_profile_definition *gatt_profiles_table[] =
{
    &router_controller_profile,
};
