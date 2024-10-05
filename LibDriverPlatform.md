
# LIB_DRIVER_PLATFORM() Manual Page

## NAME
**LibDriverPlatform** – A platform abstraction for driver management and configuration in embedded systems.

## SYNOPSIS
```cpp
#include "LibDriverPlatform.h"

void *driverPlatformCreate(FuncInternalDrive pFctDrive, void *pProc, void *pConfigDriver);
void driverPlatformCleanUp(void *pDriver);
size_t sizeStackGet();

class ConfigDriver {
public:
    ConfigDriver();
    static void sizeStackDefaultSet(size_t sizeStack);
    size_t mSizeStack;
};
```

## DESCRIPTION
The **LibDriverPlatform** provides an abstraction layer for creating and managing platform-specific drivers, offering functions for initializing drivers and managing configuration settings like stack size. The primary goal is to offer a flexible interface that can be used across various driver implementations, while maintaining platform independence.

The library includes a `ConfigDriver` class for driver configuration and several global functions for platform driver management.

## FUNCTIONS

### Driver Platform Management

- **void \*driverPlatformCreate(FuncInternalDrive pFctDrive, void \*pProc, void \*pConfigDriver)**  
  Creates and initializes a platform-specific driver using the function pointer `pFctDrive`, which points to the internal driving function.  
  - **pFctDrive**: Function pointer for internal driver operation.
  - **pProc**: A pointer to a processing object.
  - **pConfigDriver**: A pointer to a driver configuration object of type `ConfigDriver`.
  
  **Returns**: A pointer to the created driver instance, or `NULL` on failure.

- **void driverPlatformCleanUp(void \*pDriver)**  
  Cleans up and releases resources associated with a driver platform. This function ensures that the memory and resources allocated during the creation of the driver are properly freed.
  - **pDriver**: A pointer to the driver instance to clean up.

- **size_t sizeStackGet()**  
  Retrieves the current default stack size for driver operations. This is used to query the size set by either the system default or a custom setting via `sizeStackDefaultSet()`.

### Configuration

- **void ConfigDriver::sizeStackDefaultSet(size_t sizeStack)**  
  Sets the default stack size for the driver platform. This allows adjusting the stack size used by drivers created by this platform.
  - **sizeStack**: The new default stack size, in bytes.

### Constructors

- **ConfigDriver::ConfigDriver()**  
  Default constructor for the `ConfigDriver` class. It initializes the `mSizeStack` member with the default stack size, which is defined by the static member `sizeStackDefault`.

## MEMBER VARIABLES

### ConfigDriver

- **size_t ConfigDriver::mSizeStack**  
  The stack size for the driver, set at initialization or changed by `sizeStackDefaultSet()`. If not explicitly set, it defaults to `sizeStackDefault`.

## STATIC VARIABLES

- **static size_t ConfigDriver::sizeStackDefault**  
  A static variable holding the default stack size for all drivers. This value is shared across all `ConfigDriver` instances and can be set via the `sizeStackDefaultSet()` method.

## EXAMPLES
```cpp
// Example usage of LibDriverPlatform

// Create a ConfigDriver object and set the stack size
ConfigDriver config;
ConfigDriver::sizeStackDefaultSet(4096);

// Create a driver platform using a custom drive function and config
void *pDriver = driverPlatformCreate(myDriveFunction, pProcessing, &config);

// Perform operations...

// Clean up driver resources
driverPlatformCleanUp(pDriver);

// Get the current stack size
size_t stackSize = sizeStackGet();
```
In this example, a `ConfigDriver` object is created, and the default stack size is set. Then, a platform-specific driver is created using a custom driving function, and after usage, the driver is cleaned up.

## SEE ALSO
- `Processing()`

## AUTHORS
Written by Johannes Natter.

