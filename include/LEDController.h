#pragma once
#include "Constants.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "Effect.h"
#include "Previewer.h"
#include <FastLED.h>
#include <optional>

#define LONG_STRIP_DATA_PIN 32
#define SHORT_STRIP_DATA_PIN 26

#define POWER_SUPPLY_PIN 18

#define SWITCH_INPUT_PIN 19

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

enum class LEDControllerMode {
	SWITCH,
	SETTING
};

enum class LEDControllerParameter {
	BRIGHTNESS,
	COLOR_TEMPERATURE,
	ON_EFFECT,
	OFF_EFFECT,
	ON_EFFECT_PARAMETER,
	OFF_EFFECT_PARAMETER,
};

class LEDController {
  public:
	LEDController();

	void begin();
	void update();

	uint8_t getBrightness() const { return this->brightness; }
	void setBrightness(uint8_t brightness);

	uint8_t getColdBrightness() const { return this->onColor.r; }
	void setColdBrightness(uint8_t brightness);

	LEDControllerMode getMode() { return this->mode; }
	void setMode(LEDControllerMode mode);

	void setOnEffect(Effect *effect);
	void setOffEffect(Effect *effect);

	void setOnEffectParameter(EffectParameter parameter, const String &value);
	void setOffEffectParameter(EffectParameter parameter, const String &value);

	Effect *getOnEffect() { return this->onEffect; }
	Effect *getOffEffect() { return this->offEffect; }

  private:
	void previewEffectParameter(EffectParameter parameter, const String &value);
	void reverseShortStripSection();

	void load();
	void save();

	void updateSwitchMode();
	void updateSettingMode();

	void exitMode(LEDControllerMode mode);
	void enterMode(LEDControllerMode mode);

	void onSwitchModeEntered();
	void onSettingModeEntered();

	void clearLEDs();

	CRGB leds[LED_ARRAY_COUNT];

	LEDControllerMode mode = LEDControllerMode::SWITCH;

	Previewer previewer;

	uint8_t brightness = DEFAULT_BRIGHTNESS;
	CRGB onColor = CRGB::Yellow;

	Effect *onEffect;
	Effect *offEffect;
	Effect *currentEffect;
	Effect *previewEffect = nullptr;

	DigitalInput switchInput = DigitalInput(SWITCH_INPUT_PIN, LOW);
	DigitalOutput powerSupply = DigitalOutput(POWER_SUPPLY_PIN);

	Preferences preferences;
};