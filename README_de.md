# HR/EKG-Audio-Plugin.

Dieses Audio-Plugin basiert auf der Vorlage [Pamplejuce](https://github.com/sudara/pamplejuce/actions).
Pamplejuce ist eine Vorlage zum Erstellen und Erstellen von JUCE-Plugins mit CMake & CLion.

Die ursprüngliche Idee war, ein Audio-Plugin zu bauen, mit dem es möglich wäre, Live-EKG-Daten (Elektrokardiographie) in einer DAW aufzuzeichnen. Dafür habe ich mich für Polar Bluetooth-Sensoren entschieden, nämlich H9, H10, OH1 und Verity Sense, weil diese im Vergleich zu Alternativen sehr benutzerfreundlich sind. Sie haben auch ein mobiles Open-Source-SDK, das als Referenzquelle verwendet werden könnte.

Für Bluetooth habe ich mich für die Arbeit mit der Bibliothek [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBLE) entschieden, die auch CMake unterstützt und daher einfach in das Projekt zu integrieren war. IMHO hat SimpleBLE auch eine leicht zu erlernende und zu verstehende API.

## Aktuelles Zustand

Derzeit funktioniert das Audio-Plugin nur mit HR als Quelle für die Automatisierung (implementiert als JUCE::AudioProcessorParameter). EKG funktioniert noch nicht. Grund dafür war eine zuvor unvorhergesehene Schwierigkeit beim Empfang von EKG-Daten. Das CoreBluetooth-Framework von MacOs unterstützt nämlich keine manuelle Änderung der MTU-Größe für eine Bluetooth-Verbindung. Obwohl Linux und Windows dies unterstützen, unterstützt zum Zeitpunkt des Schreibens auch die SimpleBLE-Bibliothek diese Funktion [noch] nicht (https://github.com/OpenBluetoothToolbox/SimpleBLE/issues/83). Die Polar-Sensoren erfordern dies (auf 256 oder einen höheren Wert einzustellen), da sie Live-EKG-Daten nur mit dieser privaten/spezialisierten API (genannt PMD) streamen können.

[Demo-Video](https://www.youtube.com/watch?v=l_BRtQtrjJE)

## Source code

Alle Quelldateien befinden sich im Quellordner.

## Building

1. `git clone https://github.com/kasparsj/ecg-audio-plugin.git`
2. `cd ecg-audio-plugin`
3. `git submodule update --init`
4. `mkdir build && cd build`
5. `cmake ../`
