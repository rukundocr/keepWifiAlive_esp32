// Include the necessary libraries
#include <Arduino.h>
#include "WiFi.h"

#define WIFI_NETWORK "claudewifi"
#define WIFI_PASSWORD "12341234"
#define WIFI_TIMEOUT_MS 20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000 // Wait 30 seconds after a failed connection attempt

/**
 * Task: monitor the WiFi connection and keep it alive!
 * 
 * When a WiFi connection is established, this task will check it every 10 seconds 
 * to make sure it's still alive.
 * 
 * If not, a reconnect is attempted. If this fails to finish within the timeout,
 * the ESP32 will wait for it to recover and try again.
 */
void keepWiFiAlive(void * parameter){
    for(;;){
        if(WiFi.status() == WL_CONNECTED){
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            Serial.println("wifi still connected");
            continue;
        }

        Serial.println("[WIFI] Connecting");
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

        unsigned long startAttemptTime = millis();

        // Keep looping while we're not connected and haven't reached the timeout
        while (WiFi.status() != WL_CONNECTED && 
                millis() - startAttemptTime < WIFI_TIMEOUT_MS){}

        // When we couldn't make a WiFi connection (or the timeout expired)
      // sleep for a while and then retry.
        if(WiFi.status() != WL_CONNECTED){
            Serial.println("[WIFI] FAILED");
            vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
        continue;
        }

        Serial.println("[WIFI] Connected: " + WiFi.localIP());
    }
}

void setup(){
  Serial.begin(9600);
  xTaskCreatePinnedToCore(
  keepWiFiAlive,
  "keepWiFiAlive",  // Task name
  5000,             // Stack size (bytes)
  NULL,             // Parameter
  1,                // Task priority
  NULL,             // Task handle
  ARDUINO_RUNNING_CORE
);
  }

  void loop(){
    
    }
