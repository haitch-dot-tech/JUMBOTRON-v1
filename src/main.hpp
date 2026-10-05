#pragma once

#include <Arduino.h>
#include <vesc_can.hpp>

typedef enum
{
    north,
    east,
    south,
    west
} CardinalDirection;

struct Track
{
    char *name;
    uint32_t trackLength;             // m
    unsigned int optimalLapTime;      // seconds
    double latitude, longitude;       // the coordinates of the start point centre
    CardinalDirection passingHeading; // which general direction the vehicle is travelling when passing the start line
};

Track tracks[] =
    {          // Name                         Length (m)  Optimal lap (s)  Latitude    Longitude   Line crossing heading
        {(char *)"Maryborough",                1580,       120,             -37.045413, 143.742544, west},
        {(char *)"Tom Flood",                  400,        27,              -36.752221, 144.280549, west},
        {(char *)"Bendigo Kart Club",          650,        60,              -36.758277, 144.240695, east},
        {(char *)"Bendigo Livestock Exchange", 86,         1200,            -36.676555, 144.300467, north},
        {(char *)"Haddon Kart Club",           675,        64,              -37.588638, 143.713429, north},
        {(char *)"Home Track",                 1000,       120,             -36.884954, 144.310404, south}};

const size_t numTracks = sizeof(tracks) / sizeof(tracks[0]);

struct gps_data
{
    int16_t speed = -1;
    uint8_t satellites = 0;
};

extern TaskHandle_t LED_Task_Handle;
extern TaskHandle_t Button_Task_Handle;
extern TaskHandle_t GPS_Task_Handle;
extern TaskHandle_t LVGL_Task_Handle;
extern TaskHandle_t VESC_Task_Handle;

extern SemaphoreHandle_t GPS_Data_Mutex;
extern SemaphoreHandle_t VESC_Data_Mutex;

extern gps_data GPS_Data;
extern vesc_data VESC_Data;

extern unsigned int currentTrack;

extern bool vesc_connected;

extern uint32_t last_clock_sync;

extern unsigned int lap_time_seconds;

extern unsigned int laps_amount;

extern uint32_t total_distance_travelled; // m

extern float current_en_eff_wh_lap;
