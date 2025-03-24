# ESP32S3 USB JTAG

ESP32S3 has built-in hardware USB JTAG by default
routed to JTAG of internal CPU. It is possible to
route this USB JTAG to physical GPIO pins
from user firmware. No efuses need to be burned.
Speed is about 2 Mbit/s. Supported by openFPGALoader
(branch: esp_usb_jtag) and openocd (needs patch).

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
             TX │43         44│ RX
                └─────────────┘
                    top view  

Here is short arduino code to initialize such JTAG and

# Binary firmware for ESP32S3

For quickstart just upload binary version. One firmware should be good for most
ESP32S3 boards not only XIAO. Just connect board's GPIO1-5 pins to target JTAG
as on above drawing.

To program binary firmware we use "esptool.py":

    pipx install esptool
    cd binary
    ./flash.sh esp32s3-jtag.ino.merged.bin

"flash.sh" actually does

    esptool.py --port /dev/ttyACM0 erase_flash
    esptool.py --port /dev/ttyACM0 write_flash -z 0 esp32s3-jtag.ino.merged.bin

ESP JTAG is enabled only when ESP USB is plugged to PC.
When ESP USB is unplugged, ESP releases JTAG to HI-Z state
which allows JTAG free to be used by other devices.

# Linux

For convenience here is udev rule to allow non-root users members of
"dialout" group to acces JTAG.

    # this is for libusb usb-jtag access
    ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", \
    GROUP="dialout", MODE="666"

# openFPGALoader

To compile currently we should use "esp_usb_jtag" branch.

    git clone https://github.com/trabucayre/openFPGALoader
    cd openFPGALoader
    git checkout esp_usb_jtag
    mkdir build
    cd build
    cmake ..
    make

Currently it is tested on Lattice ECP5 FPGA.
Other target chips may have issues.
openFPGALoader works on fixing small bugs.

    ./openFPGALoader -c esp32s3 ulx3s_25f_oscg_blink.bit
    empty
     0x1 0xa 0x1 0x8 0x40 0x1f 0x1 0x0 0xff 0x0
    esp_usb_jtag: Device found. Base speed 40000 KHz, div range 1 to 255
    Jtag frequency : requested 6000000Hz -> real 6000000Hz divisor=6
    Open file: DONE
    b3bdffff
    Parse file: DONE
    Enable configuration: DONE
    SRAM erase: DONE
    Loading: [==================================================] 100.00%
    Done
    Disable configuration: DONE
    drain_in

# openocd

Currently "stock" openocd doesn't work good enough for programming
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
patched to work with esp32s3-jtag or apply
[espfix.patch](openocd/espfix.patch) to mainstream openocd
and compile

    ./bootstrap
    ./configure --disable-dummy
    make

To compile esp32s3 firmware install arduino
then install board support "esp32 by espressif"
and select board with esp32s3 like "XIAO_ESP32S3".

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

    [ ] micropython version
        https://docs.micropython.org/en/latest/esp32/tutorial/peripheral_access.html
    [ ] ESP32C3, C6, H2, ...
