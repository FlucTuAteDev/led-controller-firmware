#pragma once
#include "Effect.h"
#include "utils.h"
#include <Arduino.h>
#include <FastLED.h>

#define UPDATE_INTERVAL 10

enum class PreviewMode {
	COLOR,
	INDEX,
	EFFECT,
};

class Previewer {
  public:
	void previewColor(const CRGB &color);
	void previewIndex(uint16_t index);
	void previewEffect(Effect *effect, EffectDirection direction);

	void setColor(const CRGB &color) {
		this->targetColor = color;
		this->currentColor = color;
	}

	bool update(CRGB *leds, uint16_t numLeds);

  private:
	bool updateColor(CRGB *leds, uint16_t numLeds);
	bool updateIndex(CRGB *leds, uint16_t numLeds);
	bool updateEffect(CRGB *leds, uint16_t numLeds);

	template <typename T>
	T updateTowardsTarget(T currentValue, T targetValue, uint32_t &lastUpdateTime, uint32_t updateInterval = UPDATE_INTERVAL);

	PreviewMode mode = PreviewMode::COLOR;

	CRGB targetColor;
	CRGB currentColor;
	uint32_t lastRColorUpdateTime = 0;
	uint32_t lastGColorUpdateTime = 0;

	uint16_t currentIndex = 0;
	uint16_t targetIndex = 0;
	uint32_t lastIndexUpdateTime = 0;

	Effect *currentEffect = nullptr;
};

template <typename T>
inline T Previewer::updateTowardsTarget(T currentValue, T targetValue, uint32_t &lastUpdateTime, uint32_t updateInterval) {
	auto difference = targetValue - currentValue;
	if (difference == 0) return currentValue;

	auto currentTime = millis();
	auto elapsedTime = currentTime - lastUpdateTime;
	if (elapsedTime < updateInterval) return currentValue;

	lastUpdateTime = currentTime;

	return currentValue + sgn(difference);
}
