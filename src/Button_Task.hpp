#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include <Preferences.h>

#include "conf.h"

Preferences flashmem;

void writeTrack(int track)
{
    flashmem.begin("settings", false);
    flashmem.putInt("track", track);
    flashmem.end();
}

int readTrack(void)
{
    flashmem.begin("settings", true);
    int track = flashmem.getInt("track", 0);
    flashmem.end();

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
