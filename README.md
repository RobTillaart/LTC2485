
[![Arduino CI](https://github.com/RobTillaart/LTC2485/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/LTC2485/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/LTC2485/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/LTC2485/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/LTC2485/actions/workflows/jsoncheck.yml)
[![GitHub issues](https://img.shields.io/github/issues/RobTillaart/LTC2485.svg)](https://github.com/RobTillaart/LTC2485/issues)

[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/LTC2485/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/LTC2485.svg?maxAge=3600)](https://github.com/RobTillaart/LTC2485/releases)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/robtillaart/library/LTC2485.svg)](https://registry.platformio.org/libraries/robtillaart/LTC2485)


# LTC2485

Arduino library for LTC2485 I2C 24 bit ADC.


## Description

**Experimental**

Not tested with hardware yet

LTC2485 is an experimental library for the LTC2485 IC which is typically used
to monitor a voltage.

Read the datasheet for the details.

As always, feedback is welcome.


### I2C Address

From datasheet - table 4

|  CA1       |  CA0       |  address  |
|:----------:|:----------:|:---------:|
|  low       |  high      |    0x14   |
|  low       |  floating  |    0x15   |
|  floating  |  high      |    0x17   |
|  floating  |  floating  |    0x24   |
|  high      |  high      |    0x26   |
|  high      |  floating  |    0x27   |


### Related

- https://github.com/RobTillaart/LTC2485
- https://github.com/RobTillaart/temperature converters


## Interface

```cpp
#include "LTC2485.h"
```

### Constructor and setup

- **LTC2485(uint8_t address, TwoWire \*wire = Wire)**
The address is 0x48..0x4F depending on the address pins.
The library does not check the range.
- **bool begin()** UNO ea. initializes the class.
Returns true if the LTC2485 address is on the I2C bus.  
Note: do call **Wire.begin()** before **begin()**
- **bool isConnected()** Returns true if the LTC2485 address is on the I2C bus.
- **uint8_t getAddress()** Returns the address set in the constructor.


### Configure

- **uint8_t configure(uint8_t value)** set flags for next conversion.


Configuration bits, should be OR-ed.

|  name                     |  value  |
|:--------------------------|:-------:|
|  LTC2485_REJECT_50_60_HZ  |  0x00   |
|  LTC2485_SPEED_1X         |  0x00   |
|  LTC2485_SPEED_2X         |  0x01   |
|  LTC2485_REJECT_50HZ      |  0x02   |
|  LTC2485_REJECT_60HZ      |  0x04   |
|  LTC2485_INTERNAL_TEMP    |  0x08   |

example
```cpp
LTC.configure(LTC2485_SPEED_2X | LTC2485_REJECT_60HZ);
```


### Read

Read returns the last conversion and triggers a new conversion at the end.
So it might be needed to discard a first read.

- **int32_t getADC()** returns ADC value. Will configure ADC mode automatically.
- **float getVolts()** converts ADC value to volts.
- **float getMilliVolts()** converts ADC value to millivolts.
- **float getMicroVolts()** converts ADC value to microvolts.
- **float getTemperature()** returns internal temperature.
Will configure temperature mode automatically.


### Performance

No data available yet.
To be measured when hardware is available...
if you happen to have performance figures, please share them in an issue.


## Future

#### Must

- improve documentation
- get hardware to test library


#### Should

- error handling.


#### Could

- calibrate internal temperature, something like
  - **void calibrateTemperature(float A, float B)**  420.0  1.40
  - TC = 27 + (Voltage - A) x B;
- check pin compatible devices as derived class?
  - LTC2481 (16 bits) and LTC2483 (16 bits)


#### Wont


## Support

If you appreciate my libraries, you can support the development and maintenance.
Improve the quality of the libraries by providing issues and Pull Requests, or
donate through PayPal or GitHub sponsors.

Thank you,

