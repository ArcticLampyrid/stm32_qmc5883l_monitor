## QMC5883L Monitor
A demo to monitor QMC5883L's data.

## BOM
- [STM32F401RCT6](https://www.st.com/en/microcontrollers-microprocessors/stm32f401rc.html)
- QMC5883L
- OLED Screen (SPI, SSD1306, 128x64)

## Pin Configuration
| Device                        | Pin                                                           |
| ----------------------------- | ------------------------------------------------------------- |
| Board                         | LED --> C13                                                   |
| QMC5883L                      | SCL --> B6, SDA --> B7                                        |
| Screen (SPI, SSD1306, 128x64) | SCK --> A5, MOSI --> A7, CS --> A8, RESET --> A11, DC --> A12 |
| UART                          | Host's RX --> A9 --> MCU's TX, Host's TX --> A10 --> MCU's RX |
