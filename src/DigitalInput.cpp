#include "DigitalInput.h"

bool DigitalInput::isOn() {
	return this->currentState == this->onState;
}

bool DigitalInput::isOff() {
	return this->currentState != this->onState;
}

void DigitalInput::begin(uint8_t inputMode) {
	pinMode(this->pin, inputMode);
}

bool DigitalInput::getSingleDebouncedStateChange() {
	int state = digitalRead(this->pin);
	if (state != this->previousState) {
		this->lastStateChangeTime = millis();
	}
	this->previousState = state;

	if (millis() - this->lastStateChangeTime > this->debounceDelayMs) {
		if (state != this->currentState) {
			this->currentState = state;
			return true;
		}
	}

	return false;
}
