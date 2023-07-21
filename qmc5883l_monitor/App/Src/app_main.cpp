#include "../Inc/app_main.h"
#include "i2c.h"
#include "oled.h"
#include "qmc5883l.h"
#include "uexecuter.h"
#include "uexecuter_service.h"
#include "uexecuter_transport_stm32.h"
#include <cmath>
#include <cstdio>
#include <cstring>

static uexecuter_transport_stm32_t uexecuter_transport;
static uexecuter_t uexecuter;
static qmc5883l_t qmc5883l(&hi2c1);
static volatile qmc5883l_data_t qmc5883l_data;

static double get_mag_x_raw()
{
    return qmc5883l_data.mag_x_raw;
}
static double get_mag_y_raw()
{
    return qmc5883l_data.mag_y_raw;
}
static double get_mag_z_raw()
{
    return qmc5883l_data.mag_z_raw;
}

UEXECUTER_DEFINE_SERVICE(my_uexecuter_service){
    UEXECUTER_FUNCTION_PROTOTYPE_AUTO(get_mag_x_raw, "get_mag_x_raw"),
    UEXECUTER_FUNCTION_PROTOTYPE_AUTO(get_mag_y_raw, "get_mag_y_raw"),
    UEXECUTER_FUNCTION_PROTOTYPE_AUTO(get_mag_z_raw, "get_mag_z_raw"),
};

extern "C" void app_pre_init()
{
    // do nothing
}
extern "C" void app_init()
{
    // do nothing
}
extern "C" void app_sys_init()
{
    // do nothing
}

extern "C" void app_main()
{
    uexecuter_transport_stm32_init(&uexecuter_transport, USART1);
    uexecuter_init(&uexecuter, my_uexecuter_service, UEXECUTER_SERVICE_N_FUNC(my_uexecuter_service));
    uexecuter_bind(&uexecuter, &uexecuter_transport.base);
    uexecuter_transport_stm32_begin(&uexecuter_transport);
    oled_init();
    u8g2_ClearBuffer(&u8g2);
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x13_mf);
    bool qmc5883l_ok = qmc5883l.init();
    if (qmc5883l_ok)
    {
        HAL_Delay(100);
    }
    uint32_t screen_last_updated = 0;
    for (;;)
    {
        qmc5883l_data_t data = qmc5883l.read_data();
        memcpy((void *)&qmc5883l_data, &data, sizeof(qmc5883l_data_t));
        if (screen_last_updated - HAL_GetTick() > 50)
        {
            u8g2_ClearBuffer(&u8g2);
            if (qmc5883l_ok)
            {
                char str_buf[32];
                if (!data.overflow)
                {
                    sprintf(str_buf, "X: %d", data.mag_x_raw);
                    u8g2_DrawStr(&u8g2, 0, 16, str_buf);
                    sprintf(str_buf, "Y: %d", data.mag_y_raw);
                    u8g2_DrawStr(&u8g2, 0, 32, str_buf);
                    sprintf(str_buf, "Z: %d", data.mag_z_raw);
                    u8g2_DrawStr(&u8g2, 0, 48, str_buf);

                    auto azimuth = std::atan2(data.mag_y_raw, data.mag_x_raw);
                    if (azimuth < 0)
                    {
                        azimuth += 2 * M_PI;
                    }
                    sprintf(str_buf, "Azimuth: %.2f D", azimuth * 180 / M_PI);
                    u8g2_DrawStr(&u8g2, 0, 64, str_buf);
                }
                else
                {
                    u8g2_DrawStr(&u8g2, 0, 16, "Overflow");
                }
            }
            else
            {
                u8g2_DrawStr(&u8g2, 50, 16, "ERROR");
            }
            u8g2_SendBuffer(&u8g2);
            screen_last_updated = HAL_GetTick();
        }
        if (HAL_GetTick() % 200 < 100)
        {
            HAL_GPIO_WritePin(BoardLED_GPIO_Port, BoardLED_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(BoardLED_GPIO_Port, BoardLED_Pin, GPIO_PIN_SET);
        }
    }
}

extern "C" void usart1_irq()
{
    uexecuter_transport_stm32_handle_isr(&uexecuter_transport);
}