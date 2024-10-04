
# PHY_ANIMATING(3) Manual Page

## NAME
**PhyAnimating** – Implementiert physikalische Animationen und Benutzeroberflächen in C++ unter Verwendung von Qt und OpenCV.

## SYNOPSIS
```cpp
#include "PhyAnimating.h"

PhyAnimating *create(const char *name);

Success process();
Success shutdown();
Success animate();
Success animShutdown();
```

## DESCRIPTION
**PhyAnimating()** ist eine C++-Klasse, die für die Erstellung von physikalischen Animationen und die Verwaltung von Benutzeroberflächen entwickelt wurde. Sie verwendet die Qt-Bibliothek für die grafische Benutzeroberfläche und ermöglicht die Interaktion mit animierten Elementen in einer Anwendung. Die Klasse bietet eine einfache Möglichkeit, Steuerelemente hinzuzufügen, zu aktualisieren und Animationen zu verwalten.

### Features:
- **Benutzeroberflächenkomponenten**: Ermöglicht das Hinzufügen von Labels, Schaltflächen, Kontrollkästchen, Schiebereglern, Eingabefeldern und Fortschrittsbalken zur Benutzeroberfläche.
- **Animation Management**: Bietet Methoden zum Starten und Stoppen von Animationen über die `animate()` und `animShutdown()`-Methoden.
- **Signal- und Slot-Mechanismus**: Unterstützt die Interaktion zwischen Benutzeroberflächenkomponenten und Backend-Logik durch Qt's Signal- und Slot-System.
- **Chart-Darstellung**: Ermöglicht das Hinzufügen von Diagrammen zur Visualisierung von Daten.

### Structs:
- **LabelInfo**: Strukturiert Informationen zu Labels, einschließlich Zeiger auf QLabel, Präfix, Einheit und maximalem Wert.

## METHODS

### Prozessverwaltung
- **create(const char *name)**  
  Allokiert eine neue Instanz der **PhyAnimating()**-Klasse mit dem angegebenen Namen für die Benutzeroberfläche.

- **process()**  
  Führt die Logik zur Verarbeitung von Animationen und Benutzerinteraktionen aus.

- **shutdown()**  
  Beendet den Animationsprozess und gibt alle Ressourcen frei.

- **animate()**  
  Virtuelle Methode, die von abgeleiteten Klassen implementiert werden muss, um die Animationslogik zu definieren.

- **animShutdown()**  
  Virtuelle Methode, die von abgeleiteten Klassen implementiert werden muss, um die Logik zum Herunterfahren von Animationen zu definieren.

### Benutzeroberflächenmanagement
- **uiLineAdd(const std::string &strLabel = "")**  
  Fügt eine neue Zeile zur Benutzeroberfläche hinzu.

- **uiLabelAdd(const std::string &strPrefix = "", bool alignRight = true)**  
  Fügt ein neues Label zur Benutzeroberfläche hinzu.

- **uiButtonAdd(const std::string &strText)**  
  Fügt eine Schaltfläche zur Benutzeroberfläche hinzu.

- **uiSwitchAdd(const std::string &strLabel)**  
  Fügt ein Kontrollkästchen zur Benutzeroberfläche hinzu.

- **uiSliderAdd(int valMax, int valStart, const std::string &strPrefix, const std::string &strUnit = "", bool isTwoSided = false)**  
  Fügt einen Schieberegler zur Benutzeroberfläche hinzu.

- **uiLineEditAdd(const std::string &strLabel = "")**  
  Fügt ein Eingabefeld zur Benutzeroberfläche hinzu.

- **uiProgressAdd(const std::string &strLabel)**  
  Fügt einen Fortschrittsbalken zur Benutzeroberfläche hinzu.

- **uiChartAdd()**  
  Fügt ein Diagramm zur Benutzeroberfläche hinzu.

## RETURN VALUES
Methoden, die den Status oder die Konfiguration ändern, geben typischerweise einen `Success`-Wert zurück, der den Status des Verarbeitungsprozesses angibt. 

## NOTES
- Diese Klasse erfordert die Installation von Qt6 und optional OpenCV, um die Benutzeroberfläche und die Animationen zu unterstützen.
- Die Klasse ist nicht kopierbar oder zuweisbar, um unbeabsichtigtes Teilen von Ressourcen oder Duplikation zu vermeiden.

## SEE ALSO
- `QApplication`, `QPushButton`, `QSlider`, `QProgressBar`, `QChart`

## AUTHORS
Written by Johannes Natter.

