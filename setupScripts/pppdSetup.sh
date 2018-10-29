#!/bin/sh

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root"
	exit 1
fi

apt-get update
apt-get install ppp

cp ./pppFiles/options /etc/ppp/
cp ./pppFiles/radioProg /etc/ppp/
cp ./pppFiles/startup.sh /etc/ppp/

cp ./pppFiles/start_ppp /etc/init.d/


echo Done!
