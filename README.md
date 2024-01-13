# SV2 Projektdokumentation

## Gruppenmitglieder

- Jan Obernberger
- René Ott
- Dennis Mayer
- Frank Mayer

## Planung

### C4 Modell

![C4 Modell: Layer 1](./c4/01.png)

![C4 Modell: Layer 2 - Frontend](./c4/02_frontend.png)

![C4 Modell: Layer 2 - Backend](./c4/02_backend.png)

![C4 Modell: Layer 2 - Sensors](./c4/02_sensors.png)


## Datenbank

In unserem Projekt wurde eine robuste Datenbankimplementierung durchgeführt, um die Übertragung und Speicherung von Daten vom Sensor effizient zu verwalten. Die Haupttechnologien, die dabei verwendet wurden, sind [Golang](https://go.dev/) als Programmiersprache, [GORM](https://gorm.io/) als ORM-Framework (Object-Relational Mapping) und [SQLite](https://www.sqlite.org) als Datenbank.

### Datenübertragung und Speicherung

Die Daten vom Sensor werden in Echtzeit übertragen und sowohl im Arbeitsspeicher als auch in der SQLite-Datenbank gespeichert. Dabei werden die neuesten Daten aus der Datenbank dynamisch im Arbeitsspeicher geladen und angezeigt, sobald der Server gestartet wird. Dies ermöglicht eine schnelle und effektive Darstellung der aktuellen Datenlage.

### Implementierung

Die Wahl von GORM erwies sich als äußerst vorteilhaft, da es ein intuitives ORM-Framework ist. Die Einarbeitung gestaltete sich trotz einiger Anfangsschwierigkeiten, aufgrund eines inkompatiblen Alpine-Docker-Images, recht unkompliziert und die Flexibilität von GORM ermöglichte eine effiziente Datenbankinteraktion. Die dokumentierte API und die aktive Community trugen dazu bei, die Entwicklungszeit zu optimieren.

Insgesamt bietet die implementierte Datenbanklösung eine stabile Grundlage für die Speicherung und Verwaltung von Sensordaten, unterstützt durch die Zuverlässigkeit von Golang, die Leistungsfähigkeit von GORM und die Einfachheit von SQLite.
