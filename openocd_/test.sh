#!/bin/sh
OPENOCD=$(find ~/.arduino15 -name openocd -type f)
$OPENOCD -f esp_usb_jtag.cfg -f ecp5_25f.ocd
