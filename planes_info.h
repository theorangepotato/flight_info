#ifndef PLANES_INFO_H2
#define PLANES_INFO_H

// Holds information about the nearest plane.
struct PlaneInfo {
  char icao[7];
  char callsign[9];
  char origin_country[25];
  float longitude;
  float latitude;
  float altitude;
  float velocity;
  float direction;
  float vertical_rate;
  int category;
};

// Get the type of plane, if available.
// Currently unused.
bool categoryToString(int category, char * out);

// Convert direction in degrees to cardinal direction
bool directionToString(float direction, char * out);

// Connects to the API, parses the result, finds the closest plane to the coordinates, and sets the plane's parameters in `closest_plane`.
bool getClosestPlane(PlaneInfo* closest_plane);

#endif // PLANES_INFO_H