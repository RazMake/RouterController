#include "bluetooth_server.h"
#include "gap_handling.h"
#include "gatt_handling.h"

// This method encapsulates all the logic for initializing the bluetooth stack for
// a server application (phone can connect to it and get information and/or send commands
// using BLE protocol only).
void IntializeBleServer(void)
{
    // Create bluetooth controller configuration structure with default settings:
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    // Then initialize the controller with those settings:
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    ESP_ERROR_CHECK(ret);

    // Documentation: https://my-esp-idf.readthedocs.io/en/stable/api-reference/bluetooth/controller_vhci.html
    // Once the controller is initialize, it can be turned on.
    // The available modes are:
    //   - ESP_BT_MODE_IDLE: Bluetooth not running
    //   - ESP_BT_MODE_BLE: BLE mode
    //   - ESP_BT_MODE_CLASSIC_BT: BT Classic mode
    //   - ESP_BT_MODE_BTDM: Dual mode (BLE + BT Classic)
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    ESP_ERROR_CHECK(ret);

    // Then initialize the bluetooth stack (bluedroid in this case):
    ret = esp_bluedroid_init();
    ESP_ERROR_CHECK(ret);
    ret = esp_bluedroid_enable();
    ESP_ERROR_CHECK(ret);

    // The bluetooth stack is now initialized, but the functionality of the application has not
    // yet been defined. We do that by registering the two handlers next:
    // For GAP (Generic Access Profile) events.
    // These events are received during the connection procedure (Generic info: https://learn.adafruit.com/introduction-to-bluetooth-low-energy/gap)
    esp_ble_gap_register_callback(gap_event_handler);
    // And for GATT (Generic Attribute Profile) events.
    // This is the mechanism for communicating with a bluetooth device (in BLE mode) (Generic info: https://learn.adafruit.com/introduction-to-bluetooth-low-energy/gatt)
    esp_ble_gatts_register_callback(gatt_event_handler);
}
