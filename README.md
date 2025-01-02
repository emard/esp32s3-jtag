# ESP32S3 USB JTAG

ESP32S3 and ESP32C3 have built-in hardware USB JTAG
by default routed to JTAG of internal CPU. It is
possible to route this USB JTAG to physical GPIO pins
from user firmware. No efuses need to be burned.
Speed is about 2 Mbit/s.

    XIAO ESP32S3 MINI
    Mouser P/N: 713-113991114    7$

                 XIAO  ESP32S3
                    ┌─────┐
                ┌───┘ USB └───┐
            TCK │1          5V│
            TMS │2         GND│ GND
            TDI │3        3.3V│ 3.3V
            TDO │4           9│
           SRST │5           8│
                │6           7│
                │43 TX   RX 44│
                └─────────────┘
                    top view  


Here is short arduino code to initialize such JTAG.

Currently openocd doesn't work good enough for programming
FPGA with esp32s3 because it doesn't support "RUNTEST" command
which is used to drive clock without any other data being sent.
for example

    RUNTEST IDLE    2 TCK   1.00E-02 SEC; 

Will generate "BUG" in all current known openocd's.

    cd openocd
    ./test.sh
    Open On-Chip Debugger v0.12.0-esp32-20240821 (2024-08-21-14:42)
    Licensed under GNU GPL v2
    For bug reports, read
    	http://openocd.org/doc/doxygen/bugs.html
    Info : only one transport option; autoselecting 'jtag'
    Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
    Info : esp_usb_jtag: capabilities descriptor set to 0x2000
    adapter speed: 40000 kHz
    Info : esp_usb_jtag: serial (64:E8:33:50:B2:30)
    Info : esp_usb_jtag: Device found. Base speed 40000KHz, div range 1 to 255
    Info : clock speed 40000 kHz
    Info : JTAG tap: lfe5.tap tap/device found: 0x41111043 (mfg: 0x021 (Lattice Semi.), part: 0x1111, ver: 0x4)
    Warn : gdb services need one or more targets defined
    svf processing file: "bitstream.svf"
    95%    Error: BUG: unknown JTAG command type encountered

To fix this BUG, use [openocd fork](https://github.com/emard/openocd)
patched to work with esp32s3-jtag or or apply
[espfix.patch](openocd/espfix.patch) to mainstream openocd
and compile

    ./bootstrap
    ./configure --disable-dummy
    make

To compile esp32s3 firmware install arduino
then install board support "esp32 by espressif"
and select adequate board with "ESP32S3".

This openocd configuration file is important:

    # esp_usb_jtag.ocd
    adapter driver esp_usb_jtag
    espusbjtag vid_pid 0x303a 0x1001
    espusbjtag caps_descriptor 0x2000
    adapter speed 40000

Example typical usage, openocd file to write bitstream.svf to ECP5 FPGA

    # ecp5_25f.ocd
    # telnet_port 4444
    # gdb port 3333
    jtag newtap lfe5 tap -expected-id 0x41111043 -irlen 8 -irmask 0xFF -ircapture 0x5
    init
    scan_chain
    svf -tap lfe5.tap -quiet -progress bitstream.svf
    shutdown

use them together

    openocd-espusbjtag -f esp_usb_jtag.ocd -f ecp5_25f.ocd

On linux udev rules are needed (users should be members of "dialout" group):

    # file: /etc/udev/rules.d/50-esp32s3.rules
    # this is for usbserial device
    SUBSYSTEM=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", \
    MODE="664", GROUP="dialout"
    # this is for libusb usb-jtag access
    ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", \
    GROUP="dialout", MODE="666"

ESP JTAG is enabled only when usb is plugged to PC.
When usb is unplugged ESP releases JTAG to HI-Z state
which allows JTAG free to be used by other devices.

# Protocol and source reference

espressif's edition of OpenOCD is here

https://github.com/espressif/openocd-esp32

It's in officual OpenOCD, driver and protocol description is here

https://github.com/openocd-org/openocd/blob/master/src/jtag/drivers/esp_usb_jtag.c

Here is the ESP32S2 or ESP32S3 usb jtag bridge project
which has firmware implementation of its JTAG protocol.
It should be same protocol used in usb-jtag hardware.
Hopfully this source can give answers to undocumented
details of the protocol.

https://github.com/espressif/esp-usb-bridge

# TODO

    [x] Detect when usb-serial is connected or disconnected
        then enable or disable physical gpio jtag.
    [ ] micropython version
        https://docs.micropython.org/en/latest/esp32/tutorial/peripheral_access.html
  
