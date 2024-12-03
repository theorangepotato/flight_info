#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error \
  "This software solely supports the Inkplate6. Please choose either the e-radionica or Soldered Inkplate6 from the Boards Manager."
#endif

#include "config.h"
#include "planes_info.h"
#include "Inkplate.h"
#include "InterVariable16pt7b.h"
#include "InterVariable24pt7b.h"
#include <WiFi.h>

Inkplate display(INKPLATE_1BIT);

RTC_DATA_ATTR bool initial_wake = true;

bool initDevice();
bool initWifi();
bool initTime();
bool wakeUpDevice();
void setDeviceToSleep();
void printStartScreen();
void printPlaneInfo(PlaneInfo * plane_info);

void setup() {
  Serial.begin(500000);

  if(initial_wake) {
    if (!initDevice()) {
      Serial.println(F("ERROR: Fatal initialization error. Restarting device..."));
      delay(200);
      ESP.restart();
    }
    initial_wake = false;
    Serial.println(F("Device initialized."));
  } else {
    if (!wakeUpDevice()) {
      Serial.println(F("ERROR: Failed to wake device up. Restarting device..."));
      delay(200);
      ESP.restart();
    }
    Serial.println(F("Device set up."));
  }

  PlaneInfo closest_plane;

  if (!getClosestPlane(&closest_plane)) {
    // TODO: Make this more graceful
    Serial.println(F("ERROR: Failed to get closest plane."));
  } else {
    printPlaneInfo(&closest_plane);
  }

  setDeviceToSleep();
}

void loop() {}

bool initDevice() {
  Serial.println(F("Printing start screen..."));
  if (!display.begin()) {
    Serial.println(F("ERROR: Failed to start display. Restarting..."));
    return false;
  }
  printStartScreen();
  Serial.println(F("Screen printed"));

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

  Serial.println(F("Starting screen..."));
  if (!display.begin()) {
    Serial.println(F("ERROR: Failed to start display. Restarting..."));
    return false;
  }

  return true;
}

void setDeviceToSleep() {
  Serial.println(F("Setting device to sleep..."));
  WiFi.setSleep(true);

  esp_sleep_enable_timer_wakeup(1000L * 10000); // 10 seconds
  (void)esp_deep_sleep_start();
}

void printStartScreen() {
  uint16_t cursor_x = 30, cursor_y = 30, delta_y = InterVariable24pt7b.yAdvance;

  display.clearDisplay();

  // Set font
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);
  display.setFont(&InterVariable24pt7b);

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(start_name);

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(start_flight);

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(start_from);

  display.setFont(&InterVariable16pt7b);
  display.setCursor(cursor_x, cursor_y += InterVariable16pt7b.yAdvance);
  display.print(start_from_date);

  display.setFont(&InterVariable24pt7b);
  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(start_to);

  display.setFont(&InterVariable16pt7b);
  display.setCursor(cursor_x, cursor_y += InterVariable16pt7b.yAdvance);
  display.print(start_to_date);

  display.setCursor(cursor_x, 570);
  display.print(start_misc);

  display.display();
}

void printPlaneInfo(PlaneInfo* plane_info) {
  Serial.print(F("The closest plane has icao: "));
  Serial.println(plane_info->callsign);

  display.clearDisplay();

  uint16_t cursor_x = 30, cursor_y = 30 + InterVariable24pt7b.yAdvance, delta_y = InterVariable16pt7b.yAdvance;

  // Set font
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);
  display.setFont(&InterVariable24pt7b);

  // Print header
  display.setCursor(cursor_x, cursor_y);
  display.print(header_text);
  cursor_y += 30;

  // Set font for plane stats
  display.setFont(&InterVariable16pt7b);

  // Get time
  setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
  tzset();
  time_t time_sec = time(nullptr);
  struct tm time_info;
  localtime_r(&time_sec, &time_info);
  char time_string[25];
  strftime(time_string, 25, "%I:%M:%S %p, %d %b %Y", &time_info);

  // Print time
  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Time: "));
  display.print(time_string);

  // Print plane stats
  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Nearest plane: "));
  display.print(plane_info->callsign);

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Origin Country: "));
  display.print(plane_info->origin_country);

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Location: "));
  display.print(plane_info->latitude);
  display.print(F(", "));
  display.print(plane_info->longitude);

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Altitude: "));
  display.print(plane_info->altitude);
  display.print(F(" m"));

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Velocity: "));
  display.print(plane_info->velocity);
  display.print(F(" m/s"));

  char direction[3];
  if (directionToString(plane_info->direction, direction)) {
    display.setCursor(cursor_x, cursor_y += delta_y);
    display.print(F("Heading: "));
    display.print(direction);
  }

  display.setCursor(cursor_x, cursor_y += delta_y);
  display.print(F("Vertical speed: "));
  display.print(plane_info->vertical_rate);
  display.print(F(" m/s"));

  // Actually display everything on the screen
  display.display();
}
