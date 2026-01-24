#pragma once
#include "Effect.h"
#include "utils.h"
#include <Arduino.h>
#include <FastLED.h>

#define UPDATE_INTERVAL 10

enum class PreviewMode {
	BRIGHTNESS,
	COLOR_TEMPERATURE,
	INDEX,
	EFFECT,
};

class Previewer {
  public:
	void previewBrightness(uint8_t brightness);
	void previewColorTemperature(uint8_t coldBrightness);
	void previewIndex(uint16_t index);
	void previewEffect(Effect *effect, EffectDirection direction);

	bool update(CRGB *leds, uint16_t numLeds);

  private:
	bool updateBrightness(CRGB *leds, uint16_t numLeds);
	bool updateColorTemperature(CRGB *leds, uint16_t numLeds);
	bool updateIndex(CRGB *leds, uint16_t numLeds);
	bool updateEffect(CRGB *leds, uint16_t numLeds);

	template <typename T>
	T updateTowardsTarget(T currentValue, T targetValue, uint32_t &lastUpdateTime, uint32_t updateInterval = UPDATE_INTERVAL);

	PreviewMode mode = PreviewMode::BRIGHTNESS;

	uint8_t currentBrightness = 1;
	uint8_t targetBrightness = 1;
	uint32_t lastBrightnessUpdateTime = 0;

	uint8_t currentColdBrightness = 0;
	uint8_t targetColdBrightness = 0;
	uint32_t lastColdBrightnessUpdateTime = 0;
	CRGB color = CRGB(this->currentColdBrightness, 255 - this->currentColdBrightness, 0);

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
