/// @file height_characteristic.c
/// @brief This file contains the definition of the characteristic that returns the current height of the router.
#include "height_characteristic.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#define COMPONENT_TAG "HEIGHT_CHARACTERISTIC"

/// @brief This is the characteristic that returns the current router height.
/// @note This is NOT defined as a const because we set some properties on it once it is successfully registered.
struct gatt_characteristic_definition height_characteristic =
{
    .id =  
    {
        .len = ESP_UUID_LEN_16, 
        .uuid = { .uuid16 = 0xFF01, },
    },
};