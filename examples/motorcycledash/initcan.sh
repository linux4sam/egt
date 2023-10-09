#!/bin/sh

ifconfig can0 down
ip link set can0 type can bitrate 250000
ifconfig can0 up
echo "CAN0 up"

