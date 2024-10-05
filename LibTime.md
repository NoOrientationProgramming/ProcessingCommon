
# LIB_TIME Manual Page

## NAME
**LibTime** – A library for time management and manipulation using the C++ Standard Library's `chrono` utilities.

## SYNOPSIS
```cpp
#include "LibTime.h"

// Time utilities
uint32_t millis();

// Current Time
std::string nowUtc();
TimePoint nowTp();
std::string nowToStr(const char *pFmt = NULL);

// Time conversion
std::string tpToStr(const TimePoint &tp, const char *pFmt = NULL);
TimePoint strToTp(const std::string &str, const char *pFmt = NULL);

// Time difference
size_t tpDiffSec(const TimePoint &tpEnd, const TimePoint &tpStart);
size_t tpDiffMs(const TimePoint &tpEnd, const TimePoint &tpStart);
```

## DESCRIPTION
**LibTime** provides a set of functions for time management and formatting, making use of the C++ Standard Library's `chrono` system clock. It offers functionality for getting the current time, formatting time points as strings, and computing time differences in seconds and milliseconds.

## TIME UTILITIES

- **uint32_t millis()**  
  Returns the number of milliseconds since the system clock's epoch (similar to `millis()` in embedded programming).
  
  **Returns**: Milliseconds as an unsigned 32-bit integer.

## CURRENT TIME FUNCTIONS

- **std::string nowUtc()**  
  Returns the current UTC time as a formatted string.

  **Returns**: A string representing the current UTC time.

- **TimePoint nowTp()**  
  Returns the current time as a `TimePoint` object.
  
  **Returns**: A `TimePoint` object representing the current time.

- **std::string nowToStr(const char *pFmt = NULL)**  
  Returns the current time as a formatted string.
  - **pFmt**: A format string for the output (optional). If `NULL`, a default format will be used.

  **Returns**: A string representing the current time in the specified format.

## TIME CONVERSION FUNCTIONS

- **std::string tpToStr(const TimePoint &tp, const char *pFmt = NULL)**  
  Converts a `TimePoint` to a formatted string.
  - **tp**: The `TimePoint` to convert.
  - **pFmt**: A format string for the output (optional). If `NULL`, a default format will be used.

  **Returns**: A string representing the time point in the specified format.

- **TimePoint strToTp(const std::string &str, const char *pFmt = NULL)**  
  Converts a formatted string to a `TimePoint`.
  - **str**: The time string to convert.
  - **pFmt**: The format of the input string (optional). If `NULL`, a default format will be assumed.

  **Returns**: A `TimePoint` object representing the time from the string.

## TIME DIFFERENCE FUNCTIONS

- **size_t tpDiffSec(const TimePoint &tpEnd, const TimePoint &tpStart)**  
  Returns the difference between two `TimePoint` objects in seconds.
  - **tpEnd**: The later `TimePoint`.
  - **tpStart**: The earlier `TimePoint`.

  **Returns**: The difference in seconds as a size_t.

- **size_t tpDiffMs(const TimePoint &tpEnd, const TimePoint &tpStart)**  
  Returns the difference between two `TimePoint` objects in milliseconds.
  - **tpEnd**: The later `TimePoint`.
  - **tpStart**: The earlier `TimePoint`.

  **Returns**: The difference in milliseconds as a size_t.

## EXAMPLES
```cpp
// Example of getting the current time in various formats
std::string currentUtc = nowUtc();
std::cout << "Current UTC Time: " << currentUtc << std::endl;

TimePoint currentTp = nowTp();
std::string currentTimeStr = nowToStr();
std::cout << "Current Time: " << currentTimeStr << std::endl;

// Example of calculating time differences
TimePoint start = nowTp();
// Do something...
TimePoint end = nowTp();
size_t elapsedMs = tpDiffMs(end, start);
std::cout << "Elapsed time in milliseconds: " << elapsedMs << std::endl;
```

## SEE ALSO
- `Processing()`

## AUTHORS
Written by Johannes Natter.

