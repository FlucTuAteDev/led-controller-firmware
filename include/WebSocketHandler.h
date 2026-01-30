#pragma once

#include "LEDController.h"
#include "LedConfig.h"
#include "Previewer.h"
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>

enum class RequestType {
	GET,
	SET
};

enum class Command {
	SET_COLOR,
	SET_ON_EFFECT,
	SET_OFF_EFFECT,
	SET_ON_EFFECT_PARAMETER,
	SET_OFF_EFFECT_PARAMETER,
	PREVIEW_ON_EFFECT,
	PREVIEW_OFF_EFFECT,
	SAVE
};

class WebSocketHandler {
  public:
	WebSocketHandler(LEDController &ledController);
	void attach(AsyncWebServer &server);
	void loop();

  private:
	LEDConfig draft;
	Previewer previewer;
	LEDController &ledController;
	AsyncWebSocketMessageHandler wsHandler;
	AsyncWebSocket ws;

	void handleMessage(AsyncWebSocketClient *client, const String &message);
};