#include "WiFiConnectionHandler.h"
#include "Secrets.h"
#include <WiFi.h>

void WiFiConnectionHandler::begin() {
	WiFi.mode(WIFI_AP_STA);

	this->setupAP();
	this->connect();
}

uint8_t WiFiConnectionHandler::connect() {
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	return WiFi.waitForConnectResult();
}

void WiFiConnectionHandler::setupAP() {
	WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 254), IPAddress(255, 255, 255, 0));
	WiFi.softAP(AP_SSID, AP_PASSWORD);
}