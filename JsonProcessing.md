
# JSON_PROCESSING(3) Manual Page

## NAME
**JsonProcessing** – A class for processing JSON data with input/output pipes and argument/result management.

## SYNOPSIS
```cpp
#include "JsonProcessing.h"

void setArguments(Json::Value &args);
const Json::Value &result() const;

Pipe<Json::Value> ppJsonIn;
Pipe<Json::Value> ppJsonOut;
```

## DESCRIPTION
**JsonProcessing** is a C++ class that extends the **Processing** class and provides a framework for processing JSON data. It is designed to manage JSON-based input and output streams via pipes, and to handle arguments and results in JSON format. This class is useful for applications where structured JSON data is passed between different processes or components.

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

### Member Variables
- **Pipe\<Json::Value\> ppJsonIn**  
  A pipe for receiving JSON input data. This pipe can be used to pass data into the processing system.

- **Pipe\<Json::Value\> ppJsonOut**  
  A pipe for sending JSON output data. This pipe can be used to transmit the results or processed data to another stage.

- **Json::Value mArgs**  
  Stores the arguments passed into the processing system. This variable holds the input data in JSON format.

- **Json::Value mResult**  
  Stores the result of the processing. This variable holds the output data in JSON format.

### Protected Methods
- **JsonProcessing(const char *name)**  
  Constructor that initializes the JSON processing object with a given name, calling the base class **Processing** constructor.

- **~JsonProcessing()**  
  Virtual destructor that cleans up resources used by the object.

### Private Methods
- **JsonProcessing() = delete;**  
  Prevents the default constructor from being used.

- **JsonProcessing(const JsonProcessing &) = delete;**  
  Disables the copy constructor to avoid copying objects.

- **JsonProcessing &operator=(const JsonProcessing &) = delete;**  
  Disables the assignment operator to prevent assignment of objects.

## MACROS
- **JS(x)**  
  A helper macro that returns the string value of a JSON object `x` if it is a string, otherwise returns an empty string.

```cpp
#define JS(x) (x.isString() ? x.asString().c_str() : "")
```

## EXAMPLES
```cpp
JsonProcessing *jsonProc = new JsonProcessing("ExampleProcessor");

Json::Value inputArgs;
inputArgs["key"] = "value";
jsonProc->setArguments(inputArgs);

const Json::Value &result = jsonProc->result();
std::cout << "Result: " << result.toStyledString() << std::endl;
```
In this example, a new **JsonProcessing** object is created, JSON arguments are set using `setArguments()`, and the result is retrieved and printed.

## USAGE WITH PIPES
**JsonProcessing** works closely with **Pipe\<Json::Value\>**, facilitating the transfer of JSON data between different components. The pipes allow for asynchronous or parallel processing of JSON messages.

### Example:
```cpp
jsonProc->ppJsonIn.push(inputArgs);
jsonProc->ppJsonOut.pop(result);
```
In this example, data is pushed into the input pipe, and the processed result is popped from the output pipe.

## THREAD SAFETY
Thread safety is not explicitly guaranteed by this class. Proper synchronization mechanisms, such as mutexes, should be used if **JsonProcessing** is to be used in a multi-threaded environment.

## SEE ALSO
- `Processing(3)`, `jsoncpp(3)`, `Pipe(3)`

## AUTHORS
Written by Johannes Natter.

