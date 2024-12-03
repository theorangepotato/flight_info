#include "planes_info.h"

#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

bool categoryToString(int category, char * out) {
  switch (category) {
    case 0:
      strcpy(out, "No information at all");
      return true;
    case 1:
      strcpy(out, "No ADS-B Emitter Category Information");
      return true;
    case 2:
      strcpy(out, "Light (< 15500 lbs)");
      return true;
    case 3:
      strcpy(out, "Small (15500 to 75000 lbs)");
      return true;
    case 4:
      strcpy(out, "Large (75000 to 300000 lbs)");
      return true;
    case 5:
      strcpy(out, "High Vortex Large (aircraft such as B-757)");
      return true;
    case 6:
      strcpy(out, "Heavy (> 300000 lbs)");
      return true;
    case 7:
      strcpy(out, "High Performance (> 5g acceleration and 400 kts)");
      return true;
    case 8:
      strcpy(out, "Rotorcraft");
      return true;
    case 9:
      strcpy(out, "Glider / sailplane");
      return true;
    case 10:
      strcpy(out, "Lighter-than-air");
      return true;
    case 11:
      strcpy(out, "Parachutist / Skydiver");
      return true;
    case 12:
      strcpy(out, "Ultralight / hang-glider / paraglider");
      return true;
    case 13:
      strcpy(out, "Reserved");
      return true;
    case 14:
      strcpy(out, "Unmanned Aerial Vehicle");
      return true;
    case 15:
      strcpy(out, "Space / Trans-atmospheric vehicle");
      return true;
    case 16:
      strcpy(out, "Surface Vehicle – Emergency Vehicle");
      return true;
    case 17:
      strcpy(out, "Surface Vehicle – Service Vehicle");
      return true;
    case 18:
      strcpy(out, "Point Obstacle (includes tethered balloons)");
      return true;
    case 19:
      strcpy(out, "Cluster Obstacle");
      return true;
    case 20:
      strcpy(out, "Line Obstacle");
      return true;
  }

  return false;
}

bool getClosestPlane(PlaneInfo* closest_plane) {
  HTTPClient http_client;
  http_client.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  char api_url[256];
  sprintf(api_url, "https://opensky-network.org/api/states/all?lamin=%.3f&lomin=%.3f&lamax=%.3f&lomax=%.3f&extended=1", center_latitude - radius_degs, center_longitude - radius_degs, center_latitude + radius_degs, center_longitude + radius_degs);

  // char api_url[] = "https://test.alphahotelbravo.io";

  Serial.print(F("Attempting to make API call to: "));
  Serial.print(api_url);
  Serial.println(F("..."));

  http_client.begin(api_url);

  int response_code = http_client.GET();
  if (response_code != 200) {
    char response_code_str[3];
    ltoa(response_code, response_code_str, 10);
    Serial.print(F("ERROR: Got the following response code: "));
    Serial.println(response_code);
    return false;
  }

  // size can be -1 if the server doesn't send a Content-Length header
  if (http_client.getSize() < -1 || http_client.getSize() == 0) {
    char response_size[4];
    ltoa(http_client.getSize(), response_size, 10);
    Serial.print(F("ERROR: Response had size: "));
    Serial.println(response_size);
    return false;
  }

  String output = http_client.getString();

  JsonDocument json;
  DeserializationError err = deserializeJson(json, output);
  if (err) {
    Serial.print(F("ERROR: JSON deserialization error: "));
    Serial.println(err.c_str());
    return false;
  }

  // TODO: Maybe we want to return default information?
  if (json["states"].size() <= 0) {
    Serial.println(F("ERROR: No planes nearby"));
    return false;
  }

  // Get closest plane
  float shortest_distance_squared = -1;
  int closest_plane_index = 0;
  for (int i = 0; i < json["states"].size(); i++) {
    // skip any vehicles still on the ground.
    float geo_alt = json["states"][i][13];
    if (geo_alt < 50) {
      continue;
    }

    float plane_longitude = json["states"][i][5];
    float plane_latitude = json["states"][i][6];
    // Distance squared is sufficient, because we don't care about the actual distance, just the order. And, if a < b, then a*a < b*b for all (a, b) that are non-negative real numbers.
    float distance_squared = ((plane_longitude - center_longitude) * (plane_longitude - center_longitude)) + ((plane_latitude - center_latitude) * (plane_latitude - center_latitude));
    if (shortest_distance_squared < 0 || distance_squared < shortest_distance_squared) {
      shortest_distance_squared = distance_squared;
      closest_plane_index = i;
    }
  }

  strcpy(closest_plane->icao, json["states"][closest_plane_index][0]);
  strcpy(closest_plane->callsign, json["states"][closest_plane_index][1]);
  strcpy(closest_plane->origin_country, json["states"][closest_plane_index][2]);
  closest_plane->longitude = json["states"][closest_plane_index][5];
  closest_plane->latitude = json["states"][closest_plane_index][6];
  closest_plane->altitude = json["states"][closest_plane_index][13];
  closest_plane->velocity = json["states"][closest_plane_index][9];
  closest_plane->direction = json["states"][closest_plane_index][10];
  closest_plane->vertical_rate = json["states"][closest_plane_index][11];
  closest_plane->category = json["states"][closest_plane_index][17];

  return true;
}