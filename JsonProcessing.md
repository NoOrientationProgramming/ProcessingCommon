
# JSON_PROCESSING() Manual Page

## NAME
**JsonProcessing()** – A class for processing JSON data with input/output pipes and argument/result management.

## SYNOPSIS
```cpp
#include "JsonProcessing.h"

void setArguments(Json::Value &args);
const Json::Value &result() const;

Pipe<Json::Value> ppJsonIn;
Pipe<Json::Value> ppJsonOut;
```

## DESCRIPTION
**JsonProcessing()** is a C++ class that extends the **Processing** class and provides a framework for processing JSON data. It is designed to manage JSON-based input and output streams via pipes, and to handle arguments and results in JSON format. This class is useful for applications where structured JSON data is passed between different processes or components.

### Features:
- **JSON Input and Output**: Utilizes **Pipe** objects for JSON input and output handling, allowing seamless transfer of data between different processing stages.
- **Arguments and Results**: Provides methods for setting input arguments and retrieving output results, both in JSON format.
- **Integration with JsonCpp**: Fully compatible with the JsonCpp library for parsing, manipulating, and outputting JSON.

## METHODS

### JSON Argument Handling
- **void setArguments(Json::Value &args)**  
  Sets the arguments for the JSON processing. The arguments are passed as a `Json::Value` object and stored in the member variable `mArgs`.

- **const Json::Value &result() const**  
  Returns the result of the JSON processing as a constant reference to a `Json::Value` object stored in the `mResult` member variable.

## MACROS
- **JS(x)**  
  A helper macro that returns the string value of a JSON object `x` if it is a string, otherwise returns an empty string.

```cpp
#define JS(x) (x.isString() ? x.asString().c_str() : "")
```

## USAGE WITH PIPES
**JsonProcessing** works closely with **Pipe\<Json::Value\>**, facilitating the transfer of JSON data between different components. The pipes allow for asynchronous or parallel processing of JSON messages.

## SEE ALSO
- `Processing()`, `Pipe`, `jsoncpp`

## AUTHORS
Written by Johannes Natter.

