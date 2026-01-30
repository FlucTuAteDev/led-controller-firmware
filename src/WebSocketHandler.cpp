#include "WebSocketHandler.h"
#include "EffectFactory.h"
#include <algorithm>

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
	const auto commandSeparator = message.indexOf(' ');
	const Command command = static_cast<Command>(message.substring(0, commandSeparator).toInt());

	if (command == Command::SET_ON_EFFECT || command == Command::SET_OFF_EFFECT) {
		const auto effectTypeSeparator = message.indexOf(' ', commandSeparator + 1);
		const auto effectType = static_cast<EffectType>(message.substring(commandSeparator + 1, effectTypeSeparator).toInt());

		if (command == Command::SET_ON_EFFECT) {
			const auto effect = EffectFactory::getFromEffectTypeOn(effectType);
			this->ledController.setOnEffect(effect);
		} else {
			const auto effect = EffectFactory::getFromEffectTypeOff(effectType);
			this->ledController.setOffEffect(effect);
		}
	} else if (command == Command::SET_ON_EFFECT_PARAMETER || command == Command::SET_OFF_EFFECT_PARAMETER) {
		const auto effectParameterSeparator = message.indexOf(' ', commandSeparator + 1);
		const auto effectParameter = static_cast<EffectParameter>(message.substring(commandSeparator + 1, effectParameterSeparator).toInt());
		const auto effectParameterValue = message.substring(effectParameterSeparator + 1);

		if (command == Command::SET_ON_EFFECT_PARAMETER) {
			this->ledController.setOnEffectParameter(effectParameter, effectParameterValue);
		} else {
			this->ledController.setOffEffectParameter(effectParameter, effectParameterValue);
		}
	} else if (command == Command::SET_COLOR) {
		const auto colorValue = message.substring(commandSeparator + 1).toInt();

		this->ledController.setColor(colorValue);
	} else if (command == Command::PREVIEW_ON_EFFECT) {
		this->ledController.previewOnEffect();
	} else if (command == Command::PREVIEW_OFF_EFFECT) {
		this->ledController.previewOffEffect();
	} else if (command == Command::SAVE) {
		this->ledController.save();
	}
}
