/// @file height_characteristic.h
/// @brief This file contains the reference of the characteristic that returns the current router height.
#pragma once
#include "ble_gatt_server_infra.h"

/// @brief This is the characteristic that returns the current router height.
/// @note This is NOT defined as a const because we set some properties on it once it is successfully registered.
extern struct gatt_characteristic_definition height_characteristic;