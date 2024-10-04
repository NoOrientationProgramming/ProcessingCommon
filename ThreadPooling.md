
# THREAD_POOLING(3) Manual Page

## NAME
**ThreadPooling** – Manage a pool of threads to handle concurrent processing tasks in C++.

## SYNOPSIS
```cpp
#include "ThreadPooling.h"

ThreadPooling *create();

void workerCntSet(uint16_t cnt);
void driverCreateSet(FuncDriverPoolCreate pFctDriverCreate);
static void procAdd(Processing *pProc, int32_t idDriver = -1);
```

## DESCRIPTION
**ThreadPooling()** ist eine C++-Klasse, die zur Verwaltung eines Pools von Threads konzipiert wurde, um parallele Verarbeitungsaufgaben effizient zu steuern. Die Klasse bietet Mechanismen zur Erstellung, Verwaltung und Koordination von Worker-Threads, die mit Verarbeitungsobjekten interagieren und Aufträge abarbeiten.

### Features:
- **Thread Management**: Konfigurieren Sie die Anzahl der aktiven Worker-Threads mit `workerCntSet()`.
- **Dynamische Auftragsverarbeitung**: Fügen Sie Verarbeitungsobjekte zur Bearbeitung in den Pool ein mit `procAdd()`.
- **Erweiterbarkeit**: Ermöglicht die Anpassung des Erstellungsprozesses von Treibern durch die Funktion `driverCreateSet()`, um spezifische Anforderungen zu erfüllen.
- **Sichere Interaktion**: Verwendet Mutex-Schutzmechanismen, um den Zugriff auf gemeinsam genutzte Ressourcen zu synchronisieren.

### Structs:
- **PoolRequest**: Struktur, die Anforderungen zur Verarbeitung mit zugehörigen Verarbeitungsobjekten und gewünschten Treiber-IDs verwaltet.

## METHODS

### Pool Management
- **create()**  
  Allokiert eine neue Instanz der **ThreadPooling()**-Klasse.

- **workerCntSet(uint16_t cnt)**  
  Setzt die Anzahl der Worker-Threads im Pool.

- **driverCreateSet(FuncDriverPoolCreate pFctDriverCreate)**  
  Legt die Funktion fest, die für die Erstellung von Treibern verwendet wird.

- **procAdd(Processing *pProc, int32_t idDriver = -1)**  
  Fügt ein Verarbeitungsobjekt zur Warteschlange hinzu, das im Pool verarbeitet werden soll.

### Process Management
- **process()**  
  Führt die Logik zur Verarbeitung von Poolanforderungen und zur Verwaltung der Worker-Threads aus.

- **shutdown()**  
  Beendet alle aktiven Threads und gibt die Ressourcen frei.

### Internal Management
- **poolRequestsProcess()**  
  Bearbeitet die Anfragen, die im Pool eingegangen sind.

- **procsDrive()**  
  Steuert die laufenden Verarbeitungsobjekte im Pool.

- **idDriverNextGet()**  
  Gibt die ID des nächsten Treibers zurück, der im Pool verwendet werden kann.

- **numProcessingGet()**  
  Gibt die Anzahl der derzeit verarbeiteten Objekte im Pool zurück.

- **procInternalAdd(Processing *pProc)**  
  Fügt ein internes Verarbeitungsobjekt zur internen Liste hinzu.

## RETURN VALUES
Methoden, die den Status oder die Konfiguration ändern, geben typischerweise `Success` zurück, um den Erfolg der Operation anzuzeigen. Funktionen, die Informationen zurückgeben, bieten spezifische Werte, wie z.B. die Anzahl der aktuell bearbeiteten Prozesse.

## NOTES
- Diese Klasse verwendet einen Broker-Mechanismus, um die Kommunikation zwischen Threads zu steuern und Ressourcen effizient zu verwalten.
- Die Klasse ist nicht kopierbar oder zuweisbar, um unbeabsichtigtes Teilen von Ressourcen oder Duplikation zu vermeiden.

## SEE ALSO
- `mutex`, `thread`, `vector`, `list`

## AUTHORS
Written by Johannes Natter.

