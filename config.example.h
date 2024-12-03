#ifndef CONFIG_H
#define CONFIG_H

// Wifi credentials
const char ssid[] = "wifi_ssid";
const char wifi_pass[] = "wifi_password";

// Timezone information
const char time_zone_str[] = "PST8PDT,M3.2.0,M11.1.0"; // Timezone string for America/Los_Angeles
const long timezone_offset_sec = -8 * 60 * 60; // Timezone offset in seconds for America/Los_Angeles
const int daylight_savings_offset_sec = 1 * 60 * 60; // Daylight savings offset in seconds for America/Los_Angeles

// API credentials
// Comment out next three lines to make unauthenticated API calls at a slower rate limit.
#define authenticated_api
const char api_username[] = "opensky-network_username";
const char api_password[] = "opensky-network_password";

// Location information
const float center_latitude = 34.05; // Latitude for Los Angeles
const float center_longitude = -118.25; // Longitude for Los Angeles
const float radius_degs = 0.5; // How wide of a box (in degrees) from the center should be searched. For instance, this default of 0.5 will result in a 1 degree x 1 degree square being searched, centered on the above coordinates.

// Display information
// Example start page text.
const char start_name[] = "PASSENGER: Jane Doe";
const char start_flight[] = "FLIGHT: JD123";
const char start_from[] = "FROM: LAX";
const char start_from_date[] = "1-1-1970";
const char start_to[] = "TO: LAX";
const char start_to_date[] = "1-2-1970";
const char start_misc[] = "Enjoy watching planes!";

// Example header text
const char header_text[] = "LA Flight Tracker";

#endif // CONFIG_H
