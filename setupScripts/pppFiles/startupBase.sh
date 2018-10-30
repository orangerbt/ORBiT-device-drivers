#!/bin/sh

runpppd(){
	until pppd; do
		echo "Respawning..."
		sleep 1
	done
}

sleep 30

cd /etc/ppp/
./radioProg -d /dev/ttyUSB0 --try-reset --set-serial-baud 19200

if [ $? -eq 0 ]; then
	runpppd
else
	echo "Failed to set radio baud rate!"
	echo "Assuming 19200 is already set..."
	runpppd
fi
