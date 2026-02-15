#include <Arduino.h>

#include "LEDController.h"
#include "Secrets.h"
#include "WebServerManager.h"
#include "WiFiConnectionHandler.h"
#include <WiFi.h>

WiFiConnectionHandler connectionHandler;

LEDController ledController;
WebServerManager webServerManager(ledController);

void WiFiEvent(arduino_event_id_t event) {
	switch (event) {
	case ARDUINO_EVENT_WIFI_STA_GOT_IP:
		// Begin handles calling it multiple times
		webServerManager.begin();
		break;
	}
}

void setup() {
	Serial.begin(115200);

	ledController.begin();

	WiFi.onEvent(WiFiEvent);
	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
	ledController.update();

	if (WiFi.isConnected()) {
		webServerManager.loop();
	}
}
