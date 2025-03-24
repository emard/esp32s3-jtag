#!/bin/sh
#pipx install esptool
ESPTOOL=esptool.py
#ESPTOOL=/home/guest/.local/bin/esptool.py
$ESPTOOL --port /dev/ttyACM0 erase_flash
$ESPTOOL --port /dev/ttyACM0 write_flash -z 0 $1
