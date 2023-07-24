#include "ble_config.h"

/*
    Advertising primer:
        Part 1: https://www.argenox.com/library/bluetooth-low-energy/ble-advertising-primer/
        Part 2: https://www.argenox.com/library/bluetooth-low-energy/designing-ble-advertising-packets/
*/

// The Service UUID specific to this device.
uint8_t serviceUUID[] = { 0xd8, 0x8e, 0x2e, 0x2c, 0x90, 0x6b, 0x46, 0x6f, 0x86, 0x47, 0x89, 0x52, 0x7b, 0x26, 0x0c, 0xc8 };

// The  advertising data emitted by this device.
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

    // This defines the kind of device this is (https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned_Numbers.pdf)
    // Generic Motorized Device:
    //   - Category bits [15-6] = 0x01D = Motorized Device
    //   - Subcategory bits [5-0] = 0x00 = Generic
    .appearance = 0x0740,

    // The length of the manufacturer data byte array.
    .manufacturer_len = 0,

    // The manufacturer data byte array.
    .p_manufacturer_data =  NULL,

    // The length of the service data byte array.  
    .service_data_len = 0,

    // The service data byte array.
    .p_service_data = NULL,

    // The length of the service UUID (16, 32, 64, 128)
    .service_uuid_len = sizeof(serviceUUID) / sizeof(uint8_t),

    // The service UUID value.
    .p_service_uuid = serviceUUID,

    // This value defines the advertising mode of the device. Defined values available:
    // ESP_BLE_ADV_FLAG_LIMIT_DISC = Limited Discoverable Mode
    // ESP_BLE_ADV_FLAG_GEN_DISC = General Discoverable Mode
    // ESP_BLE_ADV_FLAG_BREDR_NOT_SPT = BR/EDR (basic rate/enhanced data rate aka. classic bluetooth) not supported
    // ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT = Simultaneous LE and BR/EDR, controller
    // ESP_BLE_ADV_FLAG_DMT_HOST_SPT = Simultaneous LE and BR/EDR, host.
    // ESP_BLE_ADV_FLAG_NON_LIMIT_DISC = Not Set ? (0x0)
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};