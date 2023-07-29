#include "esp_log.h"
#include "nvs_flash.h"
#include "ble_gatt_server_infra.h"

void app_main(void)
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

    initialize_ble_gatt_server();
}