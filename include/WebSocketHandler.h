#pragma once

#include "LEDController.h"
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>

enum RequestType {
	GET,
	SET
};

class WebSocketHandler {
  public:
	WebSocketHandler(LEDController &ledController);
	void attach(AsyncWebServer &server);
	void loop();

  private:
	LEDController &ledController;
	AsyncWebSocketMessageHandler wsHandler;
	AsyncWebSocket ws;

	void handleMessage(AsyncWebSocketClient *client, const String &message);
};