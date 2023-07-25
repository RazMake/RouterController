#include "ble_config.h"

// Initialy no response is sent.
// This field has bits set for each kind of advertising data sent by the device.
static uint8_t advertising_configured_bits = 0;

// This bit is set when advertising data is sent by the device.
#define advertising_data_configured_bit 0x01

// This bit is set when the scan response data is sent by the device.
#define scan_rsp_configured_bit 0x02

// Sets the state to indicate the scan response was sent.
void set_scanresponse_configured()
{
    advertising_configured_bits |= scan_rsp_configured_bit;
}

// Sets the state to indicate the scan advertising data was sent.
void set_advertisingdata_configured()
{
    advertising_configured_bits |= advertising_data_configured_bit;
}

// Checks if the device is ready to start advertising (the advertising data is configured)
bool should_start_advertising()
{
    advertising_configured_bits &= (~advertising_data_configured_bit);
    return advertising_configured_bits == 0;
}
