#pragma once
#include "Constants.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "Effect.h"
#include "EffectFactory.h"
#include "LedConfig.h"
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

#define DEFAULT_COLOR 0x7F7F00

enum class LEDControllerMode {
	SWITCH,
	SETTING
};

enum class LEDControllerParameter {
	COLOR,
	ON_EFFECT,
	OFF_EFFECT,
	ON_EFFECT_PARAMETER,
	OFF_EFFECT_PARAMETER,
};

class LEDController {
  public:
	void begin();
	void update();

	const CRGB &getColor() const { return this->config.color; }
	void setColor(const CRGB &color);

	LEDControllerMode getMode() const { return this->mode; }
	void setMode(LEDControllerMode mode);

	const Effect *const getOnEffect() const { return this->config.onEffect; }
	const Effect *const getOffEffect() const { return this->config.offEffect; }

	void setOnEffect(Effect *effect);
	void setOffEffect(Effect *effect);

	void setOnEffectParameter(EffectParameter parameter, const String &value);
	void setOffEffectParameter(EffectParameter parameter, const String &value);

	void previewOnEffect();
	void previewOffEffect();

	void save();

  private:
	LEDConfig &getConfig();

	void previewEffectParameter(EffectParameter parameter, const String &value);
	void reverseShortStripSection();

	void load();

	void updateSwitchMode();
	void updateSettingMode();

	void exitMode(LEDControllerMode mode);
	void enterMode(LEDControllerMode mode);

	void onSwitchModeEntered();
	void onSettingModeEntered();

	CRGB leds[LED_ARRAY_COUNT];

	LEDControllerMode mode = LEDControllerMode::SWITCH;

	Previewer previewer;

	LEDConfig config = {
		.color = CRGB(255, 0, 0),
		.onEffect = &EffectFactory::lightsaberOn,
		.offEffect = &EffectFactory::lightsaberOff,
	};

	LEDConfig draft = config;

	Effect *currentEffect = this->config.onEffect;

	DigitalInput switchInput = DigitalInput(SWITCH_INPUT_PIN, LOW);
	DigitalOutput powerSupply = DigitalOutput(POWER_SUPPLY_PIN);

	Preferences preferences;
};