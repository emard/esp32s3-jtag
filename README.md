# ESP32S3 USB JTAG

ESP32S3 and ESP32C3 have built-in hardware USB JTAG
by default routed to JTAG of internal CPU. It is
possible to route this USB JTAG to physical GPIO pins.

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

Modern linux already has openocd which supports esp32s3 usb jtag.

    apt install openocd

On arduino install board support "esp32 by espressif".
There is patched OpenOCD to supports this JTAG.
It can be found here:

    find ~/.arduino15 -name openocd
    .arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240821/bin/openocd
    .arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240821/share/openocd

    find ~/.arduino15/packages/esp32/tools/ -name esp32s3-builtin.cfg
    .arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240821/share/openocd/scripts/board/esp32s3-builtin.cfg

    find ~/.arduino15/packages/esp32/tools/ -name esp_usb_jtag.cfg
    .arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240821/share/openocd/scripts/interface/esp_usb_jtag.cfg

    adapter driver esp_usb_jtag
    espusbjtag vid_pid 0x303a 0x1001
    espusbjtag caps_descriptor 0x2000
    adapter speed 40000

    ~/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240821/bin/openocd
      -f ~/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240821/share/openocd/scripts/interface/esp_usb_jtag.cfg

On linux udev rules are needed (users should be members of "dialout" group):

    # file: /etc/udev/rules.d/50-esp32s3.rules
    # this is for usbserial device
    SUBSYSTEM=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", \
    MODE="664", GROUP="dialout"
    # this is for libusb usb-jtag access
    ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", \
    GROUP="dialout", MODE="666"

Using arduino serial monitor or any other serial terminal
we can enable or disable jtag:

    0 disable JTAG and turn LED OFF
    1 enable  JTAG and turn LED ON

It detects FPGA chip ID so JTAG hardware works.
But there's some incompatibility issues with
generated .svf file and openocd. We are close
but our FPGA is not programmed yet.

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

This is problematic command generating BUG.
I don't know why, it looks normal:

    RUNTEST IDLE    2 TCK   1.00E-02 SEC; 

The patched OpenOCD is here

https://github.com/espressif/openocd-esp32

There is a code which should disable usb-jtag and
put esp32s3 jtag pins high-z to be used as normal gpio.
If external jtag is connected, this is one of possible
ways to check that it is working:

    openFPGALoader --board ulx3s --detect

# TODO

Detect when usb-serial is connected or disconnected
then enable or disable physical gpio jtag.