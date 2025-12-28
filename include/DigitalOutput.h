#pragma once
#include <Arduino.h>

class DigitalOutput {
  public:
	DigitalOutput(uint8_t pin, uint8_t onState = HIGH)
		: pin(pin), onState(onState) {}

	void begin();
	void on();
	void off();
	bool isOn();
	bool isOff();

  private:
	uint8_t pin;
	uint8_t onState;
};