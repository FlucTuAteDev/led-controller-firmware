#pragma once
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "Effect.h"
#include <FastLED.h>
#include <optional>

#define LONG_STRIP_DATA_PIN 32
#define SHORT_STRIP_DATA_PIN 26

#define POWER_SUPPLY_PIN 18

#define SWITCH_INPUT_PIN 19

// TODO: Ezeket definiálni
#define LONG_STRIP_SECTION_COUNT 80
#define SHORT_STRIP_SECTION_COUNT 30
#define STRIP_GAP_SECTION_COUNT 11

#define LED_ARRAY_COUNT (LONG_STRIP_SECTION_COUNT + SHORT_STRIP_SECTION_COUNT + STRIP_GAP_SECTION_COUNT)
/*
	From the datasheet sending 1 bit in the worst case takes 2us. Each LED controller (WS2811) takes 24 bits of data
	(8 bits for red, green and blue each) + there is a minimum of 280us of reset time after the bits have been.
	From that and multiplying by the number of leds on a 5m strip we get this value
	THIS IS WORST CASE SCENARIO! The highest refresh rate could probably be much better than this
*/
#define MAX_REFRESH_RATE 164

#define LED_PREFERENCES_NAMESPACE "led"
#define ON_EFFECT_PREFERENCE_KEY "on"
#define OFF_EFFECT_PREFERENCE_KEY "off"

#define DEFAULT_BRIGHTNESS 20
#define DEFAULT_COLOR_TEMPERATURE 127
#define BRIGHNESS_STEP_INTERVAL_MS 10
#define COLOR_TEMPERATURE_STEP_INTERVAL_MS 10

enum LEDControllerMode {
	SWITCH,
	SETTING
};

enum LEDControllerParameter {
	BRIGHTNESS,
	COLOR_TEMPERATURE,
	ON_EFFECT,
	OFF_EFFECT
};

class LEDController {
  public:
	LEDController();

	void begin();
	void update();

	uint8_t getBrightness() const { return FastLED.getBrightness(); }
	void setBrightness(uint8_t brightness);

	uint8_t getTargetBrigtness() const { return this->targetBrightness; }
	void setTargetBrightness(uint8_t brightness);

	uint8_t getColdBrightness() const { return this->onColor.r; }
	void setColdBrightness(uint8_t brightness);

	uint8_t getTargetColdBrightness() const { return this->targetColdBrightness; }
	void setTargetColdBrightness(uint8_t brightness);

	LEDControllerMode getMode() { return this->mode; }
	void setMode(LEDControllerMode mode);

	void setOnEffect(Effect *effect);
	void setOffEffect(Effect *effect);

	Effect *getOnEffect() { return this->onEffect; }
	Effect *getOffEffect() { return this->offEffect; }

  private:
	void reverseShortStripSection();

	void load();
	void save();

	void updateSwitchMode();
	void updateSettingMode();

	bool updateBrightness();
	bool updateColorTemperature();

	void exitMode(LEDControllerMode mode);
	void enterMode(LEDControllerMode mode);

	void onSwitchModeEntered();
	void onSettingModeEntered();

	void setPreviewEffect(Effect *effect, EffectDirection direction);

	void turnOnAllLEDs();
	void clearLEDs();

	CRGB leds[LED_ARRAY_COUNT];

	LEDControllerMode mode = LEDControllerMode::SWITCH;

	bool firstBrightnessUpdate = true;
	uint8_t targetBrightness;
	uint8_t targetColdBrightness;
	uint32_t lastBrightnessStepTime = 0;
	uint32_t lastColorTemperatureStepTime = 0;
	CRGB onColor = CRGB::Yellow;

	Effect *onEffect;
	Effect *offEffect;
	Effect *currentEffect;
	Effect *previewEffect = nullptr;

	DigitalInput switchInput = DigitalInput(SWITCH_INPUT_PIN, LOW);
	DigitalOutput powerSupply = DigitalOutput(POWER_SUPPLY_PIN);

	Preferences preferences;
};