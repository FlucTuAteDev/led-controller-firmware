#pragma once

#include <FastLED.h>
#include <Preferences.h>

enum class EffectDirection {
	OFF = -1,
	ON = 1,
};

enum class EffectType {
	NONE,
	LIGHTSABER,
	FADE
};

enum class EffectParameter {
	DURATION,
	START_LED_INDEX
};

/**
 * Class for defining turn on and turn off effects for LEDs
 */
class Effect {
  public:
	Effect(EffectType type, uint32_t duration)
		: type(type), duration(duration) {}

	void start(EffectDirection direction);

	void reset();
	void reset(EffectDirection direction);

	bool update(CRGB *leds, uint16_t numLeds, const CRGB &onColor);

	uint32_t getDuration() const { return this->duration; }
	void setDuration(uint32_t duration);

	virtual bool setParameter(const EffectParameter parameter, const String &value);
	virtual void load(Preferences &prefs, const char *key);
	virtual void save(Preferences &prefs, const char *key);

	bool isRunning() const { return this->running; }

	EffectType const type;

  protected:
	virtual void animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) = 0;
	String getPreferenceKey(EffectParameter parameter, const char *key);

  private:
	uint32_t lastUpdate = 0;
	uint32_t duration;
	float progress = 0.0f;
	bool running = false;
	EffectDirection direction = EffectDirection::ON;
};

class StaticEffect : public Effect {
  public:
	StaticEffect()
		: Effect(EffectType::NONE, 0) {}

	// These don't do anything
	bool setParameter(const EffectParameter parameter, const String &value) {}
	void load(Preferences &prefs, const char *key) {}
	void save(Preferences &prefs, const char *key) {}

  protected:
	void animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) override;
};

class LightsaberEffect : public Effect {
  public:
	LightsaberEffect(uint32_t duration, uint16_t startLedIndex)
		: Effect(EffectType::LIGHTSABER, duration), startLedIndex(startLedIndex), currentStartLedIndex(startLedIndex) {}

	uint16_t getStartLedIndex() const { return this->startLedIndex; }
	void setStartLedIndex(uint16_t startLedIndex);

	bool setParameter(const EffectParameter parameter, const String &value) override;
	void load(Preferences &prefs, const char *key) override;
	void save(Preferences &prefs, const char *key) override;

  protected:
	void animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) override;

  private:
	uint16_t getLongerSectionLength(uint16_t numLeds);

	uint16_t currentStartLedIndex;
	uint16_t startLedIndex;
};

class FadeEffect : public Effect {
  public:
	FadeEffect(uint32_t duration)
		: Effect(EffectType::FADE, duration) {}

  protected:
	void animate(CRGB *leds, uint16_t numLeds, const CRGB &onColor, float progress) override;
};