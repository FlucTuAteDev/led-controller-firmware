#pragma once
#include <Arduino.h>

class WiFiConnectionHandler {
  public:
	void begin();

  public:
	uint8_t connect();
	void setupAP();
};