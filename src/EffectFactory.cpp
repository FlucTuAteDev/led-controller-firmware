#include "EffectFactory.h"

FadeEffect EffectFactory::fadeOn = FadeEffect(3000);
FadeEffect EffectFactory::fadeOff = FadeEffect(3000);

LightsaberEffect EffectFactory::lightsaberOn = LightsaberEffect(3000, 0);
LightsaberEffect EffectFactory::lightsaberOff = LightsaberEffect(3000, 0);

StaticEffect EffectFactory::staticOn = StaticEffect();
StaticEffect EffectFactory::staticOff = StaticEffect();

Effect *EffectFactory::getFromEffectTypeOn(EffectType type) {
	// TODO:
	switch (type) {
	case EffectType::LIGHTSABER:
		return &EffectFactory::lightsaberOn;
	case EffectType::FADE:
		return &EffectFactory::fadeOn;
	}

	return &EffectFactory::staticOn;
}

Effect *EffectFactory::getFromEffectTypeOff(EffectType type) {
	switch (type) {
	case EffectType::LIGHTSABER:
		return &EffectFactory::lightsaberOff;
	case EffectType::FADE:
		return &EffectFactory::fadeOff;
	}

	return &EffectFactory::staticOff;
}
