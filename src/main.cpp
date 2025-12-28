#include <Arduino.h>

#include "LEDController.h"
#include "WebServerManager.h"
#include "WiFiConnectionHandler.h"
#include <WiFi.h>

WiFiConnectionHandler connectionHandler;

LEDController ledController;
WebServerManager webServerManager(ledController);

void setup() {
	Serial.begin(115200);
	// Wait for serial connection to establish
	delay(2000);

	ledController.begin();

	const auto connectionResult = connectionHandler.connect();
	if (connectionResult == WL_CONNECTED) {
		webServerManager.begin();
	}
}

void loop() {
	ledController.update();

	webServerManager.loop();
}
