#include <stdio.h>
#include <limero.h>
#include <Log.h>
#include <Sys.h>
#include <LedBlinker.h>
#include "espnow.h"
#include "espnow_storage.h"
#include "espnow_utils.h"
#include "esp_wifi.h"
#include <vector>
#include <string>

typedef std::vector<uint8_t> Bytes;
typedef std::string String;

#define PIN_LED 2

#define BT1 "30:ae:a4:ff:37:08"
#define BT2 "30:ae:a4:0f:38:b0"

#undef ESPNOW_INIT_CONFIG_DEFAULT
#define ESPNOW_INIT_CONFIG_DEFAULT()             \
    {                                            \
        .pmk = "ESP_NOW",                        \
        .forward_enable = 1,                     \
        .forward_switch_channel = 0,             \
        .sec_enable = 0,                         \
        .reverse = 0,                            \
        .qsize = 32,                             \
        .send_retry_num = 10,                    \
        .send_max_timeout = pdMS_TO_TICKS(3000), \
        .receive_enable = {                      \
            .ack = 1,                            \
            .forward = 1,                        \
            .group = 1,                          \
            .provisoning = 0,                    \
            .control_bind = 0,                   \
            .control_data = 0,                   \
            .ota_status = 0,                     \
            .ota_data = 0,                       \
            .debug_log = 0,                      \
            .debug_command = 0,                  \
            .data = 0,                           \
            .sec_status = 0,                     \
            .sec = 0,                            \
            .sec_data = 0,                       \
            .reserved = 0,                       \
        },                                       \
    }

Thread mainThread("main");
LedBlinker led(mainThread, PIN_LED, 10);
TimerSource timer(mainThread, 1000,true);
Log logger;

String hexToMac(uint8_t *mac_addr)
{
    char mac_str[18] = {0};
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    return String(mac_str);
}

void wifi_init()
{
    INFO("wifi_init_sta");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
//    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_event_loop_create_default();
    INFO("wifi_init_sta finished.");
}

void checkEspRc(esp_err_t rc)
{
    if (rc != ESP_OK)
    {
        ERROR("esp rc: %d", rc);
    }
}

class EspNow : public Actor
{

public:
    EspNow(Thread &thr) : Actor(thr) {}
    void init()
    {
        INFO("init");
        espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
        espnow_init(&espnow_config);
        INFO("init done");
    }

    Bytes mac2hex(String mac)
    {
        Bytes macBytes;
        int j = 0;
        char dummy[3] = {0, 0, 0};
        char *ptr;
        for (int k = 0; k < 6; k++)
        {
            for (int i = 0; i < 2; i++)
                dummy[i] = mac[i + j];
            int num = strtol(dummy, &ptr, 16);
            j = j + 3;
            macBytes.push_back(num);
        }
        INFO("mac %s -> %s", mac.c_str(), hexToMac(macBytes.data()).c_str());
        return macBytes;
    }

    void addPeer(const char *mac)
    {
        Bytes macBytes = mac2hex(mac);
        esp_now_peer_info_t peerInfo;
        bzero(&peerInfo, sizeof(peerInfo));
        memcpy(peerInfo.peer_addr, macBytes.data(), macBytes.size());
        esp_now_add_peer(&peerInfo);
    }

    void send(const char *mac, Bytes &data)
    {
        Bytes macBytes = mac2hex(mac);

        checkEspRc(esp_now_send(macBytes.data(), data.data(), data.size()));
    }

    void onRecv(esp_now_recv_cb_t cb)
    {
        checkEspRc(esp_now_register_recv_cb(cb));
    }
};

extern "C" void app_main(void)
{
    espnow_storage_init();

    wifi_init();
    led.init();
    led.interval(10);
    led.mode(LedBlinker::Mode::PULSE);
    timer.start();

    EspNow espNow(mainThread);
    espNow.init();
    espNow.addPeer(BT1);
    espNow.onRecv([](const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
                  {
        INFO("data [%d] from %s", data_len, hexToMac(esp_now_info->src_addr).c_str());
        led.pulse(); });
    timer >> [&](const TimerMsg &tm)
    {
        INFO("send -------------------> ");
        String hello = "helloHello";
        Bytes msg = Bytes(hello.c_str(), hello.c_str() + hello.size());
  //      espNow.send(BT1, msg);
    };

    mainThread.run();
}
