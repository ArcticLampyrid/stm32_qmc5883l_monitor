#include "oled.h"
#include "main.h"
#include "spi.h"

u8g2_t u8g2;

static uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
                                         U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        HAL_Delay(1);
        break;
    case U8X8_MSG_DELAY_MILLI:
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
        break;
    case U8X8_MSG_GPIO_RESET:
        HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, arg_int);
        break;
    default:
        break;
    }
    return 1;
}

static uint8_t u8x8_byte_4wire_hw_spi(U8X8_UNUSED u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        HAL_SPI_Transmit(&hspi1, (uint8_t *)arg_ptr, arg_int, 10000);
        break;
    case U8X8_MSG_BYTE_INIT:
        break;
    case U8X8_MSG_BYTE_SET_DC:
        HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);
        break;
    default:
        return 0;
    }
    return 1;
}

void oled_init()
{
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}
