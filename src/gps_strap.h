#pragma once

#define TIMEOUT_MS 1000
#define MAX_READ_SIZE 100
  
typedef struct {
  int16_t lat;
  uint16_t lat_dec;
  int16_t lon;
  uint16_t lon_dec;
  int16_t alt;
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t valid;
  uint8_t null_pack;
} GPSData;
  
void gps_init();
void gps_deinit();
int32_t gps_get_lat();
int32_t gps_get_lon();