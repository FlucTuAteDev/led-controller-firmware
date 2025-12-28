#include "LEDController.h"
#include "EffectFactory.h"
#include "utils.h"

LEDController::LEDController()
	: onEffect(&EffectFactory::lightsaberOn), offEffect(&EffectFactory::lightsaberOff), currentEffect(onEffect) {}

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

	FastLED.setBrightness(this->targetBrightness);
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
				this->currentEffect = this->onEffect;
				this->currentEffect->reset(EffectDirection::ON);
			}

			this->currentEffect->start(EffectDirection::ON);
		} else {
			if (!this->currentEffect->isRunning()) {
				this->currentEffect = this->offEffect;
				this->currentEffect->reset(EffectDirection::OFF);
			}

			this->currentEffect->start(EffectDirection::OFF);
		}
	}

	auto updated = this->currentEffect->update(this->leds, LED_ARRAY_COUNT, this->onColor);

	if (updated) {
		FastLED.show();
	}
}

void LEDController::updateSettingMode() {
	auto updatedPreviewEffect = false;
	auto previewEffectFinished = false;
	if (this->previewEffect) {
		updatedPreviewEffect = this->previewEffect->update(this->leds, LED_ARRAY_COUNT, this->onColor);
	}

	const auto updatedBrightness = this->updateBrightness();
	const auto updatedColorTemperature = this->updateColorTemperature();

	const auto updated = updatedPreviewEffect || updatedBrightness || updatedColorTemperature;
	if (updated) {
		FastLED.show();
	}

	if (this->previewEffect && !this->previewEffect->isRunning()) {
		this->previewEffect = nullptr;
		delay(300);
		this->turnOnAllLEDs();
		FastLED.show();
	}
}

bool LEDController::updateBrightness() {
	auto currentBrightness = FastLED.getBrightness();
	auto difference = this->targetBrightness - currentBrightness;
	if (difference == 0) return false;

	auto currentTime = millis();
	auto brighnessStepElapsedTime = currentTime - this->lastBrightnessStepTime;
	if (brighnessStepElapsedTime < BRIGHNESS_STEP_INTERVAL_MS) return false;

	FastLED.setBrightness(currentBrightness + sgn(difference));
	this->lastBrightnessStepTime = currentTime;

	return true;
}

bool LEDController::updateColorTemperature() {
	// TODO: Is the red channel the cold one?
	const auto currentColdBrightness = this->getColdBrightness();
	const auto difference = this->targetColdBrightness - currentColdBrightness;
	if (difference == 0) return false;

	const auto currentTime = millis();
	const auto elapsedTime = currentTime - this->lastColorTemperatureStepTime;
	if (elapsedTime < COLOR_TEMPERATURE_STEP_INTERVAL_MS) return false;

	const auto newColdBrightness = currentColdBrightness + sgn(difference);
	this->setColdBrightness(newColdBrightness);

	// TODO:
	this->turnOnAllLEDs();

	return true;
}

void LEDController::setMode(LEDControllerMode mode) {
	if (this->mode == mode) return;

	this->exitMode(this->mode);
	this->mode = mode;
	this->enterMode(mode);
}

void LEDController::exitMode(LEDControllerMode mode) {
	this->clearLEDs();
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
		this->clearLEDs();
		this->powerSupply.off();
		return;
	}

	this->powerSupply.on();
	this->currentEffect = this->onEffect;
	this->currentEffect->reset(EffectDirection::ON);
	this->currentEffect->start(EffectDirection::ON);
}

#define SETTING_MODE_BLINK_DELAY 300

void LEDController::onSettingModeEntered() {
	// In setting mode the power supply is always on
	this->powerSupply.on();

	// Blink the first led to signal entering into setting modeí
	this->leds[0] = this->onColor;
	FastLED.show();
	delay(SETTING_MODE_BLINK_DELAY);
	this->leds[0] = CRGB::Black;
	FastLED.show();
	delay(SETTING_MODE_BLINK_DELAY);
	this->leds[0] = this->onColor;
	FastLED.show();
	delay(SETTING_MODE_BLINK_DELAY);
	this->leds[0] = CRGB::Black;
	FastLED.show();
	delay(SETTING_MODE_BLINK_DELAY);

	this->turnOnAllLEDs();
	FastLED.show();
}

void LEDController::load() {
	this->preferences.begin(LED_PREFERENCES_NAMESPACE, true);

	this->targetBrightness = this->preferences.getUChar(String(LEDControllerParameter::BRIGHTNESS).c_str(), DEFAULT_BRIGHTNESS);
	this->setBrightness(this->targetBrightness);
	this->targetColdBrightness = this->preferences.getUChar(String(LEDControllerParameter::COLOR_TEMPERATURE).c_str(), DEFAULT_COLOR_TEMPERATURE);
	this->setColdBrightness(this->targetColdBrightness);
	const auto onEffectType = (EffectType)this->preferences.getUChar(String(LEDControllerParameter::ON_EFFECT).c_str(), EffectType::LIGHTSABER);
	const auto offEffectType = (EffectType)this->preferences.getUChar(String(LEDControllerParameter::OFF_EFFECT).c_str(), EffectType::LIGHTSABER);

	Effect *onEffect = EffectFactory::getFromEffectTypeOn(onEffectType);
	Effect *offEffect = EffectFactory::getFromEffectTypeOff(offEffectType);

	onEffect->load(this->preferences, ON_EFFECT_PREFERENCE_KEY);
	offEffect->load(this->preferences, OFF_EFFECT_PREFERENCE_KEY);

	this->preferences.end();
}

void LEDController::save() {
	this->preferences.begin(LED_PREFERENCES_NAMESPACE);

	this->preferences.putUChar(String(LEDControllerParameter::BRIGHTNESS).c_str(), this->targetBrightness);
	this->preferences.putUChar(String(LEDControllerParameter::ON_EFFECT).c_str(), this->onEffect->type);
	this->preferences.putUChar(String(LEDControllerParameter::OFF_EFFECT).c_str(), this->offEffect->type);

	onEffect->save(this->preferences, ON_EFFECT_PREFERENCE_KEY);
	offEffect->save(this->preferences, OFF_EFFECT_PREFERENCE_KEY);

	this->preferences.end();
}

void LEDController::setBrightness(uint8_t brightness) {
	FastLED.setBrightness(brightness);
}

void LEDController::setTargetBrightness(uint8_t brightness) {
	this->targetBrightness = brightness;
}

void LEDController::setColdBrightness(uint8_t brightness) {
	this->onColor.r = brightness;
	this->onColor.g = 255 - brightness;
}

void LEDController::setTargetColdBrightness(uint8_t brightness) {
	this->targetColdBrightness = brightness;
}

void LEDController::turnOnAllLEDs() {
	for (uint16_t i = 0; i < LED_ARRAY_COUNT; i++) {
		this->leds[i] = this->onColor;
	}
}

void LEDController::clearLEDs() {
	for (uint16_t i = 0; i < LED_ARRAY_COUNT; i++) {
		this->leds[i] = CRGB::Black;
	}
}

void LEDController::setOnEffect(Effect *effect) {
	this->setPreviewEffect(effect, EffectDirection::ON);
	this->onEffect = effect;
}

void LEDController::setOffEffect(Effect *effect) {
	this->setPreviewEffect(effect, EffectDirection::OFF);
	this->offEffect = effect;
}

void LEDController::setPreviewEffect(Effect *effect, EffectDirection direction) {
	this->previewEffect = effect;
	this->previewEffect->reset(direction);
	this->previewEffect->start(direction);
}

void LEDController::reverseShortStripSection() {
	for (size_t i = 0; i < SHORT_STRIP_SECTION_COUNT / 2; i++) {
		CRGB tmp = this->leds[i];
		this->leds[i] = this->leds[SHORT_STRIP_SECTION_COUNT - i - 1];
		this->leds[SHORT_STRIP_SECTION_COUNT - i - 1] = tmp;
	}
}
