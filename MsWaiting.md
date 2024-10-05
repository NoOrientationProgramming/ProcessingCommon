
# MS_WAITING() Manual Page

## NAME
**MsWaiting** – Implements millisecond delays and manages processing status in C++.

## SYNOPSIS
```cpp
#include "MsWaiting.h"

MsWaiting *create(uint32_t duration);

Success process();
```

## DESCRIPTION
**MsWaiting()** is a C++ class used to implement delays in milliseconds. It allows waiting for a specified period and manages processing during that time. This class is useful in applications that require synchronized delays or pauses in execution.

### Features:
- **Time Control**: Set a duration in milliseconds when creating an instance with `create()`.
- **Processing Management**: The `process()` method manages the waiting status, allowing the application to remain in a controlled state during the waiting period.

### Structs:
- This class does not contain specific structures but manages internal variables for time control.

## METHODS

### Wait Management
- **create(uint32_t duration)**  
  Allocates a new instance of the **MsWaiting()** class with the specified waiting time in milliseconds.

- **process()**  
  Executes the waiting process logic and enables management of the time delay.

## RETURN VALUES
Methods that modify the status or configuration typically return a `Success` value, indicating the status of the processing.

## NOTES
- This class provides a simple way to implement millisecond delays and can be useful in multithreaded or asynchronous environments.
- The class is not copyable or assignable to prevent unintended resource sharing or duplication.

## SEE ALSO
- `Processing()`

## AUTHORS
Written by Johannes Natter.

