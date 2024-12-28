#include "setup.h"

#include "config.h"
#include "Inkplate.h"
#include "InterVariable16pt7b.h"
#include "InterVariable24pt7b.h"
#include <WiFi.h>

#ifdef authenticated_api
const uint64_t requests_per_day = 4000;
#else
const uint64_t requests_per_day = 400;
#endif

const uint64_t micros_per_day = 86400000000;
const uint64_t micros_between_requests = micros_per_day / requests_per_day;

bool initDevice() {
  Serial.println(F("Initializing WiFi network..."));
  if (!initWifi()) {
    Serial.println(F("ERROR: Failed to initialize the network. Restarting..."));
    return false;
  }
  Serial.println(F("Network initialization complete."));
  Serial.print(F("Device IP address: "));
  Serial.println(WiFi.localIP());

  Serial.println(F("Setting the time..."));
  if (!initTime()) {
    Serial.println(F("ERROR: Failed to initialize time. Continueing anyway..."));
  }

  return true;
}

bool initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_pass);

  // Try to connect to WiFi for 10 seconds. Fail initialization if still not connected after that long.
  Serial.println(F("Connecting to WiFi..."));
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("ERROR: Could not connect to WiFi. Timed out."));
    return false;
  }
  Serial.println(F("Connected to WiFi."));

  return true;
}

bool initTime() {
  configTime(timezone_offset_sec, daylight_savings_offset_sec, "pool.ntp.org", "time.nist.gov");

  Serial.println(F("Getting time from the network..."));
  time_t time_sec = time(nullptr);

  // Try to get time for up to 10 minutes. Fail initialization if still not connected after that long.
  while (time_sec < 60 * 10) {
    delay(500);
    yield();
    time_sec = time(nullptr);
  }
  if (time_sec < 60 * 60 * 24) {
    return false;
  }

  struct tm time_info;
  localtime_r(&time_sec, &time_info);

  Serial.print(F("Time set: "));
  Serial.println(asctime(&time_info));

  return true;
}

bool wakeUpDevice() {
  Serial.println(F("Waking device up..."));
  Serial.println(F("Initializing WiFi network..."));
  if (!initWifi()) {
    Serial.println(F("ERROR: Failed to connect to network."));
  }
  Serial.println(F("Network initialization complete."));
  Serial.print(F("Device IP address: "));
  Serial.println(WiFi.localIP());

  return true;
}

void setDeviceToSleep() {
  Serial.println(F("Setting device to sleep..."));
  WiFi.setSleep(true);

  esp_sleep_enable_timer_wakeup(micros_between_requests);
  (void)esp_deep_sleep_start();
}