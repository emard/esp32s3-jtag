/*
Tools->Board->Boards Manager->esp32 by espressif
Tools->USB CDC On Boot: Enabled
Tools->CPU Frequency: 80 MHz (WiFi)
Tools->Board: XIAO_ESP32S3
Tools->JTAG Adapter: Integrated USB JTAG
Tools->USB Mode: Hardware CDC and JTAG
*/

#include "soc/usb_serial_jtag_reg.h" // JTAG

/*
references
https://esp32.com/viewtopic.php?t=25670
https://eloquentarduino.com/posts/esp32-cam-quickstart
*/

#define LED_ON  LOW
#define LED_OFF HIGH

/* arguments are GPIO pin numbers like (1,2,3,4,5) */
//   route_usb_jtag_to_gpio(      1,       2,       3,       4,        5);
void route_usb_jtag_to_gpio(int tck, int tms, int tdi, int tdo, int srst)
{
  WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG, 
    READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) | USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
  // esp_rom_gpio_connect_out_signal(GPIO, IOMATRIX, false, false);
  esp_rom_gpio_connect_out_signal(tck,   85, false, false); // tck
  esp_rom_gpio_connect_out_signal(tms,   86, false, false); // tms
  esp_rom_gpio_connect_out_signal(tdi,   87, false, false); // tdi
  esp_rom_gpio_connect_in_signal (tdo,  251, false);        // tdo
  esp_rom_gpio_connect_out_signal(srst, 251, false, false); // srst, wire to reset or EN of target
}

void setup()
{
  route_usb_jtag_to_gpio(1,2,3,4,5);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_ON);
}

void loop() {
  digitalWrite(LED_BUILTIN, (((micros()>>16) & 15)==0)^LED_OFF );
}
