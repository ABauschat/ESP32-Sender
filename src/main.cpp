#include <Arduino.h>
#include <WiFi.h>
// Minimal ESP32 Sketch to Send MAC Address

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; } // Wait for Serial to initialize
  Serial.println("ESP32 Ready");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("GET_MAC")) {
      String mac = WiFi.macAddress();
      Serial.println(mac);
    } else {
      Serial.println("UNKNOWN_COMMAND");
    }
  }
}
