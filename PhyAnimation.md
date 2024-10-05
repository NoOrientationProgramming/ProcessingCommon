
# PHY_ANIMATING() Manual Page

## NAME
**PhyAnimating** – Implements physical animations and user interfaces in C++ using Qt and OpenCV.

## SYNOPSIS
```cpp
#include "PhyAnimating.h"

PhyAnimating *create(const char *name);

Success process();
Success shutdown();
Success animate();
Success animShutdown();
```

## DESCRIPTION
**PhyAnimating()** is a C++ class designed for creating physical animations and managing user interfaces. It uses the Qt library for the graphical user interface and allows interaction with animated elements within an application. The class provides an easy way to add, update, and manage controls and animations.

### Features:
- **User Interface Components**: Allows the addition of labels, buttons, checkboxes, sliders, input fields, and progress bars to the user interface.
- **Animation Management**: Provides methods for starting and stopping animations using the `animate()` and `animShutdown()` methods.
- **Signal and Slot Mechanism**: Supports interaction between user interface components and backend logic through Qt's signal and slot system.
- **Chart Visualization**: Enables adding charts for visualizing data.

### Structs:
- **LabelInfo**: Structures information about labels, including pointers to QLabel, prefix, unit, and maximum value.

## METHODS

### Process Management
- **create(const char *name)**  
  Allocates a new instance of the **PhyAnimating()** class with the specified name for the user interface.

- **process()**  
  Executes the logic for handling animations and user interactions.

- **shutdown()**  
  Shuts down the animation process and releases all resources.

- **animate()**  
  A virtual method that must be implemented by derived classes to define the animation logic.

- **animShutdown()**  
  A virtual method that must be implemented by derived classes to define the shutdown logic for animations.

### User Interface Management
- **uiLineAdd(const std::string &strLabel = "")**  
  Adds a new line to the user interface.

- **uiLabelAdd(const std::string &strPrefix = "", bool alignRight = true)**  
  Adds a new label to the user interface.

- **uiButtonAdd(const std::string &strText)**  
  Adds a button to the user interface.

- **uiSwitchAdd(const std::string &strLabel)**  
  Adds a checkbox to the user interface.

- **uiSliderAdd(int valMax, int valStart, const std::string &strPrefix, const std::string &strUnit = "", bool isTwoSided = false)**  
  Adds a slider to the user interface.

- **uiLineEditAdd(const std::string &strLabel = "")**  
  Adds an input field to the user interface.

- **uiProgressAdd(const std::string &strLabel)**  
  Adds a progress bar to the user interface.

- **uiChartAdd()**  
  Adds a chart to the user interface.

## RETURN VALUES
Methods that modify the status or configuration typically return a `Success` value, indicating the status of the processing.

## NOTES
- This class requires Qt6 and optionally OpenCV to support the user interface and animations.
- The class is not copyable or assignable to prevent unintended resource sharing or duplication.

## SEE ALSO
- `Processing()`, `QApplication`, `QPushButton`, `QSlider`, `QProgressBar`, `QChart`

## AUTHORS
Written by Johannes Natter.

