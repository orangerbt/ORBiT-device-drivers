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
	echo 1 > /proc/sys/net/ipv4/ip_forward
	route add -net 10.0.0.0/16 dev eth0
	iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
else
	#rocket code
	#sudo /sbin/route add default gw 192.168.1.201
	sudo /sbin/route add default gw 192.168.1.201

	sudo echo nameserver 8.8.8.8 >> /etc/resolv.conf
	sudo echo nameserver 8.8.4.4 >> /etc/resolv.conf
	ntpd pool.ntp.org
fi

echo Done!
