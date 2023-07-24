idf_component_register(
    SRCS
        "ble_advertising_data.c"
        "ble_advertising_params.c"
        "ble_routercontroller_profile.c"
    INCLUDE_DIRS
        "include"
    REQUIRES bt
)