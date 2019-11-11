# ftcommunity-neopixel - Neopixel für alle!

Nach  der  weihnachtlichen  Lektüre  des  Artikels  über  programmierbare fischertechnik-LEDs  in  der  ft:pedia  4/2017  musste  der  fischertechniker sofort  das  coolste  und  am  besten  beleuchtete  Modell  aller Zeiten  beginnen.  Aber:  Muss  man  jetzt  auch  noch  das Programmieren von Arduinos erlernen? Wie bekommt man  das  Modell  nun  vom  TX(T)  aus  angesteuert? Der NeopixelController löst beide Probleme: Die Firmware   beherrscht   im   stand-alone-Betrieb u.   a.    Lauflicht-,    NightRider-    und    Rainbow- Effekte.  Im  I²C-Modus  können  die  LEDs  vom TX(T) mit Robo Pro gesteuert werden. Es werden nur  wenige  elektronische  Bauteile  benötigt,  so dass der Nachbau für jeden geeignet ist.

Der volle Artikel kann in der [ft:pedia 1/2018](https://ftcommunity.de/ftpedia_ausgaben/ftpedia-2018-1.pdf) ab Seite 53 nachgelesen werden.

**Achtung! Im Artikel sind zwei Druckfehler:**
* S.57 Abbildung 6 hat einen 10poligen Stecker für den I2C-Bus. Korrektur: Es ist ein 6-poliger Stecker.
* S.60 Abbildung 14 zeigt die Einstellung für I2C-Modus mit der dezimalen Adresse 11. Der RoboPro-Treiber arbeitet jedoch mit der hexadezimalen Adresse 0x11. Korrektur: Die richtige Schalterstellung lautet ON-ON-ON-ON-ON-OFF-ON-ON-ON-OFF. (Alle auf ON, nur 6 und 10 auf OFF.)
