#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include <Preferences.h>

#include "conf.h"

Preferences btn_flashmem;

void writeTrack(int track)
{
    btn_flashmem.begin("settings", false);
    btn_flashmem.putInt("track", track);
    btn_flashmem.end();

    btn_flashmem.begin("data", false);
    btn_flashmem.putInt("laps", 0);
    btn_flashmem.end();

    initialLapCount = 0;
}

int readTrack(void)
{
    btn_flashmem.begin("settings", true);
    int track = btn_flashmem.getInt("track", 0);
    btn_flashmem.end();

    return track;
}

void IRAM_ATTR buttonISR()
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(Button_Task_Handle, &higherPriorityTaskWoken);

    if (higherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

void Button_Task(void *pvParameters)
{
    pinMode(0, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(0), buttonISR, FALLING);

    currentTrack = readTrack();

    xTaskNotifyGive(LVGL_Task_Handle);

    for (;;)
    {
        // Wait indefinitely for the interrupt
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Debounce
        vTaskDelay(pdMS_TO_TICKS(30));

        // Check that the button is actually pressed
        if (digitalRead(0) == LOW)
        {
            if (currentTrack < numTracks - 1)
            {
                currentTrack++;
            }
            else 
            {
                currentTrack = 0;
            }

            writeTrack(currentTrack);

            // Wait for button release
            while (digitalRead(0) == LOW)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            xTaskNotifyGive(LVGL_Task_Handle);

            vTaskDelay(1000);

            xTaskNotifyGive(LVGL_Task_Handle);
        }
    }
}
