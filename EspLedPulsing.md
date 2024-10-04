
# ESP_LED_PULSING(3) Manual Page

## NAME
**EspLedPulsing** – Control LED pulsing behavior on ESP32 devices.

## SYNOPSIS
```cpp
#include "EspLedPulsing.h"

EspLedPulsing *create();

void pinSet(uint8_t id);
void paramSet(uint32_t width, uint32_t period, uint8_t count = 1, uint32_t gap = 0);
```

## DESCRIPTION
**EspLedPulsing** is a C++ class designed to control the pulsing behavior of an LED on an ESP32 microcontroller. The class allows users to configure the pulse width, period, pulse count, and gap between pulses. This is useful for creating effects like blinking, pulsing, or signaling patterns using an LED connected to a GPIO pin.

### Features:
- **Pin Configuration**: Set the GPIO pin controlling the LED with `pinSet()`.
- **Pulse Parameters**: Define the pulse width, period, and optional gap between pulses using `paramSet()`.
- **Burst Mode**: Control how many times the LED should pulse consecutively using the `count` parameter.

## METHODS

### LED Configuration
- **create()**  
  Allocates a new instance of the **EspLedPulsing** class.

- **pinSet(uint8_t id)**  
  Assigns the GPIO pin to which the LED is connected. The pin should be valid and supported by the ESP32 platform.

- **paramSet(uint32_t width, uint32_t period, uint8_t count = 1, uint32_t gap = 0)**  
  Configures the pulsing parameters:
  - **width**: Duration (in milliseconds) for which the LED stays ON in each pulse.
  - **period**: Total duration of each pulse cycle, including ON and OFF times.
  - **count**: Number of consecutive pulses in each burst (default is 1).
  - **gap**: Time (in milliseconds) between bursts of pulses (default is 0, meaning no gap).

### Protected Methods
These methods are designed for internal use during the LED pulsing operation and are not meant to be called directly by users:

- **EspLedPulsing()**  
  Default constructor, initializes the internal state of the LED pulsing object.

- **~EspLedPulsing()**  
  Destructor, cleans up resources used by the LED pulsing object.

- **process()**  
  Starts the LED pulsing process according to the parameters set by `paramSet()`.

- **shutdown()**  
  Safely stops the LED pulsing process and resets any associated resources.

- **gpioInit()**  
  Initializes the GPIO pin for output to control the LED.

## MEMBER VARIABLES
The following internal variables manage the state and timing of the pulsing process:

- **mStartMs**  
  Stores the start time of the pulsing process.

- **mPin**  
  The GPIO pin number assigned to control the LED.

- **mCount**  
  The total number of pulses in a burst.

- **mCurCount**  
  Tracks the current number of pulses in the ongoing burst.

- **mWidth**  
  The ON duration (in milliseconds) of each pulse.

- **mPeriod**  
  The total time for each pulse cycle, including the ON and OFF phases.

- **mGap**  
  Time gap between bursts of pulses.

- **mNumBurstsDone**  
  Tracks the number of pulse bursts completed.

## EXAMPLES
```cpp
EspLedPulsing *ledPulsing = EspLedPulsing::create();
ledPulsing->pinSet(GPIO_NUM_2);  // Set GPIO pin 2
ledPulsing->paramSet(500, 1000, 5, 2000);  // 500ms ON, 1000ms cycle, 5 pulses, 2000ms gap
ledPulsing->process();  // Start the pulsing process
```

In this example, an LED connected to GPIO pin 2 will pulse with a width of 500ms, a period of 1000ms, and will repeat for 5 pulses, with a 2-second gap between bursts.

## RETURN VALUES
Most methods are `void` unless otherwise specified. The `process()` and `shutdown()` methods return `Success` to indicate whether the operation succeeded.

## NOTES
- The class is designed specifically for the ESP32 platform and uses ESP-IDF's GPIO driver to control the LED.
- The class is not copyable or assignable to prevent unintended behavior with GPIO resources.
- Make sure the GPIO pin you set with `pinSet()` is properly configured as an output pin before calling `process()`.

## SEE ALSO
- `gpio_config(3)`, `esp32(3)`

## AUTHORS
Written by Johannes Natter.

