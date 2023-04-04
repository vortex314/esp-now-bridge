# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(WIFI_SSID $ENV{WIFI_SSID}) # copy from env
set(WIFI_PASS $ENV{WIFI_PASS})
set(ENV{IDF_CCACHE_ENABLE} 1)
set(ENV{ESPPORT} /dev/ttyUSB0)
set(ENV{ESPBAUD} 460800)

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)

set(EXTRA_COMPONENT_DIRS /home/lieven/workspace/limero/esp32_idf)
set(EXTRA_COMPONENT_INC /home/lieven/workspace/limero/inc)
set(LIMERO /home/lieven/workspace/limero )
set(ARDUINOJSON /home/lieven/workspace/ArduinoJson )
set(IDF_PATH  /home/lieven/esp/esp-idf)

set(TARGET MAIN_BATTERY) # Battery capacity tester

# set(TARGET MAIN_I2C_SCANNER) # SCAN I2C BUS for devices
# set(TARGET MAIN_AS5600_TEST) # AS5600 I2C test to evaluate the AS5600 module

include_directories( main 
    ${LIMERO}/inc  
    ${ARDUINOJSON}/src
    ${IDF_PATH}/components/freertos/include/freertos
     )

     

#set ( CPPFLAGS   -I$(WORKSPACE)/ArduinoJson/src -I $(IDF_PATH)/components/freertos/include/freertos  -I$(WORKSPACE)/limero/inc )
     
set ( CXXFLAGS   -fno-rtti -ffunction-sections -fdata-sections -std=gnu++11 -fno-exceptions -lstdc++ -faligned-new )

add_definitions(
    -D${TARGET}
    -DWIFI_PASS=${WIFI_PASS} 
    -DWIFI_SSID=${WIFI_SSID} 
    -DMQTT_HOST=limero.ddns.net 
    -DMQTT_PORT=1883
    -DESP32_IDF 
    -DSERIAL_BAUD=460800
    -DHOSTNAME=esp32
    -ffunction-sections 
    -fdata-sections 
    -Wl,--gc-sections
    -Werror=switch
    ) # was no-exceptions 

project(esp-now-bridge)

                    