#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include "conf.h"

void VESC_Task(void *pvParameters)
{
    vesc_can_init(CAN_TX, CAN_RX);

    for (;;)
    {
        if (xSemaphoreTake(VESC_Data_Mutex, pdMS_TO_TICKS(10)))
        {
            vesc_can_update(&VESC_Data);

            vesc_connected = (VESC_Data.vesc_temp > 0) ? true : false;

            if (vesc_connected)
                current_en_eff_wh_lap = VESC_Data.watt_hours / static_cast<float>(laps_amount);

            xSemaphoreGive(VESC_Data_Mutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
