#!/bin/sh

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root"
	exit 1
fi

baseStation=0
rocket=0

while [ "$1" != "" ]; do
	case $1 in
		-b | --base )	baseStation=1
					;;
		-r | --rocket )	rocket=1
					;;
		* )		echo "Unknown option '$key'"
					;;
	esac
	shift
done

if [[ $baseStation == 0 ]] && [[ $rocket == 0 ]]; then
	echo "Error! Must define -b or -r!"
	exit 1
fi

if [[ $baseStation == 1 ]] && [[ $rocket == 1 ]]; then
	echo "Error! Cannot define -b and -r simultaniously!"
	exit 1
fi


if [ $baseStation == 1 ]; then
	#base station code

	#copy all device tree overlays into firmware directory
	cp ../dto/*.dtbo /lib/firmware/

	#copy boot settings
	cp ./dtoFiles/uEnv.txt /boot/
else
	#rocket code

	#copy all device tree overlays into firmware directory
	cp ../dto/*.dtbo /lib/firmware/

	#copy boot settings
	cp ./dtoFiles/uEnv.txt /boot/
fi

echo Done!
