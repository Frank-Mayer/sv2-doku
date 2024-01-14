# SV2 Projektdokumentation

## Gruppenmitglieder

- Jan Obernberger
- René Ott
- Dennis Mayer
- Frank Mayer

## Planung

### C4 Modell

#### Layer 1

![](./c4/01.png)

#### Layer 2

![](./c4/02_frontend.png)

![](./c4/02_backend.png)

![](./c4/02_sensors.png)

## Datenbank

In unserem Projekt wurde eine robuste Datenbankimplementierung durchgeführt, um die Übertragung und Speicherung von Daten vom Sensor effizient zu verwalten. Die Haupttechnologien, die dabei verwendet wurden, sind [Golang](https://go.dev/) als Programmiersprache, [GORM](https://gorm.io/) als ORM-Framework (Object-Relational Mapping) und [SQLite](https://www.sqlite.org) als Datenbank.

### Datenübertragung und Speicherung

Die Daten vom Sensor werden in Echtzeit übertragen und sowohl im Arbeitsspeicher als auch in der SQLite-Datenbank gespeichert. Dabei werden die neuesten Daten aus der Datenbank dynamisch im Arbeitsspeicher geladen und angezeigt, sobald der Server gestartet wird. Dies ermöglicht eine schnelle und effektive Darstellung der aktuellen Datenlage.

### Implementierung

Die Wahl von GORM erwies sich als äußerst vorteilhaft, da es ein intuitives ORM-Framework ist. Die Einarbeitung gestaltete sich trotz einiger Anfangsschwierigkeiten, aufgrund eines inkompatiblen Alpine-Docker-Images, recht unkompliziert und die Flexibilität von GORM ermöglichte eine effiziente Datenbankinteraktion. Die dokumentierte API und die aktive Community trugen dazu bei, die Entwicklungszeit zu optimieren.

Insgesamt bietet die implementierte Datenbanklösung eine stabile Grundlage für die Speicherung und Verwaltung von Sensordaten, unterstützt durch die Zuverlässigkeit von Golang, die Leistungsfähigkeit von GORM und die Einfachheit von SQLite.


# Arbeitsschritte zur Anfertigung des Gehäuses für unsere Wetterstation

### Schritte zur Herstellung

#### 1. Anreißen der Bohrung und der Aussparungen
   - **Zweck:** Markierung der Positionen für Bohrungen und Aussparungen.
   - **Werkzeug:** Höhenanreißer.
   - **Verfahren:** 
     - Mit dem Höhenanreißer Positionen für Bohrungen und Aussparungen auf dem Material präzise markieren.
     - Sorgfältige Markierung gewährleistet spätere Passgenauigkeit.

#### 2. Anfertigen der Aussparungen mit Pneumatikschleifer
   - **Zweck:** Erstellen der Aussparungen für Komponenten der Wetterstation.
   - **Werkzeug:** Pneumatikschleifer.
   - **Verfahren:** 
     - Aussparungen gemäß den Anreißmarkierungen schleifen.
     - Kontinuierliche Überprüfung der Maße zur Sicherstellung der Passgenauigkeit.

       ![](./bearbeitung/penumatik.png)

#### 3. Bohren an Standbohrmaschine
   - **Zweck:** Erstellen präziser Bohrungen für die LED.
   - **Werkzeug:** Standbohrmaschine, entsprechender Bohrer.
   - **Verfahren:** 
     - Material in die Standbohrmaschine einspannen.
     - Bohrungen an den markierten Stellen exakt durchführen.

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

