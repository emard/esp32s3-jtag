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

#define PIN_TCK  1
#define PIN_TMS  2
#define PIN_TDI  3
#define PIN_TDO  4
#define PIN_SRST 5

/* arguments are GPIO pin numbers like (1,2,3,4,5) */
void route_usb_jtag_to_gpio()
{
  pinMode(PIN_TCK, OUTPUT);
  pinMode(PIN_TMS, OUTPUT);
  pinMode(PIN_TDI, OUTPUT);
  pinMode(PIN_TDO, INPUT);
  pinMode(PIN_SRST, OUTPUT);
  WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
    READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG)
  | USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
  // esp_rom_gpio_connect_out_signal(GPIO, IOMATRIX, false, false);
  esp_rom_gpio_connect_out_signal(PIN_TCK,   85, false, false);
  esp_rom_gpio_connect_out_signal(PIN_TMS,   86, false, false);
  esp_rom_gpio_connect_out_signal(PIN_TDI,   87, false, false);
  esp_rom_gpio_connect_out_signal(PIN_SRST, 251, false, false);
  esp_rom_gpio_connect_in_signal (PIN_TDO,  251, false);
}

void unroute_usb_jtag_to_gpio()
{
  WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
    READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG)
  & ~USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
  pinMode(PIN_TCK,  INPUT);
  pinMode(PIN_TMS,  INPUT);
  pinMode(PIN_TDI,  INPUT);
  pinMode(PIN_TDO,  INPUT);
  pinMode(PIN_SRST, INPUT);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_ON);
  route_usb_jtag_to_gpio();
  // unroute_usb_jtag_to_gpio();
}

void loop() {
  digitalWrite(LED_BUILTIN, (((micros()>>16) & 15)==0)^LED_OFF );
  // digitalWrite(LED_BUILTIN, digitalRead(2)^LED_OFF); // test input pin
}
