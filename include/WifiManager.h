#pragma once
#include <Preferences.h>
#include <optional>

class WifiManager {
  public:
	WifiManager() {}

	void init();
	void connect();

  private:
	bool hasSavedWifi(Preferences);
	bool connectToWifi(const String &ssid, const String &password, const IPAddress &ip, const IPAddress &subnet, const IPAddress &gateway);

	void setupAP();
	void setupServer();

	IPAddress getIpAddressFromPreferences(Preferences, const char *ipKey);

	inline static const char *WIFI_PREFERENCES_NAME = "wifi";
	inline static const char *SSID_KEY = "ssid";
	inline static const char *PASSWORD_KEY = "pw";
	inline static const char *IP_KEY = "ip";
	inline static const char *SUBNET_KEY = "sub";
	inline static const char *GATEWAY_KEY = "gate";

	inline static const uint8_t MAX_CONNECT_TIME = 10; // s

	inline static const char *AP_SSID = "GoonpaLED";

	// inline static const char *html = R"html
	// 	<!DOCTYPE html>
	// 	<html>
	// 		<head></head>
	// 		<body>
	// 			HALOU
	// 		</body>
	// 	</html>
	// html";

	Preferences wifiPreferences;
};