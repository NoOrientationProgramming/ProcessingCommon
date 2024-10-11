
# HTTP_REQUESTING() Manual Page

## NAME

**HttpRequesting** – Perform HTTP requests and manage sessions in C++.

## SYNOPSIS

```cpp
#include "HttpRequesting.h"

HttpRequesting *create();
HttpRequesting *create(const std::string &url);

void urlSet(const std::string &url);
void typeSet(const std::string &type);
void userPwSet(const std::string &userPw);
void hdrAdd(const std::string &hdr);
void dataSet(const std::string &data);
void authMethodSet(const std::string &authMethod);
void versionTlsSet(const std::string &versionTls);
void versionHttpSet(const std::string &versionHttp);
void modeDebugSet(bool en);

uint16_t respCode() const;
std::string &respHdr();
std::string &respData();
```

## DESCRIPTION
**HttpRequesting()** is a C++ class designed to facilitate making HTTP requests and handling responses. It integrates with **libcurl** to send requests, receive responses, and manage HTTP sessions, with features for setting various HTTP parameters such as headers, authentication, and data payloads.

### Features:
- **URL Configuration**: Set target URLs with `urlSet()`.
- **Request Type**: Specify the HTTP method (e.g., GET, POST) using `typeSet()`.
- **Authentication**: Configure username/password and authentication method.
- **Headers**: Add custom HTTP headers with `hdrAdd()`.
- **TLS/SSL**: Set specific TLS/SSL versions for secure connections.
- **Debugging Mode**: Enable or disable detailed logging for debugging purposes.

### Structs:
- **HttpSession**: Manages connection details such as the URL, port, and shared data for session handling with **libcurl**. It includes mutexes for handling thread safety during concurrent requests.

## METHODS

### Request Configuration
- **create()**  
  Allocates a new instance of the **HttpRequesting()** class without initializing a URL.

- **create(const std::string &url)**  
  Allocates a new instance of the **HttpRequesting()** class and sets the initial URL.

- **urlSet(const std::string &url)**  
  Specifies the URL for the HTTP request.

- **typeSet(const std::string &type)**  
  Sets the HTTP method (e.g., "GET", "POST", "PUT").

- **userPwSet(const std::string &userPw)**  
  Configures HTTP basic authentication by providing a `username:password` string.

- **hdrAdd(const std::string &hdr)**  
  Adds a custom header to the HTTP request.

- **dataSet(const std::string &data)**  
  Sets the request payload data (useful for POST and PUT requests).

- **authMethodSet(const std::string &authMethod)**  
  Specifies the authentication method (e.g., "Basic", "Bearer").

- **versionTlsSet(const std::string &versionTls)**  
  Defines the TLS version for secure connections (e.g., "TLSv1.2").

- **versionHttpSet(const std::string &versionHttp)**  
  Sets the HTTP version (e.g., "HTTP/1.1", "HTTP/2").

- **modeDebugSet(bool en)**  
  Enables or disables debug mode for verbose logging of the HTTP request and response process.

### Response Handling
- **respCode() const**  
  Returns the HTTP response status code (e.g., 200 for OK).

- **respHdr()**  
  Returns the headers received in the HTTP response.

- **respData()**  
  Retrieves the data/body of the HTTP response.

## RETURN VALUES
Most configuration methods return `void`, while query methods like `respCode()`, `respHdr()`, and `respData()` return the corresponding response status, headers, or data.

## NOTES
- The class internally manages **libcurl** sessions and supports thread-safe operations with mutex-protected shared resources.
- It is not copyable or assignable to prevent resource management issues and ensure proper session handling.
- The class supports both synchronous and asynchronous processing of HTTP requests using **libcurl**’s multi-interface features.

## EXAMPLES
```cpp
  pReq = HttpRequesting::create("https://example.com/api")
  if (!pReq)
    return procErrLog(-1, "could not create process");

  pReq->typeSet("POST");
  pReq->hdrAdd("Content-Type: application/json");
  pReq->dataSet("{\"key\":\"value\"}");

  start(pReq);

  mState = StReqDoneWait;

  break;
case StReqDoneWait:

  success = pReq->success();
  if (success == Pending)
    break;

  if (success != Positive)
    return procErrLog(-1, "could not finish HTTP request");

  statusCode = httpReq->respCode();
  responseBody = httpReq->respData();

  repel(pReq);
  pReq = NULL;
```

## SEE ALSO
- `Processing()`

## AUTHORS
Written by Johannes Natter.

