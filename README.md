# SV2 Projektdokumentation

## Gruppenmitglieder

- Jan Obernberger
- René Ott
- Dennis Mayer
- Frank Mayer

## Planung

### C4 Modell

#### Layer 1

<img src="./c4/01.png" width="40%">

#### Layer 2

<img src="./c4/02_frontend.png" width="40%">

<img src="./c4/02_backend.png" width="40%">

<img src="./c4/02_sensors.png" width="40%">

## Hardware

Zum Hosten der Website, sowie die Verarbeitung und Speicherung der Meßdaten entschlossen wir uns, einen Raspberry Pi zu verwenden, da dieser über außreichend Leistung verfügt, um eine komplexe Single-Page-Webanwendung mit Datenbank zu hosten.

Zur Steuerung von Sensoren und Aktoren entschieden wir uns für einen ESP32, da dieser Standard für diesen Anwendungsbereich ist. Für seinen Betrieb greifen wir Standard Li-Ionen Zellen zürück.

Beide Systeme verfügen außerdem über ein großes Angebot an Ressourcen zur Programmierung.

Für den ESP suchten wir nach passenden Sensoren. Ursprünglich wollten wir verschiedene Sensoren verbauen, was wir jedoch ändern mussten.

<img src="./hardware/image.png" width="40%">

Eine Auflage war es, repräsentativ einen Aktor am ESP über die Netzwerkverbindung steuern zu können. Hierfür wählten wir der Einfachheit halber eine LED.

#### Probleme

Leider war der Sensor, den wir als Alternative zur Bestellung bekamen defekt. Aufgrund unserer fehlenden Erfahrung, konnte wir dies aber Anfangs nicht mit Sicherheit sagen. Jedoch konnten wir mit I²C Scanner-Skripten für den ESP keinen Sensor erkennen.

<img src="./hardware/image-3.png" width="40%">

Wir teilten dies dem Betreuer mit und machten das Gegenangebot, einen DHT22 zu benutzen, die ein Team-Mitglied noch daheim hatte.

<img src="./hardware/image-2.png" width="40%">

### Außenstation

Letzlich planten wir ein minimalistisches Konzept für die Außenstation.

<img src="./hardware/image-1.png" width="40%">

Wir bauten einen Protoyp, den wir gegen eine Mosquitto Docker Container testenen, bevor wir mit der einrichtung des Pis begannen.

<img src="./hardware/image-4.png" width="40%">

Nachdem alles funktioniert hat, löteten wir zie Komponenten zusammen, sodass wir sie einfach in unser selbstgebautes Gehäuse einsetzen können.

<img src="./hardware/image-5.png" width="40%">

#### Code

Platformio Konfiguration

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
build_flags = 
	-I/home/jano/dev/CLionProjects/weather-station/src
lib_deps =
	adafruit/DHT sensor library@^1.4.6
	knolleary/PubSubClient@^2.8
	bblanchon/ArduinoJson@^6.21.5
```

Konfiguration

```c++
const char *ssid = "Toilet Cam";
const char *password = "!234Amsterdam";
const char *mqtt_server = "192.168.2.114";
const int mqtt_port = 1883;
const char *mqtt_topic_publish = "sensordata";
const char *mqtt_topic_subscribe = "led";
const int led_pin = 16;
const bool debug_mode = true;
const bool sleep_mode = false;
const unsigned long read_publish_delay = 30000;
const uint64_t deep_sleep_delay = 60e6;
const uint loop_delay = 250;
```

Der meiste Teil ist aus den Dokumentationen der Libraries adaptiert und in Funktionen verpackt.

Da wir zum Spaß auch Morsecode in Echtzeit als on/off commands an den ESP übertragen wollen, entschieden wir uns, für das Lesen und Senden der Sensordaten deinen Thread zu erstellen, wozu wir die xTaskCreate-Funktion des FreeRTOS nutzen.

```c++
void setup() {
    Serial.begin(115200);

    initializeDHT();
    initializeActuator();
    initializeWiFi();
    initializeMQTT();

    xTaskCreate(mqttPublishTask,"MQTTPublishTask", 10000, NULL, 1, NULL);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    delay(250);
}
```

Für die Kommunikation muss der ESP neben WiFi ebenso eine Verbindung zu einem MQTT-Broker aufbauen.

```c++
void initializeMQTT() {
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqtt_broker_callback);
    client.subscribe(mqtt_topic_subscribe);
}
```

Zum Epfangen der Daten wird eine Callback-Funktion beim broker registriert.

```c++
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
```

Zum Senden der Daten wird ein nebenläufiger Prozess verwendet.

```c++
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
```

Ist sleep_mode = true, so wird der ESP nach einmaligem Senden in deep sleep versetzt und wenn debug_mode true ist, werden die Aktionen des ESP über die serielle Schnittstelle geprinted.

### Server

Da wir sowieso mit Docker als Umgebung für die Anwendungen (MQTT Broker, Webserver, etc.) planten, mussten wir beim Pi nur ein aktuelles Raspian Lite Image aufspielen und den WLAN-Zugang konfigurieren, sodass wir ihn per SSH bedienen konnten.

Wir klonten das Repository, erstellten bash-Skripte zum starten und beenden unserer Applikation.

```bash
#!/bin/bash
cd /home/sv2/weather-station/sv2/
docker-compose up --build --detach
```

Außerden erstellten wir systemd Services, sodass die Skripte beim Starten und Beenden des Pis ausgeführt werden.

```ini
[Unit]
Description=Start Weather Station Docker Compose
After=network.target

[Service]
Type=simple
User=sv2
ExecStart=/home/sv2/start_weather_station.sh

[Install]
WantedBy=multi-user.target
```

## Datenbank

In unserem Projekt wurde eine robuste Datenbankimplementierung durchgeführt, um die Übertragung und Speicherung von Daten vom Sensor effizient zu verwalten. Die Haupttechnologien, die dabei verwendet wurden, sind [Golang](https://go.dev/) als Programmiersprache, [GORM](https://gorm.io/) als ORM-Framework (Object-Relational Mapping) und [SQLite](https://www.sqlite.org) als Datenbank.

### Datenübertragung und Speicherung

Die Daten vom Sensor werden in Echtzeit übertragen und sowohl im Arbeitsspeicher als auch in der SQLite-Datenbank gespeichert. Dabei werden die neuesten Daten aus der Datenbank dynamisch im Arbeitsspeicher geladen und angezeigt, sobald der Server gestartet wird. Dies ermöglicht eine schnelle und effektive Darstellung der aktuellen Datenlage.

### Implementierung

Die Wahl von GORM erwies sich als äußerst vorteilhaft, da es ein intuitives ORM-Framework ist. Die Einarbeitung gestaltete sich trotz einiger Anfangsschwierigkeiten, aufgrund eines inkompatiblen Alpine-Docker-Images, recht unkompliziert und die Flexibilität von GORM ermöglichte eine effiziente Datenbankinteraktion. Die dokumentierte API und die aktive Community trugen dazu bei, die Entwicklungszeit zu optimieren.

Insgesamt bietet die implementierte Datenbanklösung eine stabile Grundlage für die Speicherung und Verwaltung von Sensordaten, unterstützt durch die Zuverlässigkeit von Golang, die Leistungsfähigkeit von GORM und die Einfachheit von SQLite.


## Anfertigung des Gehäuses für unsere Wetterstation

### Schritte zur Herstellung

#### 1. Anreißen der Bohrung und der Aussparungen
   - **Zweck:** Markierung der Positionen für Bohrungen und Aussparungen.
   - **Werkzeug:** Höhenanreißer.
   - **Verfahren:** 
     - Mit dem Höhenanreißer Positionen für Bohrungen und Aussparungen auf dem Material präzise markieren.
     - Sorgfältige Markierung gewährleistet spätere Passgenauigkeit.

<img src="./bearbeitung/pneumatik.jpg" width="40%">

#### 2. Anfertigen der Aussparungen mit Pneumatikschleifer
   - **Zweck:** Erstellen der Aussparungen für Komponenten der Wetterstation.
   - **Werkzeug:** Pneumatikschleifer.
   - **Verfahren:** 
     - Aussparungen gemäß den Anreißmarkierungen schleifen.
     - Kontinuierliche Überprüfung der Maße zur Sicherstellung der Passgenauigkeit.
       
<img src="./bearbeitung/ausspar.jpg" width="40%">
   
#### 3. Bohren an Standbohrmaschine
   - **Zweck:** Erstellen präziser Bohrungen für die LED.
   - **Werkzeug:** Standbohrmaschine, entsprechender Bohrer.
   - **Verfahren:** 
     - Material in die Standbohrmaschine einspannen.
     - Bohrungen an den markierten Stellen exakt durchführen.

<img src="./bearbeitung/bohrer.jpg" width="40%">

#### 4. Entgraten der Kanten
   - **Zweck:** Glättung und Sicherheitsverbesserung der bearbeiteten Kanten.
   - **Werkzeug:** Entgratwerkzeug, Feile.
   - **Verfahren:** 
     - Scharfe und raue Kanten sorgfältig entgraten.
     - Gleichmäßige Bearbeitung zur Vermeidung von Materialschwächungen.

#### 5. Befestigung für den ESP durch Einkleben von Abstandhaltern
   - **Zweck:** Fester Halt für den ESP und Sicherstellung das Die CPU nicht auf Kunstoff aufliegt.
   - **Material:** Abstandhalter, Klebstoff.
   - **Verfahren:**
     - Abstandhalter an den vorgesehenen Positionen einkleben.
     - Sicherstellen, dass der ESP nicht direkt auf dem Kunststoff aufliegt, um Überhitzungsrisiken zu minimieren.
     - Ausreichende Aushärtezeit des Klebstoffs gewährleisten.
    
   <img src="./bearbeitung/befestigung.jpg" width="40%">

