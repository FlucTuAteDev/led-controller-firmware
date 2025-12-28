#include "EffectFactory.h"

LightsaberEffect EffectFactory::lightsaberOn = LightsaberEffect(3000, 0);
LightsaberEffect EffectFactory::lightsaberOff = LightsaberEffect(3000, 0);

StaticEffect EffectFactory::staticOn = StaticEffect();
StaticEffect EffectFactory::staticOff = StaticEffect();

Effect *EffectFactory::getFromEffectTypeOn(EffectType type) {
	// TODO:
	switch (type) {
	case EffectType::LIGHTSABER:
		return &EffectFactory::lightsaberOn;
	}

	return &EffectFactory::staticOn;
}

Effect *EffectFactory::getFromEffectTypeOff(EffectType type) {
	switch (type) {
	case EffectType::LIGHTSABER:
		return &EffectFactory::lightsaberOff;
	}

	return &EffectFactory::staticOff;
}
