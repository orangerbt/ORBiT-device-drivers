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

apt-get update
apt-get install ppp

cp ./pppFiles/radioProg /etc/ppp/

if [ $baseStation == 1 ]; then
	cp ./pppFiles/optionsBase /etc/ppp/options
	cp ./pppFiles/startupBase.sh /etc/ppp/startup.sh
else
	cp ./pppFiles/optionsRocekt /etc/ppp/options
	cp ./pppFiles/startupRocket.sh /etc/ppp/startup.sh
fi

cp ./pppFiles/start_ppp /etc/init.d/


echo Done!
