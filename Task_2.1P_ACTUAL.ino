#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include "ThingSpeak.h"

#include "Secret.h" 
// Please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // Your network SSID (name)
char pass[] = SECRET_PASS;    // Your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // The WiFi radio's status
WiFiClient  client;

// ThingSpeak credentials
const unsigned long myChannelNumber = 2502599;
const char *myWriteAPIKey = "6WWTZ3R8V6VZ7ARQ";


BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // Wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Reconnected to WiFi");
  }

  Serial.println("Reading from BH1750...");
  uint16_t lux = lightMeter.readLightLevel(); // Read light level in lux
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");

  // Update ThingSpeak fields with the actual values
  ThingSpeak.setField(1, lux);

  // Write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Delay before the next read
  delay(20000); // Delay adjusted to 20 seconds to avoid hitting ThingSpeak's rate limit
}
