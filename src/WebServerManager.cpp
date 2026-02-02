#include "WebServerManager.h"
#include <ElegantOTA.h>
#include <LittleFS.h>

WebServerManager::WebServerManager(LEDController &ledController)
	: server(80), wsHandler(ledController), ledController(ledController) {
}

void WebServerManager::begin() {
	LittleFS.begin();

	this->server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
	this->server.serveStatic("/assets", LittleFS, "/assets");

	this->server.on("/api/state", [this](AsyncWebServerRequest *request) {
		String state = this->getState();
		request->send(200, "text/plain", state);
	});

	this->wsHandler.attach(this->server);

	ElegantOTA.begin(&this->server);

	this->server.begin();
}

void WebServerManager::loop() {
	this->wsHandler.loop();

	ElegantOTA.loop();
}

String WebServerManager::getState() const {
	char buf[40];

	const auto onEffect = this->ledController.getOnEffect();
	const auto offEffect = this->ledController.getOffEffect();
	const auto color = this->ledController.getColor().as_uint32_t();

	sprintf(
		buf,
		"%u %d %d %d %d %d %d",
		color,
		onEffect->type,
		onEffect->getDuration(),
		onEffect->type == EffectType::LIGHTSABER ? static_cast<const LightsaberEffect *>(onEffect)->getStartLedIndex() : -1,
		offEffect->type,
		offEffect->getDuration(),
		offEffect->type == EffectType::LIGHTSABER ? static_cast<const LightsaberEffect *>(offEffect)->getStartLedIndex() : -1);

	return String(buf);
}