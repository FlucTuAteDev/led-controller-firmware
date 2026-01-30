#pragma once

#include "Effect.h"
// #include "EffectParameters.h"

struct LEDConfig {
	CRGB color;

	Effect *onEffect;
	Effect *offEffect;
};