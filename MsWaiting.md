
# MS_WAITING() Manual Page

## NAME
**MsWaiting** – Implementiert Wartezeiten in Millisekunden und verwaltet den Verarbeitungsstatus in C++.

## SYNOPSIS
```cpp
#include "MsWaiting.h"

MsWaiting *create(uint32_t duration);

Success process();
```

## DESCRIPTION
**MsWaiting()** ist eine C++-Klasse, die verwendet wird, um Wartezeiten in Millisekunden zu implementieren. Sie ermöglicht es, einen bestimmten Zeitraum zu warten und die Verarbeitung während dieser Zeit zu steuern. Diese Klasse ist nützlich in Anwendungen, die synchronisierte Verzögerungen oder Pausen in der Ausführung benötigen.

### Features:
- **Zeitsteuerung**: Setzen Sie eine Dauer in Millisekunden beim Erstellen einer Instanz mit `create()`.
- **Verarbeitungsmanagement**: Die `process()`-Methode verarbeitet den Status des Wartens und ermöglicht es, die Anwendung während der Wartezeit in einem kontrollierten Zustand zu halten.

### Structs:
- Diese Klasse enthält keine speziellen Strukturen, jedoch verwaltet sie interne Variablen zur Zeitsteuerung.

## METHODS

### Wartezeit Management
- **create(uint32_t duration)**  
  Allokiert eine neue Instanz der **MsWaiting()**-Klasse mit der angegebenen Wartezeit in Millisekunden.

- **process()**  
  Führt die Warteprozesslogik aus und ermöglicht das Management des Zeitablaufs.

## RETURN VALUES
Methoden, die den Status oder die Konfiguration ändern, geben typischerweise einen `Success`-Wert zurück, der den Status des Verarbeitungsprozesses angibt. 

## NOTES
- Diese Klasse bietet eine einfache Möglichkeit, Wartezeiten in Millisekunden zu implementieren, und kann in Multithreaded- oder asynchronen Umgebungen nützlich sein.
- Die Klasse ist nicht kopierbar oder zuweisbar, um unbeabsichtigtes Teilen von Ressourcen oder Duplikation zu vermeiden.

## SEE ALSO
- `Processing()`

## AUTHORS
Written by Johannes Natter.

