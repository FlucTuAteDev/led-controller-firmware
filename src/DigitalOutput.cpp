#include "DigitalOutput.h"

void DigitalOutput::begin() {
	pinMode(this->pin, OUTPUT);
	digitalWrite(this->pin, !this->onState);
}

void DigitalOutput::on() {
	digitalWrite(this->pin, this->onState);
}

void DigitalOutput::off() {
	digitalWrite(this->pin, !this->onState);
}

bool DigitalOutput::isOn() {
	return digitalRead(this->pin) == this->onState;
}

bool DigitalOutput::isOff() {
	return digitalRead(this->pin) != this->onState;
}