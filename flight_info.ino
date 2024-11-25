#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error \
  "This software solely supports the Inkplate6. Please choose either the e-radionica or Soldered Inkplate6 from the Boards Manager."
#endif

#include "Inkplate.h"
#include "config.h"
#include <WiFi.h>

bool initWifi();
bool initTime();

void setup() {
  Serial.begin(115200);

  if (!initWifi()) {
    Serial.println("Failed to initialize the network. Restarting.");
    delay(200);
    ESP.restart();
  }
  Serial.println("Network initialization complete.");
}

void loop() {
  // put your main code here, to run repeatedly:

}

bool initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_pass);

  Serial.println(F("Connecting to WiFi..."));
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
    if (i >= 10) {
      Serial.println(F("Could not connect to WiFi. Timed out."));
      return false;
    }

    delay(1000);
  }

  Serial.println(F("Connected to WiFi."));

  return initTime();
}

bool initTime() {
  configTime(timezone_offset_sec, daylight_savings_offset_sec, "pool.ntp.org", "time.nist.gov");

  Serial.println(F("Getting time from the network..."));
  time_t time_sec = time(nullptr);
  for (int i = 0; time_sec < 60 * 60 * 24; i++) {
    if (i >= 10) {
      Serial.println(F("Could not get the time from the network."));
      return false;
    }

    delay(500);
    yield();
    time_sec = time(nullptr);
  }

  struct tm time_info;
  localtime_r(&time_sec, &time_info);

  Serial.print(F("Time: "));
  Serial.print(asctime(&time_info));
  Serial.println();

  return true;
}
