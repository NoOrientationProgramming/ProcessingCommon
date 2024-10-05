
# MAIL_SENDING() Manual Page

## NAME
**MailSending** – Sendet E-Mails, verwaltet SMTP-Serververbindungen und verarbeitet E-Mail-Komponenten in C++.

## SYNOPSIS
```cpp
#include "MailSending.h"

MailSending *create();

MailSending &serverSet(const std::string &server);
MailSending &passwordSet(const std::string &password);
MailSending &recipientSet(const std::string &recipient);
MailSending &senderSet(const std::string &sender);
MailSending &subjectSet(const std::string &subject);
MailSending &bodySet(const std::string &body);

Success process();
```

## DESCRIPTION
**MailSending()** ist eine C++-Klasse, die verwendet wird, um E-Mails über SMTP zu versenden. Sie verwaltet die Verbindung zu einem SMTP-Server, konfiguriert E-Mail-Komponenten wie Absender, Empfänger, Betreff und Inhalt und ermöglicht das Versenden von E-Mails über die `process()`-Methode.

### Features:
- **SMTP-Server-Konfiguration**: Stellen Sie den SMTP-Server mit `serverSet()` ein.
- **E-Mail-Komponenten**: Konfigurieren Sie Absender, Empfänger, Betreff und Inhalt der E-Mail mit den entsprechenden Setter-Methoden.
- **E-Mail-Versand**: Versenden Sie die E-Mail mit der `process()`-Methode, die alle konfigurierten Parameter verarbeitet.

### Structs:
- **MailSeState**: Enum, das die verschiedenen Zustände des E-Mail-Versands verwaltet, z. B. `MailSeStart` und `MailSeDoneWait`.
  
## METHODS

### E-Mail Konfiguration
- **create()**  
  Allokiert eine neue Instanz der **MailSending()**-Klasse.

- **serverSet(const std::string &server)**  
  Setzt den SMTP-Server, über den die E-Mails gesendet werden.

- **passwordSet(const std::string &password)**  
  Setzt das Passwort für die Authentifizierung beim SMTP-Server.

- **recipientSet(const std::string &recipient)**  
  Setzt die E-Mail-Adresse des Empfängers.

- **senderSet(const std::string &sender)**  
  Setzt die E-Mail-Adresse des Absenders.

- **subjectSet(const std::string &subject)**  
  Setzt den Betreff der E-Mail.

- **bodySet(const std::string &body)**  
  Setzt den Inhalt der E-Mail.

### E-Mail Versand
- **process()**  
  Verarbeitet alle konfigurierten E-Mail-Parameter und sendet die E-Mail an den angegebenen Empfänger.

## RETURN VALUES
Methoden, die den Zustand oder die Konfiguration ändern, geben typischerweise eine Referenz auf das **MailSending()**-Objekt selbst zurück, um die Methodenverkettung zu ermöglichen. Die `process()`-Methode gibt den Status des Versandprozesses zurück, typischerweise als `Success`-Wert, der den Erfolg oder das Scheitern des E-Mail-Versands angibt.

## NOTES
- Diese Klasse bietet eine Abstraktion für den Umgang mit SMTP-Servern und das Senden von E-Mails in C++. 
- Die Klasse ist nicht kopierbar oder zuweisbar, um unbeabsichtigtes Teilen von Ressourcen oder Duplikation zu vermeiden.

## SEE ALSO
- `Processing()`, `libcurl`

## AUTHORS
Written by Johannes Natter.

