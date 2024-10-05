
# ESP_LED_PULSING() Manual Page

## NAME
**EspLedPulsing()** – Control LED pulsing behavior on ESP32 devices.

## SYNOPSIS
```cpp
#include "EspLedPulsing.h"

EspLedPulsing *create();

void pinSet(uint8_t id);
void paramSet(uint32_t width, uint32_t period, uint8_t count = 1, uint32_t gap = 0);
```

## DESCRIPTION
**EspLedPulsing()** is a C++ class designed to control the pulsing behavior of an LED on an ESP32 microcontroller. The class allows users to configure the pulse width, period, pulse count, and gap between pulses. This is useful for creating effects like blinking, pulsing, or signaling patterns using an LED connected to a GPIO pin.

### Features:
- **Pin Configuration**: Set the GPIO pin controlling the LED with `pinSet()`.
- **Pulse Parameters**: Define the pulse width, period, and optional gap between pulses using `paramSet()`.
- **Burst Mode**: Control how many times the LED should pulse consecutively using the `count` parameter.

## METHODS

### LED Configuration
- **create()**  
  Allocates a new instance of the **EspLedPulsing()** class.

- **pinSet(uint8_t id)**  
  Assigns the GPIO pin to which the LED is connected. The pin should be valid and supported by the ESP32 platform.

- **paramSet(uint32_t width, uint32_t period, uint8_t count = 1, uint32_t gap = 0)**  
  Configures the pulsing parameters:
  - **width**: Duration (in milliseconds) for which the LED stays ON in each pulse.
  - **period**: Total duration of each pulse cycle, including ON and OFF times.
  - **count**: Number of consecutive pulses in each burst (default is 1).
  - **gap**: Time (in milliseconds) between bursts of pulses (default is 0, meaning no gap).

## EXAMPLES
```cpp
  EspLedPulsing *pLed;

  pLed = EspLedPulsing::create();
  if (!pLed)
    return procErrLog(-1, "could not create process");

  pLed->pinSet(GPIO_NUM_2);  // Set GPIO pin 2
  pLed->paramSet(500, 1000, 5, 2000);  // 500ms ON, 1000ms cycle, 5 pulses, 2000ms gap

  start(pLed);
```

In this example, an LED connected to GPIO pin 2 will pulse with a width of 500ms, a period of 1000ms, and will repeat for 5 pulses, with a 2-second gap between bursts.

## RETURN VALUES
Most methods are `void` unless otherwise specified. The `process()` and `shutdown()` methods return `Success` to indicate whether the operation succeeded.

## NOTES
- The class is designed specifically for the ESP32 platform and uses ESP-IDF's GPIO driver to control the LED.
- The class is not copyable or assignable to prevent unintended behavior with GPIO resources.
- Make sure the GPIO pin you set with `pinSet()` is properly configured as an output pin before calling `process()`.

## SEE ALSO
- `Processing()`

## AUTHORS
Written by Johannes Natter.

