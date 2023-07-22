#pragma once

#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#include "gap_handling.h"
#include "gatt_handling.h"

// This method encapsulates all the logic for initializing the bluetooth stack for
// a server application (phone can connect to it and get information and/or send commands
// using BLE protocol only).
void IntializeBleServer(void);
