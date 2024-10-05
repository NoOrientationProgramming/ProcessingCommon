
# LIB_DSPC Manual Page

## NAME
**LibDspc** – A comprehensive utility library for debugging, JSON processing, cryptography, networking, and string manipulation.

## SYNOPSIS
```cpp
#include "LibDspc.h"

// General Utilities
std::string appVersion();
void hexDump(const void *pData, size_t len, const char *pName = NULL, size_t colWidth = 0x10);
std::string toHexStr(const std::string &strIn);

// JSON Utilities (requires CONFIG_LIB_DSPC_HAVE_JSONCPP)
bool jKeyFind(const Json::Value &val, const std::string &nameKey);

// Cryptography Utilities (requires CONFIG_LIB_DSPC_HAVE_CRYPTOPP)
std::string md5(const std::string &msg, const std::string &prefix = "");
std::string sha256(const std::string &msg, const std::string &prefix = "");
bool isValidSha256(const std::string &digest);

// Curl Utilities (requires CONFIG_LIB_DSPC_HAVE_CURL)
void curlGlobalInit();
void curlGlobalDeInit();
```

## DESCRIPTION
The **LibDspc** library provides a variety of tools for developers working with debugging, JSON processing, cryptography, and networking. It includes a range of utility functions, from string manipulation to hashing, making it a versatile toolkit for low-level operations in embedded systems or networked applications.

This library has conditional features based on build-time configuration options, including support for **JsonCpp** (for JSON handling), **CryptoPP** (for cryptography), and **Curl** (for HTTP communications).

## UTILITIES

### General Utilities

- **std::string appVersion()**  
  Retrieves the current application version as a string. Typically used for debugging and logging purposes.

- **void hexDump(const void \*pData, size_t len, const char \*pName = NULL, size_t colWidth = 0x10)**  
  Performs a hex dump of the given data, printing the content in a readable hexadecimal format. Useful for debugging binary data.
  - **pData**: Pointer to the data buffer.
  - **len**: Length of the data in bytes.
  - **pName**: (Optional) Name or label for the data being dumped.
  - **colWidth**: (Optional) Number of bytes per line (default: 16).

- **std::string toHexStr(const std::string &strIn)**  
  Converts a given string into its hexadecimal representation.
  - **strIn**: Input string to be converted to hex.

- **size_t strReplace(std::string &strIn, const std::string &strFind, const std::string &strReplacement)**  
  Replaces all occurrences of **strFind** in **strIn** with **strReplacement**. Returns the number of replacements made.

### JSON Utilities
(Requires **JsonCpp** support, controlled by **CONFIG_LIB_DSPC_HAVE_JSONCPP**)

- **bool jKeyFind(const Json::Value &val, const std::string &nameKey)**  
  Searches for a key in a JSON object.
  - **val**: JSON object to search within.
  - **nameKey**: Name of the key to find.
  
  **Returns**: `true` if the key is found, otherwise `false`.

- **void jsonPrint(const Json::Value &val)**  
  Prints a formatted JSON object to standard output for debugging.

### Cryptography Utilities
(Requires **CryptoPP** support, controlled by **CONFIG_LIB_DSPC_HAVE_CRYPTOPP**)

- **std::string md5(const std::string &msg, const std::string &prefix = "")**  
  Computes the MD5 hash of the input string **msg**.
  - **msg**: Input message to be hashed.
  - **prefix**: (Optional) Prefix added to the hash result.

- **std::string sha256(const std::string &msg, const std::string &prefix = "")**  
  Computes the SHA-256 hash of the input string **msg**.
  - **msg**: Input message to be hashed.
  - **prefix**: (Optional) Prefix added to the hash result.

- **bool isValidSha256(const std::string &digest)**  
  Validates if a given string is a valid SHA-256 hash.

- **std::string hmacSha256(const std::string &msg, const CryptoPP::SecByteBlock &key)**  
  Computes the HMAC-SHA256 of the input string **msg** using the provided key.
  - **msg**: Input message.
  - **key**: Secret key used in HMAC.

### Curl Utilities
(Requires **Curl** support, controlled by **CONFIG_LIB_DSPC_HAVE_CURL**)

- **void curlGlobalInit()**  
  Initializes the global Curl environment. This must be called before any other Curl operations.

- **void curlGlobalDeInit()**  
  Deinitializes the global Curl environment, cleaning up any resources used by Curl.

- **std::string versionCurl()**  
  Retrieves the version information of Curl as a string.

- **void versionCurlInfoPrint()**  
  Prints detailed version information about the Curl library, including supported features.

### Networking Utilities

- **bool isValidEmail(const std::string &mail)**  
  Checks if the provided email address is syntactically valid.
  - **mail**: Email address to validate.

- **bool isValidIp4(const std::string &ip)**  
  Validates an IPv4 address string.
  - **ip**: IPv4 address as a string.

### String Utilities

- **void strToVecStr(const std::string &str, VecStr &vStr, char delim = '\n')**  
  Splits a string into a vector of substrings using a delimiter.
  - **str**: The input string to be split.
  - **vStr**: Vector to store the split substrings.
  - **delim**: Delimiter used to split the string (default: newline).

## EXAMPLES
```cpp
// Example of MD5 hashing using LibDspc
std::string message = "Hello, World!";
std::string hash = md5(message);
std::cout << "MD5 Hash: " << hash << std::endl;

// Example of using hexDump for debugging
const char data[] = "Example data";
hexDump(data, sizeof(data), "Data Dump");
```

## SEE ALSO
- `Processing()`, `LibTime`, `LibFilesys`

## AUTHORS
Written by Johannes Natter.

