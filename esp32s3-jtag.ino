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

#define RXD  44
#define TXD  43
#define BAUD 115200

#define PIN_TCK  1
#define PIN_TMS  2
#define PIN_TDI  3
#define PIN_TDO  4
#define PIN_SRST 5

#if CONFIG_IDF_TARGET_ESP32S3
#define OMATRIX_TCK   85
#define OMATRIX_TMS   86
#define OMATRIX_TDI   87
#define OMATRIX_SRST 251
#define IMATRIX_TDO  251
#endif

#if CONFIG_IDF_TARGET_ESP32C3
/* untested */
#define OMATRIX_TCK   36
#define OMATRIX_TMS   37
#define OMATRIX_TDI   38
#define OMATRIX_SRST 127
#define IMATRIX_TDO   39
#endif

#if CONFIG_IDF_TARGET_ESP32C6
/* incomplete */
#define OMATRIX_TCK
#define OMATRIX_TMS
#define OMATRIX_TDI
#define OMATRIX_SRST  19
#define IMATRIX_TDO   19
#endif

#if CONFIG_IDF_TARGET_ESP32H2
/* incomplete */
#define OMATRIX_TCK
#define OMATRIX_TMS
#define OMATRIX_TDI
#define OMATRIX_SRST  19
#define IMATRIX_TDO   19
#endif

/* arguments are GPIO pin numbers like (1,2,3,4,5) */
void route_usb_jtag_to_gpio()
{
  digitalWrite(LED_BUILTIN, LED_ON);
  pinMode(PIN_TCK, OUTPUT);
  pinMode(PIN_TMS, OUTPUT);
  pinMode(PIN_TDI, OUTPUT);
  pinMode(PIN_TDO, INPUT);
  pinMode(PIN_SRST, OUTPUT);
  WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
    READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG)
  | USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
  // esp_rom_gpio_connect_out_signal(GPIO, IOMATRIX, false, false);
  esp_rom_gpio_connect_out_signal(PIN_TCK,   OMATRIX_TCK,  false, false);
  esp_rom_gpio_connect_out_signal(PIN_TMS,   OMATRIX_TMS,  false, false);
  esp_rom_gpio_connect_out_signal(PIN_TDI,   OMATRIX_TDI,  false, false);
  esp_rom_gpio_connect_out_signal(PIN_SRST,  OMATRIX_SRST, false, false);
  esp_rom_gpio_connect_in_signal (PIN_TDO,   IMATRIX_TDO,  false);
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
  digitalWrite(LED_BUILTIN, LED_OFF);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF);
  // usb-serial
  Serial.begin(); // usb-serial
  Serial1.begin(BAUD, SERIAL_8N1, RXD, TXD); // hardware serial
  route_usb_jtag_to_gpio();
  // unroute_usb_jtag_to_gpio();
}

void loop() {
  uint8_t usb_rx;
  // digitalWrite(LED_BUILTIN, (((micros()>>16) & 15)==0)^LED_OFF ); // blink
  // digitalWrite(LED_BUILTIN, digitalRead(0)^LED_OFF); // test input pin
  if(Serial.available())
  {
    usb_rx = Serial.read();
    Serial1.write(usb_rx);
    if(usb_rx == '0') // serial monitor send 0 to disable jtag
      unroute_usb_jtag_to_gpio();
    if(usb_rx == '1') // serial monitor send 1 to enable jtag
      route_usb_jtag_to_gpio();
  }
  if(Serial1.available())
    Serial.write(Serial1.read());
}
