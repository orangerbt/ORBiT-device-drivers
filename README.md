# ORBiT Device Drivers

## Main Purpose

Provide interfaces with various SPI devices on BeagleBone capes as well as GPIO capabilities.

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

* GPIO handling

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
    - interupt engines
    - few-time non-volitile memory management
    - digital interfaces(I2C/3wire SPI)
    - offselts
    - interupts
  - Additional gyroscope functions
    - interupt engines
    - few-time non-volitile memory management
    - digital interfaces(I2C/3wire SPI)
    - offselts
    - interupts
  - Additional  functions
    - interupts
    - temperature compention

### Refine

* BMX055
  - Power management

* BME280

## Device Tree Overlays

To compile a device tree overlay, use the command:

    dtc -O dtb -o OUTFILE_NAME.dtbo -b 0 -@ INFILE_NAME.dts
	
Where OUTPUTFILE_NAME.dtbo and INFILE_NAME.dts have to match the name given
in your .dts file under partnumber and version in the format:

    <partnumber>-<version>.dts
	
To make it reachable by the system, copy your output file using:

    sudo cp OUTPUTFILE_NAME.dtbo /lib/firmware/
	
To allow the cape to be loaded, open the file uEnv.txt using:

    sudo nano /boot/uEnv.txt
	
Once open, find the line:

    cape_enable=bone_capemgr.enable_partno= [...]
	
Where [...] will be a comma seperated list of the partnumbers of all the enabled capes.


After a reboot, your newly added DTO should be in the list when the following command is entered:

    cat /sys/devices/platform/bone_capemgr/slots
	
If the list doesn't contain your DTO, then there was an issue in loading it.
You can check for boot time errors with the command:

    dmesg
	
For examples on how default DTOs are implemented, see the directory:

    /opt/source/bb.org-overlays/src/arm

	
For some functions, some capes may have to be loaded earlier, for instance the SPI0 bus.
For these kinds of DTOs, you may have to add the partnumbers to the file found under:

    sudo nano /etc/default/capemgr

## Credits
Cem Eden

Rebecca White

Gabe Smolnycki

