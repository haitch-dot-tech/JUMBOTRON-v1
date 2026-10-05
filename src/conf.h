#include <Arduino.h>

#define JT_FW_VER_MAJOR "1"
#define JT_FW_VER_MINOR "01"
#define JT_FW_VER_BUILD "0041"

// FreeRTOS
#define LVGL_TASK_STACK 8192
#define LED_TASK_STACK 2048
#define BUTTON_TASK_STACK 2048
#define GPS_TASK_STACK 2048
#define VESC_TASK_STACK 2048

// Hardware
#define LED_PIN 48
#define LED_PERIOD 1000 // ms
#define LED_TIME 10     // ms

#define BLE_ACTIVE 12
#define WiFi_ACTIVE 13

// LVGL
#define TFT_HOR_RES 320
#define TFT_VER_RES 480

#define BUF_LINES 80

#define BOOT_SCREEN_TIME 4000

// GPS
#define GPS_BAUD 9600
#define GPS_TX 10
#define GPS_RX 11

#define SYNC_INTERVAL 5 * 60 * 1000

// CAN
#define CAN_RX 8
#define CAN_TX 9
