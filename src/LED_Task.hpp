#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include "conf.h"

void LED_Task(void *pvParameters)
{
    pinMode(LED_PIN, OUTPUT);

    for (;;)
    {
        digitalWrite(LED_PIN, HIGH);

        vTaskDelay(pdMS_TO_TICKS(LED_TIME));

        digitalWrite(LED_PIN, LOW);

        vTaskDelay(pdMS_TO_TICKS(LED_PERIOD - LED_TIME));
    }
}
