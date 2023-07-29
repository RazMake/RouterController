/// @file ble_advertising_configuration.c
/// @brief This file contains the configuration values for BLE advertising
///   It fills in the types required by the GATT server library.
#include "ble_gatt_server_infra.h"

/// @brief The name of the device, which will be present in the advertising data.
const char *ble_device_name = "Router Controller";

// The service UUID advertised by this device.
// The array contains 16 bit or 32 bit UUID, but they are all represented as 16 bytes arrays.
// Basically (https://stackoverflow.com/questions/36212020/how-can-i-convert-a-bluetooth-16-bit-service-uuid-into-a-128-bit-uuid):
// The Xes are the 16 bit UUID: 0000xxxx-0000-1000-8000-00805F9B34FB
//       - formula: 128_bit_value = 16_bit_value * 2^96 + Bluetooth_Base_UUID
// The Xes are the 32 bit UUID: xxxxxxxx-0000-1000-8000-00805F9B34FB
//       - formula: 128_bit_value = 32_bit_value * 2^96 + Bluetooth_Base_UUID
// Where Bluetooth_Base_UUID = 00000000-0000-1000-8000-00805F9B34FB.
static uint8_t advertised_serviceUUID[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x95, 0x4e, 0x37, 0x1d,
};

// This defines the kind of device this is (https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned_Numbers.pdf)
// Generic Motorized Device:
//   - Category bits [15-6] = 0x01D = Motorized Device
//   - Subcategory bits [5-0] = 0x00 = Generic
static const int appearance = 0x0740;

// The BLE advertising parameters used by the current device
// Note: This variable is defined/required by the ble_gatt_server_infra.ble_advertising_infra component.
esp_ble_adv_params_t ble_advertising_parameters =
{
    // Minimum advertising interval for
    //       undirected
    //          and
    //       low duty cycle directed advertising
    // Range: 0x20-0x4000
    // Default value = 0x800 (1.28 second) => value multiplier = 0.625 milliseconds
    // Which means the range is 20 ms to 10.24 sec.
    .adv_int_min = 0x20, // 20 ms

    // Maximum advertising interval for
    //       undirected
    //          and
    //       low duty cycle directed advertising
    // Same default and value multiplier as adv_int_min
    .adv_int_max = 0x40, // 40 ms
    
    // Advertising type (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv418esp_ble_adv_type_t)
    // ADV_TYPE_IND = generic, not directed to a particular central device and connectable advertisment.
    // ADV_TYPE_DIRECT_IND_HIGH
    // ADV_TYPE_SCAN_IND
    // ADV_TYPE_NONCONN_IND
    // ADV_TYPE_DIRECT_IND_LOW
    .adv_type = ADV_TYPE_IND,

    // Owner bluetooth device address type (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_bt_defs.html#_CPPv419esp_ble_addr_type_t)
    // BLE_WL_ADDR_TYPE_PUBLIC
    // BLE_WL_ADDR_TYPE_RANDOM
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,

    // Advertising channel mask (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv421esp_ble_adv_channel_t)
    // ADV_CHNL_37
    // ADV_CHNL_38
    // ADV_CHNL_39
    // ADV_CHNL_ALL
    .channel_map = ADV_CHNL_ALL,
    
    // Advertising filter policy (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_filter_t)
    // ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = Allow both scan req from White List devices only and connection req from anyone.
    // ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = Allow both scan req from White List devices only and connection req from anyone.
    // ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = Allow both scan req from anyone and connection req from White List devices only.
    // ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = Allow scan and connection requests from White List devices only.
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// This field is used to build a default advertising packet, to be sent out at regular interval
// by the current device.
// About advertising:
//       https://www.youtube.com/watch?v=KEXRiOYCHCo
// The advertising process: https://academy.nordicsemi.com/topic/advertising-process/
esp_ble_adv_data_t ble_advertising_data =
{
    // A value indicating whether this advertising data instance is a scan response or not.
    .set_scan_rsp = false,

    // A value indicating whether this advertising data instance includes the device name or not.
    // If the value is true, the name of the device will be in ???????
    .include_name = true,

    // Avalue indicating whether this advertising data instance include transimission power 
    .include_txpower = true,

    // The device's preferred connection min interval.
    // Allowed values are 0x0006 to 0x0C80
    //     - the value in milliseconds is calculated by multiplying with 1.25 the numberic value in this field.
    // And 0xFFFF - spcial meaning constant. It means NOT SPECIFIED.
    .min_interval = 0x0006,

    // The device's preferred connection max interval.
    // Allowed values are 0x0006 to 0x0C80
    //      - the value in milliseconds is calculated by multiplying with 1.25 the numberic value in this field.
    // And 0xFFFF - spcial meaning constant. It means NOT SPECIFIED.
    .max_interval = 0x0010,

    // This defines the kind of device this is
    // (https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned_Numbers.pdf)
    .appearance = appearance,

    // The length of the manufacturer data byte array.
    .manufacturer_len = 0,

    // The manufacturer data byte array.
    .p_manufacturer_data =  NULL,

    // The length of the service data byte array.  
    .service_data_len = 0,

    // The service data byte array.
    .p_service_data = NULL,

    // The size of the p_service_uuid value.
    // According to this comment: https://github.com/espressif/esp-idf/issues/1090
    // and to the Espressif example (https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/gatt_server/main/gatts_demo.c#L86),
    // This is the number of bytes in the array.
    .service_uuid_len = sizeof(advertised_serviceUUID),

    // The service UUID value. The array has a single value now (see the serviceUUID definition above)
    .p_service_uuid = advertised_serviceUUID,

    // This value defines the advertising mode of the device. Defined values available:
    // ESP_BLE_ADV_FLAG_LIMIT_DISC = Limited Discoverable Mode
    // ESP_BLE_ADV_FLAG_GEN_DISC = General Discoverable Mode
    // ESP_BLE_ADV_FLAG_BREDR_NOT_SPT = BR/EDR (basic rate/enhanced data rate aka. classic bluetooth) not supported
    // ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT = Simultaneous LE and BR/EDR, controller
    // ESP_BLE_ADV_FLAG_DMT_HOST_SPT = Simultaneous LE and BR/EDR, host.
    // ESP_BLE_ADV_FLAG_NON_LIMIT_DISC = Not Set ? (0x0)
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// This field is used to build the default scan response packet, to be sent when the device
// receives a scan request.
// Note: this is the same data type as the ble_advertising_data, but it has possibly some different
//  values, one in particular must always be different: set_scan_rsp (should be true).
esp_ble_adv_data_t ble_scan_response_data =
{
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .appearance = appearance,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(advertised_serviceUUID),
    .p_service_uuid = advertised_serviceUUID,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
