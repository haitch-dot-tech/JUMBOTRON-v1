#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include "conf.h"

unsigned int last_lap = 0;

void VESC_Task(void *pvParameters)
{
    vesc_can_init(CAN_TX, CAN_RX);

    while (!laps_count_updated)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    last_lap = laps_amount;

    for (;;)
    {
        if (xSemaphoreTake(VESC_Data_Mutex, pdMS_TO_TICKS(10)))
        {
            vesc_can_update(&VESC_Data);

            vesc_connected = (VESC_Data.vesc_temp > 0) ? true : false;

            if (vesc_connected && last_lap < laps_amount)
            {
                current_en_eff_wh_km = (VESC_Data.watt_hours - last_energy_consumption) / (static_cast<float>(laps_amount - last_lap) * static_cast<float>(tracks[currentTrack].trackLength) / 1000.0f);

                last_energy_consumption = VESC_Data.watt_hours;

                last_lap = laps_amount;
            }

            xSemaphoreGive(VESC_Data_Mutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
