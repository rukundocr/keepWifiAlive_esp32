
# WiFi Connection Monitoring with ESP32

This code is written for an ESP32 microcontroller to monitor and maintain a WiFi connection. It continuously checks the connection status and attempts to reconnect if the connection is lost.

## Libraries and Definitions

First, the necessary libraries are included, and several constants are defined:

```cpp
#include <Arduino.h>
#include "WiFi.h"

#define WIFI_NETWORK "claudewifi"
#define WIFI_PASSWORD "12341234"
#define WIFI_TIMEOUT_MS 20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000 // Wait 30 seconds after a failed connection attempt
```

## Task Function: `keepWiFiAlive`

The `keepWiFiAlive` function is designed to run as a separate task. It monitors the WiFi connection and attempts to reconnect if the connection is lost. The task runs indefinitely in a loop.

### Function Details

```cpp
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
```

- The function starts with an infinite loop `for(;;)` to keep running indefinitely.
- It checks if the WiFi is connected using `WiFi.status() == WL_CONNECTED`.
  - If connected, it waits for 10 seconds (`vTaskDelay(10000 / portTICK_PERIOD_MS)`) and prints "wifi still connected" to the serial monitor.
- If the WiFi is not connected:
  - It prints "[WIFI] Connecting" and attempts to connect using `WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD)`.
  - It tracks the start time of the connection attempt.
  - It continues to check the connection status until either it connects or the timeout (`WIFI_TIMEOUT_MS`) is reached.
  - If the connection attempt fails, it prints "[WIFI] FAILED" and waits for 30 seconds (`vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS)`) before retrying.
  - If the connection is successful, it prints the local IP address to the serial monitor.

## Setup Function

The `setup` function initializes the serial communication and creates the WiFi monitoring task.

```cpp
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
```

- `Serial.begin(9600)` initializes serial communication at a baud rate of 9600.
- `xTaskCreatePinnedToCore` creates the `keepWiFiAlive` task and pins it to a specific core of the ESP32.

## Loop Function

The `loop` function is empty as the main functionality is handled by the `keepWiFiAlive` task.

```cpp
void loop(){
    // Empty loop
}
```

This code ensures that the ESP32 maintains a WiFi connection, attempting to reconnect whenever the connection is lost.
