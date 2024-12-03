#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error \
  "This software solely supports the Inkplate6. Please choose either the e-radionica or Soldered Inkplate6 from the Boards Manager."
#endif

#include "config.h"
#include "planes_info.h"
#include "Inkplate.h"
#include <WiFi.h>

bool initWifi();
bool initTime();
bool reconnectWifi();

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing network...");
  if (!initWifi()) {
    Serial.println("ERROR: Failed to initialize the network. Restarting.");
    delay(200);
    ESP.restart();
  }
  Serial.println("Network initialization complete.");

  Serial.print("Device IP address: ");
  Serial.println(WiFi.localIP());

  PlaneInfo closestPlane;

  if (!getClosestPlane(&closestPlane)) {
    // TODO: Make this more graceful
    Serial.println("ERROR: Failed to get closest plane.");
  } else {
    Serial.print(F("The closest plane has icao: "));
    Serial.println(closestPlane.callsign);
    // TODO: remove
    Serial.println("AUSTIN WAS HERE");

  }
}

void loop() {}

bool initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_pass);

  Serial.println(F("Connecting to WiFi..."));
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
    if (i >= 10) {
      Serial.println(F("ERROR: Could not connect to WiFi. Timed out."));
      return false;
    }

    delay(1000);
  }

  Serial.println(F("Connected to WiFi."));

  return true;

  // return initTime();
}

bool initTime() {
  configTime(timezone_offset_sec, daylight_savings_offset_sec, "pool.ntp.org", "time.nist.gov");

  Serial.println(F("Getting time from the network..."));
  time_t time_sec = time(nullptr);
  for (int i = 0; time_sec < 60 * 60 * 24; i++) {
    if (i >= 10) {
      Serial.println(F("ERROR: Could not get the time from the network."));
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

bool reconnectWifi() {
  // TODO: Perform reconnect, if needed.
  return true;
}
