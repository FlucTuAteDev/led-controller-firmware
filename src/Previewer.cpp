#include "Previewer.h"

void Previewer::previewColor(const CRGB &color) {
	this->mode = PreviewMode::COLOR;

	this->targetColor = color;
}

void Previewer::previewIndex(uint16_t index) {
	this->mode = PreviewMode::INDEX;

	this->targetIndex = index;
}

void Previewer::previewEffect(Effect *effect, EffectDirection direction) {
	this->mode = PreviewMode::EFFECT;

	this->currentEffect = effect;
	if (this->currentEffect == nullptr) return;

	this->currentEffect->reset(direction);
	this->currentEffect->start(direction);
}

bool Previewer::update(CRGB *leds, uint16_t numLeds) {
	const auto updatedColor = this->updateColor(leds, numLeds);
	const auto updatedIndex = this->updateIndex(leds, numLeds);
	const auto updatedEffect = this->updateEffect(leds, numLeds);

	return updatedColor || updatedIndex || updatedEffect;
}

bool Previewer::updateColor(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::COLOR) {
		this->currentColor = this->targetColor;
		return false;
	}

	if (this->currentColor == this->targetColor) return false;

	this->currentColor.r = this->updateTowardsTarget(this->currentColor.r, this->targetColor.r, this->lastRColorUpdateTime);
	this->currentColor.g = this->updateTowardsTarget(this->currentColor.g, this->targetColor.g, this->lastGColorUpdateTime);

	fill_solid(leds, numLeds, this->currentColor);

	return true;
}

bool Previewer::updateIndex(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::INDEX) {
		this->currentIndex = this->targetIndex;
		return false;
	}

	if (this->currentIndex == this->targetIndex) return false;
	this->currentIndex = this->updateTowardsTarget(this->currentIndex, this->targetIndex, this->lastIndexUpdateTime);

	fill_solid(leds, numLeds, CRGB::Black);
	leds[this->currentIndex] = this->currentColor;

	return true;
}

bool Previewer::updateEffect(CRGB *leds, uint16_t numLeds) {
	if (this->mode != PreviewMode::EFFECT) {
		this->currentEffect = nullptr;
		return false;
	}

	if (this->currentEffect == nullptr) return false;

	return this->currentEffect->update(leds, numLeds, this->currentColor);
}
