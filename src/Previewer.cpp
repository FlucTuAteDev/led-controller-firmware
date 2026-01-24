#include "Previewer.h"

void Previewer::previewBrightness(uint8_t brightness) {
	this->mode = PreviewMode::BRIGHTNESS;

	this->targetBrightness = brightness;
}

void Previewer::previewColorTemperature(uint8_t coldBrightness) {
	this->mode = PreviewMode::COLOR_TEMPERATURE;

	this->targetColdBrightness = coldBrightness;
}

void Previewer::previewIndex(uint16_t index) {
	this->mode = PreviewMode::INDEX;

	this->targetIndex = index;
}

void Previewer::previewEffect(Effect *effect, EffectDirection direction) {
	this->mode = PreviewMode::EFFECT;

	this->currentEffect = effect;
	if (this->currentEffect != nullptr) {
		this->currentEffect->reset(direction);
		this->currentEffect->start(direction);
	}
}

bool Previewer::update(CRGB *leds, uint16_t numLeds) {
	const auto updatedBrightness = this->updateBrightness(leds, numLeds);
	const auto updatedColorTemperature = this->updateColorTemperature(leds, numLeds);
	const auto updatedIndex = this->updateIndex(leds, numLeds);
	const auto updatedEffect = this->updateEffect(leds, numLeds);

	return updatedBrightness || updatedColorTemperature || updatedIndex || updatedEffect;
}

bool Previewer::updateBrightness(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::BRIGHTNESS) {
		FastLED.setBrightness(this->targetBrightness);
		this->currentBrightness = this->targetBrightness;
		return false;
	}

	uint8_t newBrightness = this->updateTowardsTarget(this->currentBrightness, this->targetBrightness, this->lastBrightnessUpdateTime);
	if (currentBrightness == newBrightness) return false;

	this->currentBrightness = newBrightness;

	for (uint16_t i = 0; i < numLeds; i++) {
		leds[i] = this->color;
	}

	FastLED.setBrightness(newBrightness);
	return true;
}

bool Previewer::updateColorTemperature(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::COLOR_TEMPERATURE) {
		this->color.r = this->targetColdBrightness;
		this->color.g = 255 - this->targetColdBrightness;

		this->currentColdBrightness = this->targetColdBrightness;
		return false;
	}

	uint8_t newColdBrightness = this->updateTowardsTarget(this->currentColdBrightness, this->targetColdBrightness, this->lastColdBrightnessUpdateTime);
	if (this->currentColdBrightness == newColdBrightness) return false;

	this->color.r = newColdBrightness;
	this->color.g = 255 - newColdBrightness;

	this->currentColdBrightness = newColdBrightness;

	for (uint16_t i = 0; i < numLeds; i++) {
		leds[i] = this->color;
	}
	return true;
}

bool Previewer::updateIndex(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::INDEX) {
		this->currentIndex = this->targetIndex;
		return false;
	}

	uint16_t newIndex = this->updateTowardsTarget(this->currentIndex, this->targetIndex, this->lastIndexUpdateTime);
	if (this->currentIndex == newIndex) return false;

	this->currentIndex = newIndex;

	for (uint16_t i = 0; i < numLeds; i++) {
		leds[i] = CRGB::Black;
	}
	leds[this->currentIndex] = this->color;

	return true;
}

bool Previewer::updateEffect(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::EFFECT) {
		this->currentEffect = nullptr;
		return false;
	}

	if (this->currentEffect == nullptr) return false;

	return this->currentEffect->update(leds, numLeds, this->color);
}
