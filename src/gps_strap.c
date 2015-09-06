#include <pebble.h>
#include "gps_strap.h"

static GPSData s_gps_data;
static const SmartstrapServiceId SERVICE_ID = 0x1ABC;
static const SmartstrapAttributeId ATTRIBUTE_ID = 0x0001;
static SmartstrapAttribute *s_attribute;

static void prv_did_read(SmartstrapAttribute *attr, SmartstrapResult result,
                              const uint8_t *data, size_t length) {
  if (attr == s_attribute) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "did_read(s_attribute, %d, %d), need %d", result, length, sizeof(s_gps_data));
    if (result == SmartstrapResultOk && length == sizeof(s_gps_data)) {
      memcpy(&s_gps_data, data, sizeof(s_gps_data));
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Lat: %i.%u Lon: %i.%u", s_gps_data.lat,
                                    s_gps_data.lat_dec, s_gps_data.lon, s_gps_data.lon_dec);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "did_read(<%p>, %d)", attr, result);
  }
}

static void prv_did_write(SmartstrapAttribute *attr, SmartstrapResult result) {
  if (attr == s_attribute) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "did_write(s_attribute, %d)", result);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "did_write(<%p>, %d)", attr, result);
  }
}

static void prv_write_read_test_attr(void) {
  SmartstrapResult result;
  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(s_attribute))) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "s_attribute is not available");
    return;
  }

  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(s_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Write of s_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  uint32_t num = rand() % 10000;
  memcpy(buffer, &num, 4);

  // send it off
  result = smartstrap_attribute_end_write(s_attribute, sizeof(num), true);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Write of s_attribute failed with result %d", result);
  }
}

static void prv_send_request(void *context) {
  prv_write_read_test_attr();
  app_timer_register(900, prv_send_request, NULL);
}

static void prv_availablility_status_changed(SmartstrapServiceId service_id, bool is_available) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Availability for 0x%x is %d", service_id, is_available);
}

static void prv_notified(SmartstrapAttribute *attr) {
  if (attr == s_attribute) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "notified(s_attribute)");
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "notified(<%p>)", attr);
  }
}
  
void gps_init() {
   SmartstrapHandlers handlers = (SmartstrapHandlers) {
    .availability_did_change = prv_availablility_status_changed,
    .did_write = prv_did_write,
    .did_read = prv_did_read,
    .notified = prv_notified
  };
  smartstrap_subscribe(handlers);
  smartstrap_set_timeout(50);
  s_attribute = smartstrap_attribute_create(SERVICE_ID, ATTRIBUTE_ID, 20);
  app_timer_register(1000, prv_send_request, NULL);
  memset(&s_gps_data, 0, sizeof(s_gps_data));
}

void gps_deinit() {
  smartstrap_unsubscribe();
}

int32_t gps_get_lat() {
  
  int32_t lat;
  
  lat = s_gps_data.lat * 1000;
  if ( lat < 0 ) {
    lat = lat - s_gps_data.lat_dec;
  } else {
    lat = lat + s_gps_data.lat_dec;
  }
  
  return lat;
}

int32_t gps_get_lon() {
  
  int32_t lon;
  
  lon = s_gps_data.lon * 1000;
  if ( lon < 1 ) {
    lon = lon - s_gps_data.lon_dec;
  } else {
    lon = lon + s_gps_data.lon_dec;
  }
  
  return lon;
}