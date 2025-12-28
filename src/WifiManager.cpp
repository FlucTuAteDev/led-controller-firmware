#include "WifiManager.h"
#include <WiFi.h>

#include <ESPAsyncWebServer.h>

void WifiManager::init() {
	WiFi.persistent(false);
}

void WifiManager::connect() {
	this->wifiPreferences.begin(WifiManager::WIFI_PREFERENCES_NAME);

	if (!this->hasSavedWifi(this->wifiPreferences)) {
		this->setupAP();
		return;
	}

	const String &ssid = this->wifiPreferences.getString(WifiManager::SSID_KEY);
	const String &password = this->wifiPreferences.getString(WifiManager::PASSWORD_KEY);
	const IPAddress &ip = this->getIpAddressFromPreferences(this->wifiPreferences, WifiManager::IP_KEY);
	const IPAddress &subnet = this->getIpAddressFromPreferences(this->wifiPreferences, WifiManager::SUBNET_KEY);
	const IPAddress &gateway = this->getIpAddressFromPreferences(this->wifiPreferences, WifiManager::GATEWAY_KEY);

	const bool success = this->connectToWifi(ssid, password, ip, subnet, gateway);
	if (!success) {
		this->setupAP();
		return;
	}
}

bool WifiManager::hasSavedWifi(Preferences wifiPreferences) {
	return wifiPreferences.isKey(WifiManager::SSID_KEY) &&
		   wifiPreferences.isKey(WifiManager::PASSWORD_KEY);
}

bool WifiManager::connectToWifi(const String &ssid, const String &password, const IPAddress &ip, const IPAddress &subnet, const IPAddress &gateway) {
	WiFi.disconnect();
	WiFi.mode(WIFI_STA);

	// It's not a problem if this is not successful. Then we'll get a new ip address from the DHCP server.
	WiFi.config(ip, subnet, gateway);
	WiFi.begin(ssid, password);

	const auto connectionStatus = WiFi.waitForConnectResult();

	return connectionStatus == WL_CONNECTED;
}

IPAddress WifiManager::getIpAddressFromPreferences(Preferences preferences, const char *ipKey) {
	if (!preferences.isKey(ipKey)) {
		return INADDR_NONE;
	}

	const uint32_t ip = preferences.getUInt(ipKey);

	return IPAddress(ip);
}

void WifiManager::setupAP() {
	WiFi.mode(WIFI_AP);

	WiFi.softAP(WifiManager::AP_SSID);

	this->setupServer();
}

void WifiManager::setupServer() {
	WiFi.scanNetworks();

	AsyncWebServer server(80);

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
	});

	server.begin();
}