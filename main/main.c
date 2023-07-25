#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"
#include "ble_device.h"

// Helper function containing the code for initializing the NVS (non volatile storage)
// The NVS is used by the bluetooth stack to preserver connection information, so it
// survives restarts (otherwise we would have to go through the connect procedure on each restart).
// Note: I did not create a component for this, since I don't have any other functionality than
//    initializing the NVS at the moment.
static void InitializeNonVolatileMemory(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        // Documentation link: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html
        // There always have to be an empty page available, because the mechanism of moving a page is:
        // 1. Copy the data from the page to be moved to the destination
        // 2. Erase the source page.
        // This is needed to safeguard against a power loss happening during the move operation.
        // When there is no page free ESP_ERR_NVS_NO_FREE_PAGES is returned.
        // This can also be returned if the NVS partition was truncated.
        
        // Or (ESP_ERR_NVS_NEW_VERSION_FOUND) the NVS partition contains data in a different format and
        // it cannot be recognised by the current code
        
        // In both cases we erase the flash and try to initialize it again:
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}

// **************************************************************************************************
// ** This is the entry point of the firmware. This executes when the device is powered on **********
// **************************************************************************************************
void app_main(void)
{
    InitializeNonVolatileMemory();
    InitializeBleDevice();
}
