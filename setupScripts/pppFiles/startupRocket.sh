#!/bin/sh

runpppd(){
	until pppd; do
		echo "Respawning..."
		sleep 1
	done
}

sleep 30

cd /etc/ppp/
./radioProg -d /dev/ttyS4 --try-reset --set-serial-baud 19200

#/sbin/route add default gw 192.168.1.201

if [ $? -eq 0 ]; then
	runpppd
else
	echo "Failed to set radio baud rate!"
	echo "Assuming 19200 is already set..."
	runpppd
fi
