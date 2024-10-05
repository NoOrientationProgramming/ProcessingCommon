
# MAIL_SENDING() Manual Page

## NAME
**MailSending** – Sends emails, manages SMTP server connections, and handles email components in C++.

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
**MailSending()** is a C++ class used to send emails via SMTP. It manages the connection to an SMTP server, configures email components such as sender, recipient, subject, and body, and enables sending emails via the `process()` method.

### Features:
- **SMTP Server Configuration**: Set the SMTP server using `serverSet()`.
- **Email Components**: Configure the sender, recipient, subject, and body of the email using the corresponding setter methods.
- **Email Sending**: Send the email using the `process()` method, which processes all configured parameters.

### Structs:
- **MailSeState**: Enum that manages the different states of the email sending process, such as `MailSeStart` and `MailSeDoneWait`.

## METHODS

### Email Configuration
- **create()**  
  Allocates a new instance of the **MailSending()** class.

- **serverSet(const std::string &server)**  
  Sets the SMTP server through which emails are sent.

- **passwordSet(const std::string &password)**  
  Sets the password for authentication with the SMTP server.

- **recipientSet(const std::string &recipient)**  
  Sets the recipient’s email address.

- **senderSet(const std::string &sender)**  
  Sets the sender’s email address.

- **subjectSet(const std::string &subject)**  
  Sets the subject of the email.

- **bodySet(const std::string &body)**  
  Sets the body of the email.

### Email Sending
- **start()**  
  Processes all configured email parameters and sends the email to the specified recipient.

## RETURN VALUES
Methods that modify the state or configuration typically return a reference to the **MailSending()** object itself, enabling method chaining. The `process()` method returns the status of the sending process, usually a `Success` value, indicating whether the email was sent successfully.

## NOTES
- This class provides an abstraction for handling SMTP servers and sending emails in C++.
- The class is not copyable or assignable to prevent unintended resource sharing or duplication.

## SEE ALSO
- `Processing()`, `libcurl`

## AUTHORS
Written by Johannes Natter.

