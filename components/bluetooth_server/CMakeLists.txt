idf_component_register(SRCS "gatt_profiles.c" "gatt_handling.c" "gap_handling.c" "bluetooth_server.c"
                    INCLUDE_DIRS "include"
                    REQUIRES bt)
# Notes:
# 1. The list of components can be found here: https://github.com/espressif/esp-idf/tree/master/components
#    In other words, if you know the header you want to inclue, look it up in git and walk the tree up to the
#    direct child of components folder. That is the name of the component.
# 2. When your component depends on other components you have to add them as REQUIRES.
# 3. If with the component added as requires, the program still does not build, make sure you enabled bluetooth
#    in sdkconfig (or using menuconfig command).
