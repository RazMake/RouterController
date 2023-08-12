#include <string.h>
#include "gatt_read.h"
#include "ble_gatt_server_infra.h"
#include "gatt_handling_macros.h"
#include "esp_log.h"
#define COMPONENT_TAG "GATT_READ"

/// @brief This event handler is invoked when a request is received for reading the value of a characteristic.
///   By the time the handler is invoked, the profile, characteristic and param are all valid (non NULL).
/// @param profile The profile whose characteristic value is being read.
/// @param characteristic  The characteristic whose value is being read.
/// @param param The event parameters.
static void on_characteristic_value_read(ble_gatt_profile_t* profile, ble_gatt_characteristic_t* characteristic, struct gatts_read_evt_param param)
{
    ESP_LOGI(
        COMPONENT_TAG,
        "Reading characteristic with index=%d from profile with index=%d (conn_id=%d, trans_id %" PRIu32 ")",
        characteristic->index,
        profile->index,
        param.conn_id,
        param.trans_id);
    esp_gatt_rsp_t response;
    memset(&response, 0, sizeof(esp_gatt_rsp_t));

    response.handle = characteristic->handle,
    response.attr_value.len = characteristic->value.attr_len;
    for (int i=0; i<characteristic->value.attr_len; i++)
    {
        response.attr_value.value[i] = characteristic->value.attr_value[i];
    }

    esp_ble_gatts_send_response(profile->profile_selector, param.conn_id, param.trans_id, ESP_GATT_OK, &response);
}

/// @brief This event handler is invoked when a request is received for reading the value of a descriptor.
///   By the time the handler is invoked, the profile, characteristic, descriptor and param are all valid (non NULL).
/// @param profile The profile whose characteristic descriptor value is being read.
/// @param characteristic  The characteristic whose descriptor value is being read.
/// @param descriptor The descriptor whose value is being read.
/// @param param The event parameters.
static void on_characteristic_descriptor_value_read(ble_gatt_profile_t* profile, ble_gatt_characteristic_t* characteristic, ble_gatt_descriptor_t* descriptor, struct gatts_read_evt_param param)
{
    ESP_LOGI(
        COMPONENT_TAG,
        "Reading descriptor with index=%d of characteristic with index=%d from profile with index=%d (conn_id=%d, trans_id %" PRIu32 ")",
        descriptor->index,
        characteristic->index,
        profile->index,
        param.conn_id,
        param.trans_id);

    esp_gatt_rsp_t response;
    memset(&response, 0, sizeof(esp_gatt_rsp_t));

    response.handle = descriptor->handle,
    response.attr_value.len = descriptor->value.attr_len;
    for (int i=0; i<descriptor->value.attr_len; i++)
    {
        response.attr_value.value[i] = descriptor->value.attr_value[i];
    }

    esp_ble_gatts_send_response(profile->profile_selector, param.conn_id, param.trans_id, ESP_GATT_OK, &response);
}

/// @brief This method is called when the ESP infrastructure receives the ESP_GATTS_READ_EVT (the connected devices wants to read the respective
///    characteristic value)
/// @param profile_selector This is an identifier assigned by the ESP infrastructure to the profile, when it is registered.
/// @param param The parameters of the received event.
void on_value_read(esp_gatt_if_t profile_selector, struct gatts_read_evt_param param)
{
    ble_gatt_profile_t* profile = get_profile_by_selector(profile_selector);
    ASSERT_NOT_NULL(profile, "Value read failed: There is no profile with selector=%d", profile_selector);

    // Find the characteristic or the descriptor being read (by its handle):
    ble_gatt_characteristic_t* characteristic;
    ble_gatt_descriptor_t* descriptor;
    for (int c=0; c<profile->characteristics_count; c++)
    {
        characteristic = profile->characteristics_table[c];
        if (characteristic->handle == param.handle)
        {
            on_characteristic_value_read(profile, characteristic, param);
            return;
        }

        for (int d=0; d<characteristic->descriptors_count; d++)
        {
            descriptor = characteristic->descriptors_table[d];
            if (descriptor->handle == param.handle)
            {
                on_characteristic_descriptor_value_read(profile, characteristic, descriptor, param);
                return;
            }
        }
    }
    ESP_LOGE(
        COMPONENT_TAG,
        "Read failed: Handle 0x%x did not match any characteristic or descriptor within profile with index=%d (conn_id=%d, trans_id %" PRIu32 ")",
        param.handle,
        profile->index,
        param.conn_id,
        param.trans_id);
}

