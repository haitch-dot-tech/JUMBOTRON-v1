#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include "conf.h"

#include <lvgl.h>

#include "ui.hpp"

static uint32_t draw_buf[TFT_HOR_RES * BUF_LINES];

uint8_t gps_satellites = 0;

bool motor_sense_connected = true;

uint32_t myTick()
{
    return millis();
}

void LVGL_Task(void *pvParameters)
{
    pinMode(BLE_ACTIVE, INPUT_PULLDOWN);
    pinMode(WiFi_ACTIVE, INPUT_PULLDOWN);

    lv_init();

    lv_tick_set_cb(myTick);

    lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    ui_bootscreen(lv_screen_active());

    lv_timer_handler();

    vTaskDelay(pdMS_TO_TICKS(BOOT_SCREEN_TIME));

    ui_create(lv_screen_active());

    for (;;)
    {
        lv_label_set_text_fmt(top_label, "#FF0000 %s# %s " LV_SYMBOL_GPS " %s# #0082fc %s# #C0C0C0 %s#", (!motor_sense_connected) ? LV_SYMBOL_CLOSE : "", (gps_satellites == 0) ? "#ff7b00" : "#C0C0C0", (gps_satellites == 0) ? "-" : std::to_string(gps_satellites).c_str(), digitalRead(BLE_ACTIVE) ? LV_SYMBOL_BLUETOOTH : "", digitalRead(WiFi_ACTIVE) ? LV_SYMBOL_WIFI : "");

        if (xSemaphoreTake(GPS_Data_Mutex, pdMS_TO_TICKS(10)))
        {
            if (GPS_Data.speed == -1 || GPS_Data.satellites == 0)
            {
                lv_label_set_text(speed_label, "--");
                lv_obj_set_style_text_color(speed_label, lv_color_hex(0xC0C0C0), LV_PART_MAIN);
            }
            else
            {
                lv_label_set_text_fmt(speed_label, "%d", GPS_Data.speed);
                lv_obj_set_style_text_color(speed_label, lv_color_white(), LV_PART_MAIN);
            }

            gps_satellites = GPS_Data.satellites;

            xSemaphoreGive(GPS_Data_Mutex);
        }

        if (last_clock_sync == 0)
        {
            lv_label_set_text(clock_label, "--:--");
            lv_obj_set_style_text_color(clock_label, lv_color_hex(0xC0C0C0), LV_PART_MAIN);
        }
        else
        {
            time_t now = time(nullptr);
            struct tm localTime = {};
            localtime_r(&now, &localTime);

            lv_label_set_text_fmt(clock_label, "%02d:%02d", localTime.tm_hour, localTime.tm_min);
            lv_obj_set_style_text_color(clock_label, lv_color_white(), LV_PART_MAIN);
        }

        lv_label_set_text_fmt(laps_label, "%d", laps_amount);

        if (lap_time_seconds != 0)
        {
            lv_label_set_text_fmt(laptime_label, "%d:%02d", lap_time_seconds / 60, lap_time_seconds % 60);
            lv_obj_set_style_text_color(laptime_label, lv_color_white(), LV_PART_MAIN);
        }
        else
        {
            lv_label_set_text(laptime_label, "-:--");
            lv_obj_set_style_text_color(laptime_label, lv_color_hex(0xC0C0C0), LV_PART_MAIN);
        }

        if (current_en_eff_wh_lap == 0.0f)
        {
            lv_label_set_text(en_eff_label, "N/A");
            lv_obj_set_style_text_color(en_eff_label, lv_color_hex(0xC0C0C0), LV_PART_MAIN);
        }
        // else if (current_en_eff_wh_lap <= 5.0f)
        // {
        //     lv_label_set_text(en_eff_label, "GOOD");
        //     lv_obj_set_style_text_color(en_eff_label, lv_color_hex(0x00FF00), LV_PART_MAIN);
        // }
        // else if (current_en_eff_wh_lap <= 10.0f)
        // {
        //     lv_label_set_text(en_eff_label, "OK");
        //     lv_obj_set_style_text_color(en_eff_label, lv_color_hex(0xFFFF00), LV_PART_MAIN);
        // }
        // else
        // {
        //     lv_label_set_text(en_eff_label, "POOR");
        //     lv_obj_set_style_text_color(en_eff_label, lv_color_hex(0xFF0000), LV_PART_MAIN);
        // }
        else 
        {
            lv_label_set_text_fmt(en_eff_label, "%d", (int)current_en_eff_wh_lap);
            lv_obj_set_style_text_color(en_eff_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        }

        lv_label_set_text_fmt(laps_label, "%d", laps_amount);

        if (xSemaphoreTake(VESC_Data_Mutex, pdMS_TO_TICKS(10)))
        {
            ui_set_battery(VESC_Data.volts_in);

            if (VESC_Data.motor_temp > 0.0f || !vesc_connected)
            {
                motor_sense_connected = true;
            }
            else 
            {
                motor_sense_connected = false;
            }

            xSemaphoreGive(VESC_Data_Mutex);
        }

        // Receive any notification from the button task if the track has changed
        if (ulTaskNotifyTake(pdTRUE, 0) > 0)
        {
            if (!notification_displayed)
            {
                ui_notify(true, NOTIFY_INFO, tracks[currentTrack].name);
            }
            else
            {
                ui_notify(false);
            }
        }

        // Refresh
        uint32_t time_until_next = lv_timer_handler();

        // Don't run the task unnecessarily often
        if (time_until_next > 10)
            time_until_next = 10;

        vTaskDelay(pdMS_TO_TICKS(time_until_next));
    }
}
