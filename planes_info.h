#ifndef PLANES_INFO_H2
#define PLANES_INFO_H

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

struct PlaneImage {
  char photo_url[250];
  int width;
  int height;
};

bool categoryToString(int category, char * out);

bool directionToString(float direction, char * out);

bool getClosestPlane(PlaneInfo* closest_plane);

bool getPlaneImage(char * icao, PlaneImage * plane_image);

#endif // PLANES_INFO_H