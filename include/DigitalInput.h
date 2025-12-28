#pragma once
#include <Arduino.h>

#define DEFAULT_DEBOUNCE_DELAY_MS 50

class DigitalInput {
  public:
	DigitalInput(uint8_t pin, uint8_t onState = HIGH, uint32_t debounceDelayMs = DEFAULT_DEBOUNCE_DELAY_MS)
		: pin(pin), onState(onState), currentState(!onState), previousState(!onState), debounceDelayMs(debounceDelayMs) {}

	bool isOn();
	bool isOff();

	void begin(uint8_t inputMode = INPUT);

	bool getSingleDebouncedStateChange();

	uint8_t getCurrentState() { return this->currentState; }

  private:
	uint8_t pin;

	uint8_t onState;
	uint8_t currentState;
	uint8_t previousState;

	uint32_t lastStateChangeTime;
	uint32_t debounceDelayMs;
};