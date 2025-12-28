#include "WebSocketHandler.h"
#include "EffectFactory.h"
#include <algorithm>

#define EFFECT_TYPE_PARAMETER -1

WebSocketHandler::WebSocketHandler(LEDController &ledController)
	: ledController(ledController), wsHandler(), ws("/ws", wsHandler.eventHandler()) {
	wsHandler.onConnect([this](AsyncWebSocket *server, AsyncWebSocketClient *client) {
		// Only enter setting mode on the first client connection
		if (server->getClients().size() > 1) return;

		this->ledController.setMode(LEDControllerMode::SETTING);
	});

	wsHandler.onMessage([this](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
		String msg((char *)data);
		this->handleMessage(client, msg);

		server->textAll(data, len);
	});

	wsHandler.onDisconnect([this](AsyncWebSocket *server, uint32_t clientId) {
		// Only exit setting mode on the last disconnected connection
		if (server->getClients().size() > 1) return;

		this->ledController.setMode(LEDControllerMode::SWITCH);
	});
}

void WebSocketHandler::attach(AsyncWebServer &server) {
	server.addHandler(&(this->ws));
}

void WebSocketHandler::loop() {
	EVERY_N_SECONDS(1) {
		this->ws.cleanupClients();
	}
}

void WebSocketHandler::handleMessage(AsyncWebSocketClient *client, const String &message) {
	const auto parameterSeparator = message.indexOf(' ');
	const LEDControllerParameter parameter = (LEDControllerParameter)message.substring(0, parameterSeparator).toInt();

	if (parameter == LEDControllerParameter::ON_EFFECT || parameter == LEDControllerParameter::OFF_EFFECT) {
		const auto effectParameterSeparator = message.indexOf(' ', parameterSeparator + 1);
		const int effectParameter = message.substring(parameterSeparator + 1, effectParameterSeparator).toInt();
		const auto effectParameterValue = message.substring(effectParameterSeparator + 1);

		if (effectParameter == EFFECT_TYPE_PARAMETER) {
			if (parameter == LEDControllerParameter::ON_EFFECT) {
				const auto effect = EffectFactory::getFromEffectTypeOn((EffectType)effectParameterValue.toInt());
				this->ledController.setOnEffect(effect);
			} else {
				const auto effect = EffectFactory::getFromEffectTypeOff((EffectType)effectParameterValue.toInt());
				this->ledController.setOffEffect(effect);
			}
		} else {
			if (parameter == LEDControllerParameter::ON_EFFECT) {
				this->ledController.getOnEffect()->setParameter((EffectParameter)effectParameter, effectParameterValue);
			} else {
				this->ledController.getOffEffect()->setParameter((EffectParameter)effectParameter, effectParameterValue);
			}
		}
	} else {
		const auto parameterValue = message.substring(parameterSeparator + 1);

		if (parameter == LEDControllerParameter::BRIGHTNESS) {
			// Brightness has to be at least 1
			const auto brightness = std::clamp((int)parameterValue.toInt(), 1, 255);
			this->ledController.setTargetBrightness(brightness);
		} else if (parameter == LEDControllerParameter::COLOR_TEMPERATURE) {
			const auto coldLEDBrightness = std::clamp((int)parameterValue.toInt(), 0, 255);
			this->ledController.setTargetColdBrightness(coldLEDBrightness);
		}
	}
}
