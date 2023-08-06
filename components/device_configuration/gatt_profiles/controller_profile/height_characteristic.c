/// @file height_characteristic.c
/// @brief This file contains the definition of the characteristic that returns the current height of the router.
#include "height_characteristic.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#define COMPONENT_TAG "HEIGHT_CHARACTERISTIC"

static uint8_t initialValue[] = { 0x0A, 0x0B };

static uint8_t property_description_text[] = { 'T', 'h', 'i', 's', ' ', 't', 'e', 's', 't' };

static struct gatt_characteristic_descriptor_definition property_description=
{
    .id =  
    {
        .len = ESP_UUID_LEN_16, 
        .uuid = { .uuid16 = 0xFF01, },
    },
    .permissions = ESP_GATT_PERM_READ,
    .value =
    {
        .attr_len = sizeof(property_description_text),
        .attr_value = property_description_text,
    }
};

/// @brief This is the characteristic that returns the current router height.
/// @note This is NOT defined as a const because we set some properties on it once it is successfully registered.
struct gatt_characteristic_definition height_characteristic =
{
    .id =  
    {
        .len = ESP_UUID_LEN_16, 
        .uuid = { .uuid16 = 0xFF01, },
    },
    .properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
    .permissions = ESP_GATT_PERM_READ,
    .value =
    {
        .attr_len = sizeof(initialValue),
        .attr_value = initialValue,
    },
    .descriptors_count = 1,
    .descriptors_table = 
    {
        &property_description,
    }
};