# SPI devices handle

## Main Purpose
	To provide interfaces with various spi devices on Beaglebone capes as well as GPIO capabilities.

## Current Version
### Ver: 0.20

## Currently Implemented

* BME280 atmospheric chip
  - Barometric sensor
  - Humidity sensor
  - Temperature sensor

* BMX055 positional chip
  - Accelerometer module
  - Gyroscope module
  - Magnetometer module

* GPIO handleing

## TODO
### Add/Implement

* BMX055
  - Additional accelerometer functions
    - Self test
  - Additional gyroscope functions
    - Self test
  - Additional magnetometer functions
    - Self Test

### Potential Features

* BMX055
  - Additional accelerometer functions
    - tnterupt engines
    - few-time non-volitile memory management
    - digital interfaces(I2C/3wire SPI)
    - offselts
    - interupts
  - Additional gyroscope functions
    - tnterupt engines
    - few-time non-volitile memory management
    - digital interfaces(I2C/3wire SPI)
    - offselts
    - interupts
  - Additional  functions
    - interupts
    - temperature compention

### Refine

* BMX055
  - Power managment

* BME280

## Credits
Cem Eden

Rebecca White

Gabe Smolnycki

