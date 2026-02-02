#include "LEDController.h"
#include "utils.h"

void LEDController::begin() {
	this->switchInput.begin(INPUT_PULLUP);
	this->powerSupply.begin();

	FastLED.addLeds<WS2811, SHORT_STRIP_DATA_PIN>(this->leds, LED_ARRAY_COUNT);
	// FIXME: Use this after cutting leds
	// FastLED.addLeds<WS2811, SHORT_STRIP_DATA_PIN>(this->leds, SHORT_STRIP_SECTION_COUNT);
	// FastLED.addLeds<WS2811, LONG_STRIP_DATA_PIN>(this->leds, SHORT_STRIP_SECTION_COUNT + STRIP_GAP_SECTION_COUNT, LONG_STRIP_SECTION_COUNT);

	FastLED.setMaxRefreshRate(MAX_REFRESH_RATE);
	FastLED.setDither(DISABLE_DITHER);

	this->load();
}

void LEDController::update() {
	switch (this->mode) {
	case LEDControllerMode::SWITCH:
		this->updateSwitchMode();
		break;
	case LEDControllerMode::SETTING:
		this->updateSettingMode();
		break;
	}
}

void LEDController::updateSwitchMode() {
	if (this->switchInput.isOff() && !this->currentEffect->isRunning()) {
		this->powerSupply.off();
	}

	if (this->switchInput.getSingleDebouncedStateChange()) {
		const auto powerSupplyWasOn = this->powerSupply.isOn();
		this->powerSupply.on();

		// Wait for power supply to turn on
		if (this->switchInput.isOn() && !powerSupplyWasOn) {
			delay(100);
		}

		if (this->switchInput.isOn()) {
			if (!this->currentEffect->isRunning()) {
				this->currentEffect = this->config.onEffect;
				this->currentEffect->reset(EffectDirection::ON);
			}

			this->currentEffect->start(EffectDirection::ON);
		} else {
			if (!this->currentEffect->isRunning()) {
				this->currentEffect = this->config.offEffect;
				this->currentEffect->reset(EffectDirection::OFF);
			}

			this->currentEffect->start(EffectDirection::OFF);
		}
	}

	auto updated = this->currentEffect->update(this->leds, LED_ARRAY_COUNT, this->config.color);

	if (updated) {
		FastLED.show();
	}
}

void LEDController::updateSettingMode() {
	const auto updated = this->previewer.update(this->leds, LED_ARRAY_COUNT);

	if (updated) {
		FastLED.show();
	}
}

void LEDController::setMode(LEDControllerMode mode) {
	if (this->mode == mode) return;

	this->exitMode(this->mode);
	this->mode = mode;
	this->enterMode(this->mode);
}

void LEDController::exitMode(LEDControllerMode mode) {
	fill_solid(this->leds, LED_ARRAY_COUNT, CRGB::Black);
}

void LEDController::enterMode(LEDControllerMode mode) {
	switch (mode) {
	case LEDControllerMode::SWITCH:
		this->onSwitchModeEntered();
		break;
	case LEDControllerMode::SETTING:
		this->onSettingModeEntered();
		break;
	}
}

void LEDController::onSwitchModeEntered() {
	if (this->switchInput.isOff()) {
		fill_solid(this->leds, LED_ARRAY_COUNT, CRGB::Black);
		this->powerSupply.off();
		return;
	}

	this->powerSupply.on();
	this->currentEffect = this->config.onEffect;
	this->currentEffect->reset(EffectDirection::ON);
	this->currentEffect->start(EffectDirection::ON);
}

#define SETTING_MODE_BLINK_DELAY 300

void LEDController::onSettingModeEntered() {
	this->draft = this->config;

	// In setting mode the power supply is always on
	this->powerSupply.on();

	// Blink the first led to signal entering into setting modeí
	for (size_t i = 0; i < 2; i++) {
		this->leds[0] = this->config.color;
		FastLED.show();
		delay(SETTING_MODE_BLINK_DELAY);
		this->leds[0] = CRGB::Black;
		FastLED.show();
		delay(SETTING_MODE_BLINK_DELAY);
	}
}

void LEDController::load() {
	this->preferences.begin(LED_PREFERENCES_NAMESPACE, true);

	const auto colorKey = String(uint8_t(LEDControllerParameter::COLOR));
	CRGB color = this->preferences.getUInt(colorKey.c_str(), DEFAULT_COLOR);
	this->setColor(color);

	const auto onEffectKey = String(uint8_t(LEDControllerParameter::ON_EFFECT));
	const auto onEffectType = (EffectType)this->preferences.getUChar(onEffectKey.c_str(), uint8_t(EffectType::LIGHTSABER));
	const auto offEffectKey = String(uint8_t(LEDControllerParameter::OFF_EFFECT));
	const auto offEffectType = (EffectType)this->preferences.getUChar(offEffectKey.c_str(), uint8_t(EffectType::LIGHTSABER));

	Effect *onEffect = EffectFactory::getFromEffectTypeOn(onEffectType);
	Effect *offEffect = EffectFactory::getFromEffectTypeOff(offEffectType);

	onEffect->load(this->preferences, ON_EFFECT_PREFERENCE_KEY);
	offEffect->load(this->preferences, OFF_EFFECT_PREFERENCE_KEY);

	this->setOnEffect(onEffect);
	this->setOffEffect(offEffect);

	this->preferences.end();
}

void LEDController::save() {
	this->preferences.begin(LED_PREFERENCES_NAMESPACE);

	this->config = this->draft;

	const auto colorKey = String(uint8_t(LEDControllerParameter::COLOR));
	const auto colorValue = this->config.color.as_uint32_t();

	const auto onEffectKey = String(uint8_t(LEDControllerParameter::ON_EFFECT));
	const auto onEffectTypeValue = uint8_t(this->config.onEffect->type);

	const auto offEffectKey = String(uint8_t(LEDControllerParameter::OFF_EFFECT));
	const auto offEffectTypeValue = uint8_t(this->config.offEffect->type);

	this->preferences.putUInt(colorKey.c_str(), colorValue);
	this->preferences.putUChar(onEffectKey.c_str(), onEffectTypeValue);
	this->preferences.putUChar(offEffectKey.c_str(), offEffectTypeValue);

	this->config.onEffect->save(this->preferences, ON_EFFECT_PREFERENCE_KEY);
	this->config.offEffect->save(this->preferences, OFF_EFFECT_PREFERENCE_KEY);

	this->preferences.end();
}

void LEDController::setColor(const CRGB &color) {
	switch (this->mode) {
	case LEDControllerMode::SETTING:
		this->draft.color = color;
		this->previewer.previewColor(color);
		break;
	case LEDControllerMode::SWITCH:
		this->config.color = color;
		this->previewer.setColor(color);
		break;
	}
}

void LEDController::setOnEffect(Effect *effect) {
	this->getConfig().onEffect = effect;
}

void LEDController::setOffEffect(Effect *effect) {
	this->getConfig().offEffect = effect;
}

void LEDController::setOnEffectParameter(EffectParameter parameter, const String &value) {
	this->getConfig().onEffect->setParameter(parameter, value);

	if (this->mode == LEDControllerMode::SETTING) {
		this->previewEffectParameter(parameter, value);
	}
}

void LEDController::setOffEffectParameter(EffectParameter parameter, const String &value) {
	this->getConfig().offEffect->setParameter(parameter, value);

	if (this->mode == LEDControllerMode::SETTING) {
		this->previewEffectParameter(parameter, value);
	}
}

void LEDController::previewOnEffect() {
	this->previewer.previewEffect(this->draft.onEffect, EffectDirection::ON);
}

void LEDController::previewOffEffect() {
	this->previewer.previewEffect(this->draft.offEffect, EffectDirection::OFF);
}

LEDConfig &LEDController::getConfig() {
	switch (this->mode) {
	case LEDControllerMode::SWITCH:
		return this->config;
	case LEDControllerMode::SETTING:
		return this->draft;
	}
}

void LEDController::previewEffectParameter(EffectParameter parameter, const String &value) {
	if (this->mode != LEDControllerMode::SETTING) return;

	switch (parameter) {
	case EffectParameter::START_LED_INDEX:
		// TODO: This clamp is duplacted in the previewer
		const auto index = std::clamp((int)value.toInt(), 0, LED_ARRAY_COUNT - 1);
		this->previewer.previewIndex(index);
		break;
	}
}

void LEDController::reverseShortStripSection() {
	for (size_t i = 0; i < SHORT_STRIP_SECTION_COUNT / 2; i++) {
		CRGB tmp = this->leds[i];
		this->leds[i] = this->leds[SHORT_STRIP_SECTION_COUNT - i - 1];
		this->leds[SHORT_STRIP_SECTION_COUNT - i - 1] = tmp;
	}
}
