#!/bin/sh
#OPENOCD=$(find ~/.arduino15 -name openocd -type f)
#OPENOCD=$(which openocd)
#OPENOCD=/usr/bin/openocd
#OPENOCD=/tmp/openocd/src/openocd
OPENOCD=openocd
$OPENOCD -f esp_usb_jtag.cfg -f ecp5_25f.ocd
