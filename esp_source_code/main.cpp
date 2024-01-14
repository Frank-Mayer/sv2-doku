/**
 * ESP32 MQTT Sensor Node with LED Control and Optional Deep Sleep
 *
 * This program enables an ESP32 to function as an IoT sensor node. It interfaces with a DHT22 sensor for temperature and
 * humidity readings, controls an LED via MQTT commands, and offers an optional deep sleep mode for energy efficiency.
 *
 * Features:
 * - Reads temperature and humidity data from DHT22 and publishes to MQTT.
 * - Controls an LED based on MQTT messages, supporting custom commands like Morse codes.
 * - Connects to Wi-Fi and MQTT broker for data transmission and command reception.
 * - Optional deep sleep mode to conserve power, ideal for battery-operated setups.
 *
 * Configuration:
 * - Set Wi-Fi and MQTT broker details.
 * - Customize deep sleep duration and sensor reading intervals as needed.
 * - LED actuator can be programmed to respond to various MQTT commands.
 *
 * Note:
 * - Deep sleep mode reduces responsiveness to real-time MQTT messages.
 * - Ensure secure handling of credentials and MQTT communication.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "ArduinoJson.h"
#include "DHT.h"
#include "DHT_U.h"
#include "PubSubClient.h"

const char *ssid = "Your SSID";
const char *password = "Your Password";
const char *mqtt_server = "Your MQTT Broker IP";
const int mqtt_port = 1883;
const char *mqtt_topic_publish = "sensordata";
const char *mqtt_topic_subscribe = "led";
const int led_pin = 16;
const bool debug_mode = true;
const bool sleep_mode = false;
const unsigned long read_publish_delay = 30000;
const uint64_t deep_sleep_delay = 60e6;
const uint loop_delay = 250;

WiFiClient espClient;
PubSubClient client(espClient);
DHT_Unified dht(4, DHT22);

void initializeWiFi();

void initializeMQTT();

void initializeDHT();

void initializeActuator();

void readDHT(float &temperature, float &humidity);

void setActuator();

void unsetActuator();

void reconnect();

void mqtt_broker_callback(char *topic, byte *payload, unsigned int length);

String generateJson(const char *name, float value, const char *unit);

void printSensorDetails(sensor_t sensor, const char *sensorType);

[[noreturn]] void mqttPublishTask(void *pvParameters);

void setup() {
    Serial.begin(115200);

    initializeDHT();
    initializeActuator();
    initializeWiFi();
    initializeMQTT();

    xTaskCreate(mqttPublishTask, "MQTTPublishTask", 10000, NULL, 1, NULL);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    delay(loop_delay);
}

void initializeWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        if (debug_mode) {
            Serial.println("Connecting to WiFi...");
        }
    }
    if (debug_mode) {
        Serial.println("Connected to WiFi");
    }
}

void initializeMQTT() {
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqtt_broker_callback);
    client.subscribe(mqtt_topic_subscribe);
}

void initializeDHT() {
    dht.begin();
    if (debug_mode) {
        sensor_t tempSensor, humiditySensor;
        dht.temperature().getSensor(&tempSensor);
        dht.humidity().getSensor(&humiditySensor);
        printSensorDetails(tempSensor, "Temperature Sensor");
        printSensorDetails(humiditySensor, "Humidity Sensor");
    }
}

void readDHT(float &temperature, float &humidity) {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature)) {
        temperature = event.temperature;
        if (debug_mode) {
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.println("°C");
        }
    }
    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity)) {
        humidity = event.relative_humidity;
        if (debug_mode) {
            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println("%");
        }
    }
}

void initializeActuator() {
    pinMode(led_pin, OUTPUT);
}

void setActuator() {
    digitalWrite(led_pin, HIGH);
}

void unsetActuator() {
    digitalWrite(led_pin, LOW);
}

void reconnect() {
    while (!client.connected()) {
        if (debug_mode) {
            Serial.println("Connecting to MQTT...");
        }
        if (client.connect("ESP32Client")) {
            client.subscribe(mqtt_topic_subscribe);
            if (debug_mode) {
                Serial.println("Connected to MQTT");
            }
        } else {
            delay(5000);
        }
    }
}

void mqtt_broker_callback(char *topic, byte *payload, unsigned int length) {
    char message[length + 1];
    strncpy(message, (char *) payload, length);
    message[length] = '\0';

    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    Serial.println(message);

    StaticJsonDocument<100> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    const char *command = doc["command"];

    if (strcmp(command, "on") == 0) {
        setActuator();
    } else if (strcmp(command, "off") == 0) {
        unsetActuator();
    }
    client.loop();
}

String generateJson(const char *name, float value, const char *unit) {
    StaticJsonDocument<100> jsonDoc;
    jsonDoc["name"] = name;
    jsonDoc["value"] = value;
    jsonDoc["unit"] = unit;

    char jsonBuffer[100];
    serializeJson(jsonDoc, jsonBuffer, sizeof(jsonBuffer));
    return {jsonBuffer};
}

void printSensorDetails(sensor_t sensor, const char *sensorType) {
    Serial.println(F("------------------------------------"));
    Serial.println(sensorType);
    Serial.print(F("Sensor Type: "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:  "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:   "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:   "));
    Serial.print(sensor.max_value);
    Serial.println(strcmp(sensorType, "Temperature Sensor") == 0 ? F("°C") : F("%"));
    Serial.print(F("Min Value:   "));
    Serial.print(sensor.min_value);
    Serial.println(strcmp(sensorType, "Temperature Sensor") == 0 ? F("°C") : F("%"));
    Serial.print(F("Resolution:  "));
    Serial.print(sensor.resolution);
    Serial.println(strcmp(sensorType, "Temperature Sensor") == 0 ? F("°C") : F("%"));
    Serial.println(F("------------------------------------"));
}

[[noreturn]] void mqttPublishTask(void *pvParameters) {
    for (;;) {
        float temperature, humidity;
        readDHT(temperature, humidity);
        client.loop();
        String jsonTemperature = generateJson("Temperature", temperature, "C");
        client.publish(mqtt_topic_publish, jsonTemperature.c_str());
        String jsonHumidity = generateJson("Humidity", humidity, "%RH");
        client.publish(mqtt_topic_publish, jsonHumidity.c_str());
        if (sleep_mode) {
            Serial.println("Entering deep sleep mode");
            esp_deep_sleep(deep_sleep_delay);
        } else {
            vTaskDelay(read_publish_delay / portTICK_PERIOD_MS);
        }
    }
}
