#include <Arduino.h>
#include <FreeRTOS.h>
#include "main.hpp"

#include "conf.h"

#include <TinyGPS++.h>

TinyGPSPlus gps;

static double lastLat = 0;
static double lastLon = 0;

uint32_t lastLapMillis = 0;

void countLap()
{
    lap_time_seconds = (millis() - lastLapMillis) / 1000;

    lastLapMillis = millis();

    laps_amount++;

    total_distance_travelled += tracks[currentTrack].trackLength;
}

void checkForLap(void)
{
    if (!(lastLat == 0 && lastLon == 0))
    {
        switch (tracks[currentTrack].passingHeading)
        {
            case north:
                if (lastLat < tracks[currentTrack].latitude && gps.location.lat() > tracks[currentTrack].latitude)
                {
                    countLap();
                }
                break;
            case east:
                if (lastLon < tracks[currentTrack].longitude && gps.location.lng() > tracks[currentTrack].longitude)
                {
                    countLap();
                }
                break;
            case south:
                if (lastLat > tracks[currentTrack].latitude && gps.location.lat() < tracks[currentTrack].latitude)
                {
                    countLap();
                }
                break;
            case west:
                if (lastLon > tracks[currentTrack].longitude && gps.location.lng() < tracks[currentTrack].longitude)
                {
                    countLap();
                }
                break;
            default:
                break;
        }
    }

    lastLat = gps.location.lat();
    lastLon = gps.location.lng();    
}

void GPS_Task(void *pvParameters)
{
    Serial1.begin(GPS_BAUD, SERIAL_8N1, GPS_TX, GPS_RX);

    Serial1.setTimeout(10);

    for (;;)
    {
        while (Serial1.available() > 0)
        {
            gps.encode(Serial1.read());
        }

        // Clock syncing
        if (gps.date.isValid() && gps.time.isValid() && gps.date.age() < 2000)
        {
            if (last_clock_sync == 0 || (millis() - last_clock_sync >= SYNC_INTERVAL))
            {
                int y = gps.date.year();
                int m = gps.date.month();
                int d = gps.date.day();
                int hh = gps.time.hour();
                int mm = gps.time.minute();
                int ss = gps.time.second();

                // Convert month/year for Zeller/Fliegel-Van Flandern algorithm
                if (m <= 2)
                {
                    y -= 1;
                    m += 12;
                }

                // Exact cumulative days since Jan 1, 1970 00:00:00 UTC
                int32_t era = (y >= 0 ? y : y - 399) / 400;
                uint32_t yoe = static_cast<uint32_t>(y - era * 400);  // [0, 399]
                uint32_t doy = (153 * (m - 3) + 2) / 5 + d - 1;       // [0, 365]
                uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy; // [0, 146096]
                int32_t days = era * 146097 + static_cast<int32_t>(doe) - 719468;

                // Calculate final epoch
                time_t epoch = (days * 86400) + (hh * 3600) + (mm * 60) + ss;

                struct timeval tv = {
                    .tv_sec = epoch,
                    .tv_usec = 0};

                settimeofday(&tv, nullptr);
                last_clock_sync = millis();
            }
        }

        if (gps.location.isValid() && gps.location.isUpdated())
        {
            checkForLap();
        }

        // Give the system other GPS data
        if (xSemaphoreTake(GPS_Data_Mutex, pdMS_TO_TICKS(10)))
        {
            if (gps.speed.isValid() && gps.speed.isUpdated())
            {
                GPS_Data.speed = static_cast<int16_t>(gps.speed.kmph());
            }
            else if (!gps.speed.isValid())
            {
                GPS_Data.speed = -1;
            }

            if (gps.satellites.isValid())
            {
                GPS_Data.satellites = gps.satellites.value();
            }

            xSemaphoreGive(GPS_Data_Mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
