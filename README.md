# SPI devices handle

## Main Purpose
	To provide interfaces with various spi devices on Beaglebone capes as well as GPIO capabilities.

## Current Version
### Ver: 0.12

## Currently Implemented

* BME280 atmospheric chip
  - Barometric sensor
  - Humidity sensor
  - Temperature sensor

* BMX055 positional chip
  - Accelerometer module
  - Gyroscope module

* GPIO handleing

## TODO
### Add/Implement

* BMX055
  - Magnetometer module
  - Additional accelerometer functions
    - Self test
  - Additional Gyroscope functions
    - Self test

### Potential Features

* BMX055
  - Additional accelerometer functions
    - Interupt engines
    - few-time non-volitile memory management
    - digital interfaces(I2C/3wire SPI)
    - offselts
    - interupts
  - Additional gyroscope functions
    - Interupt engines
    - few-time non-volitile memory management
    - digital interfaces(I2C/3wire SPI)
    - offselts
    - interupts

### Refine

* BMX055
  - Power managment


* BME280

## Credits
Cem Eden

Rebecca White

Gabe Smolnycki

