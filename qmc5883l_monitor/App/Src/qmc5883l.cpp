#include "qmc5883l.h"
#include <cstring>

#define QMC5883L_MODE_CONTINUOUS (0x01)
#define QMC5883L_SOFT_RST (0x80)
#define QMC5883L_CONTROL_REGISTER_1 (0x09)
#define QMC5883L_CONTROL_REGISTER_2 (0x0A)
#define QMC5883L_CHIP_ID_REGISTER (0x0D)
#define QMC5883L_CHIP_ID (0xFF)
#define QMC5883L_ADDR (0x1A)

qmc5883l_t::qmc5883l_t(I2C_HandleTypeDef *I2Cx, uint32_t i2c_timeout) : I2Cx(I2Cx), i2c_timeout(i2c_timeout)
{
}

bool qmc5883l_t::init(qmc5883l_output_data_rate odr, qmc5883l_range range, qmc5883l_over_sample_ratio osr)
{
    uint8_t data;

    // Reset
    data = QMC5883L_SOFT_RST;
    if (HAL_I2C_Mem_Write(I2Cx, QMC5883L_ADDR, QMC5883L_CONTROL_REGISTER_2, I2C_MEMADD_SIZE_8BIT, &data, 1,
                          i2c_timeout) != HAL_OK)
    {
        return false;
    }
    HAL_Delay(50);

    // SET/RESET Period
    data = 1;
    if (HAL_I2C_Mem_Write(I2Cx, QMC5883L_ADDR, 0x0B, I2C_MEMADD_SIZE_8BIT, &data, 1, i2c_timeout) != HAL_OK)
    {
        return false;
    }

    data =
        static_cast<uint8_t>(odr) | static_cast<uint8_t>(range) | static_cast<uint8_t>(osr) | QMC5883L_MODE_CONTINUOUS;
    if (HAL_I2C_Mem_Write(I2Cx, QMC5883L_ADDR, QMC5883L_CONTROL_REGISTER_1, I2C_MEMADD_SIZE_8BIT, &data, 1,
                          i2c_timeout) != HAL_OK)
    {
        return false;
    }

    // Check
    if (HAL_I2C_Mem_Read(I2Cx, QMC5883L_ADDR, QMC5883L_CHIP_ID_REGISTER, I2C_MEMADD_SIZE_8BIT, &data, 1, i2c_timeout) !=
        HAL_OK)
    {
        return false;
    }
    if (data != QMC5883L_CHIP_ID)
    {
        return false;
    }

    return true;
}

qmc5883l_data_t qmc5883l_t::read_data()
{
    qmc5883l_data_t result;
    uint8_t Rec_Data[7];
    if (HAL_I2C_Mem_Read(I2Cx, QMC5883L_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, Rec_Data, 7, i2c_timeout) != HAL_OK)
    {
        memset(&result, 0, sizeof(result));
        return result;
    }
    result.mag_x_raw = static_cast<int16_t>(Rec_Data[1] << 8 | Rec_Data[0]);
    result.mag_y_raw = static_cast<int16_t>(Rec_Data[3] << 8 | Rec_Data[2]);
    result.mag_z_raw = static_cast<int16_t>(Rec_Data[5] << 8 | Rec_Data[4]);
    result.overflow = !!(Rec_Data[6] & 0x02);
    return result;
}