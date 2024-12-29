# Plane Info

This repository is the source code for a flight tracker for the [Inkplate 6](https://soldered.com/product/inkplate-6-6-e-paper-board/). Every time the screen updates, it shows the closest plane to a given target coordinate.

It is written in Arduino C++.

## Contributing

To build, you will need to use [Arduino IDE](https://www.arduino.cc/en/software). Either follow your system's install instructions, or use the included `shell.nix` file. Then follow the instructions [here](https://inkplate.readthedocs.io/en/latest/get-started.html#arduino) to set up the board. You will also need to add the [`ArduinoJson`](https://arduinojson.org/) library in Arduino IDE.

Also, copy or rename `config.example.h` to `config.h` and update the values accordingly.

At this point, the code should build in Arduino IDE, and you should be able to upload it to the board.

## APIs

### [OpenSkyNetwork](https://openskynetwork.github.io/opensky-api/rest.html#all-state-vectors)

Currently in use to get plane information. It has a very generous free API and decent availability.

`GET /states/all`
* hex string
* callsign
* country
* Distance
* altitude
* velocity

### [PlaneSpotter](https://www.planespotters.net/photo/api)

Planned usage. There is some WIP work on a branch.

`/pub/photos/hex/`
* Thumbnail of most recent image of the plane

### FlightRadar24

Planned usage. Would not call API, but provide a QR code link to FlightRadar24.

`https://www.flightradar24.com/[CALLSIGN]`
* Link to FlightRadar24 information on the flight
* Can convert to QR code
