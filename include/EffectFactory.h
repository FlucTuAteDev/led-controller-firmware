#include "Effect.h"

class EffectFactory {
  public:
	static Effect *getFromEffectTypeOn(EffectType type);
	static Effect *getFromEffectTypeOff(EffectType type);

	static StaticEffect staticOn;
	static StaticEffect staticOff;

	static LightsaberEffect lightsaberOn;
	static LightsaberEffect lightsaberOff;
};