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

	this->wsHandler.attach(this->server);

	ElegantOTA.begin(&this->server);

	this->server.begin();
}

void WebServerManager::loop() {
	this->wsHandler.loop();

	ElegantOTA.loop();
}
