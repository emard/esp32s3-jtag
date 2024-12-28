# ESP32S3 USB JTAG

ESP32S3 and ESP32C3 have built-in hardware USB JTAG
by default routed to JTAG of internal CPU but it seems
possible to route this USB JTAG to physical GPIO pins.

Here is short arduino code to initialize such JTAG.

ESP patched OpenOCD to supports this JTAG.
On arduino board support "esp32 by espressif" it can be
found here:

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

