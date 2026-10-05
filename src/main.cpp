#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include "conf.h"

#include "LVGL_Task.hpp"
#include "LED_Task.hpp"
#include "Button_Task.hpp"
#include "GPS_Task.hpp"
#include "VESC_Task.hpp"

TaskHandle_t GPS_Task_Handle;
TaskHandle_t LED_Task_Handle;
TaskHandle_t Button_Task_Handle;
TaskHandle_t LVGL_Task_Handle;
TaskHandle_t VESC_Task_Handle;

SemaphoreHandle_t VESC_Data_Mutex;
SemaphoreHandle_t GPS_Data_Mutex;

gps_data GPS_Data;
vesc_data VESC_Data = {0};

unsigned int currentTrack = 0;

bool vesc_connected = false;

uint32_t last_clock_sync = 0;

unsigned int lap_time_seconds = 0;

unsigned int laps_amount = 0;

uint32_t total_distance_travelled = 0; // m

float current_en_eff_wh_lap = 0.0f;

void setup()
{
	Serial.begin(115200);

	setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3", 1);
	tzset();

	GPS_Data_Mutex = xSemaphoreCreateMutex();
	VESC_Data_Mutex = xSemaphoreCreateMutex();

	xTaskCreate(LED_Task, "LED Task", LED_TASK_STACK, NULL, 1, &LED_Task_Handle); // LED NOT BOUND

	xTaskCreate(Button_Task, "BUTTON Task", BUTTON_TASK_STACK, NULL, 1, &Button_Task_Handle); // BUTTON NOT BOUND

	xTaskCreatePinnedToCore(LVGL_Task, "LVGL Task", LVGL_TASK_STACK, NULL, 2, &LVGL_Task_Handle, 1); // LVGL CORE 1

	xTaskCreatePinnedToCore(GPS_Task, "GPS Task", GPS_TASK_STACK, NULL, 3, &GPS_Task_Handle, 0); // GPS CORE 0

	xTaskCreatePinnedToCore(VESC_Task, "VESC CAN Task", VESC_TASK_STACK, NULL, 4, &VESC_Task_Handle, 0); // VESC VORE 0
}

void loop()
{
	vTaskDelete(NULL);
}
