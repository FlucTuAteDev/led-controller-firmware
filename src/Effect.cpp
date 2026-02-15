#include "Effect.h"
#include "Constants.h"
#include <FastLED.h>

#define DEFAULT_EFFECT_DURATION 3000

void Effect::reset() {
	this->reset(this->direction);
}

void Effect::reset(EffectDirection direction) {
	if (direction == EffectDirection::ON) {
		this->progress = 0.0f;
	} else {
		this->progress = 1.0f;
	}

	this->running = false;
	this->direction = direction;
}

void Effect::start(EffectDirection direction) {
	this->running = true;
	this->direction = direction;
	this->lastUpdate = millis();
}

void Effect::setDuration(uint32_t duration) {
	this->duration = duration;
}

bool Effect::setParameter(const EffectParameter parameter, const String &value) {
	if (parameter == EffectParameter::DURATION) {
		this->setDuration(value.toInt());
		return true;
	}

	return false;
}

void Effect::load(Preferences &preferences, const char *key) {
	const auto durationKey = this->getPreferenceKey(EffectParameter::DURATION, key);
	const auto durationValue = preferences.getUInt(durationKey.c_str(), DEFAULT_EFFECT_DURATION);
	this->setDuration(durationValue);
}

void Effect::save(Preferences &preferences, const char *key) {
	const auto durationKey = this->getPreferenceKey(EffectParameter::DURATION, key);
	const auto durationValue = this->getDuration();
	preferences.putUInt(durationKey.c_str(), durationValue);
}

String Effect::getPreferenceKey(EffectParameter parameter, const char *key) {
	return String(int(this->type)) + "_" + key + "_" + int(parameter);
}

bool Effect::update(CRGB *leds, uint16_t numLeds, const CRGB &onColor) {
	if (!this->running) return false;

	uint32_t now = millis();
	uint32_t dt = now - this->lastUpdate;
	// We don't update the lastUpdate time here. When the update function is called too fast this gives it a chance to work
	if (dt <= 0) return false;

	if (this->duration > 0.0f) {
		float speed = 1.0f / this->duration;

		this->progress += int(this->direction) * speed * dt;
	} else {
		this->progress = this->direction == EffectDirection::ON ? 1.0f : 0.0f;
	}

	if (this->progress >= 1.0f) {
		this->progress = 1.0f;
		this->running = false;
	} else if (this->progress <= 0.0f) {
		this->progress = 0.0f;
		this->running = false;
	}

	this->animate(leds, numLeds, onColor, this->progress);
	this->lastUpdate = now;
	return true;
}

void StaticEffect::animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) {
	const CRGB color = progress > 0.5 ? onColor : CRGB::Black;
	fill_solid(leds, numLeds, color);
}

void LightsaberEffect::setStartLedIndex(uint16_t startLedIndex) {
	const auto index = std::clamp((int)startLedIndex, 0, LED_ARRAY_COUNT - 1);
	this->startLedIndex = index;
}

bool LightsaberEffect::setParameter(const EffectParameter parameter, const String &value) {
	if (Effect::setParameter(parameter, value)) return true;

	if (parameter == EffectParameter::START_LED_INDEX) {
		this->setStartLedIndex(value.toInt());
		return true;
	}

	return false;
}

void LightsaberEffect::load(Preferences &preferences, const char *key) {
	Effect::load(preferences, key);

	const auto startLedIndexKey = this->getPreferenceKey(EffectParameter::START_LED_INDEX, key);
	const auto startLedIndexValue = preferences.getUShort(startLedIndexKey.c_str(), 0);
	this->setStartLedIndex(startLedIndexValue);
}

void LightsaberEffect::save(Preferences &preferences, const char *key) {
	Effect::save(preferences, key);

	const auto startLedIndexKey = this->getPreferenceKey(EffectParameter::START_LED_INDEX, key);
	const auto startLedIndexValue = this->getStartLedIndex();
	preferences.putUShort(startLedIndexKey.c_str(), startLedIndexValue);
}

uint16_t LightsaberEffect::getLongerSectionLength(uint16_t numLeds) {
	if (this->startLedIndex > numLeds / 2) {
		return this->startLedIndex + 1;
	}

	return numLeds - this->startLedIndex;
}

void LightsaberEffect::animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) {
	uint16_t longerSectionLength = this->getLongerSectionLength(numLeds);

	uint16_t distance = longerSectionLength * progress;

	for (uint16_t i = 0; i < distance; i++) {
		uint16_t rightIndex = this->startLedIndex + i;
		if (rightIndex < numLeds) {
			leds[rightIndex] = onColor;
		}
		int16_t leftIndex = (int16_t)this->startLedIndex - i;
		if (leftIndex >= 0) {
			leds[leftIndex] = onColor;
		}
	}

	for (uint16_t i = distance; i < numLeds; i++) {
		uint16_t rightIndex = this->startLedIndex + i;
		if (rightIndex < numLeds) {
			leds[rightIndex] = CRGB::Black;
		}
		int16_t leftIndex = (int16_t)this->startLedIndex - i;
		if (leftIndex >= 0) {
			leds[leftIndex] = CRGB::Black;
		}
	}
}

void FadeEffect::animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) {
	const auto newColor = onColor.scale8(progress * 255);

	for (uint16_t i = 0; i < numLeds; i++) {
		leds[i] = newColor;
	}
}
