/// @file controller_profile.h
/// @brief This file contains the reference of the profile containing the router control commands.
#pragma once
#include "ble_gatt_server_infra.h"
#include "height_characteristic.h"

/// @brief This is the profile that contains the router controls.
/// @note This is NOT defined as a const because we set some properties on it once it is successfully registered.
extern ble_gatt_profile_t controller_profile;