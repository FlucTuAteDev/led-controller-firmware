#pragma once
#include "LEDController.h"
#include "WebSocketHandler.h"
#include <ESPAsyncWebServer.h>

class WebServerManager {
  public:
	WebServerManager(LEDController &ledController);
	void begin();
	void loop();

  private:
	AsyncWebServer server;
	WebSocketHandler wsHandler;
	LEDController &ledController;
};