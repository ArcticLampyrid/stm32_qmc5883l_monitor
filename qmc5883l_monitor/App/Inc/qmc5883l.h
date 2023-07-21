#pragma once
#include "i2c.h"
#include <cstdint>

typedef struct
{
    int16_t mag_x_raw;
    int16_t mag_y_raw;
    int16_t mag_z_raw;
    bool overflow;
} qmc5883l_data_t;

enum class qmc5883l_output_data_rate : uint8_t
{
    odr_10Hz = 0 << 2,
    odr_50Hz = 1 << 2,
    odr_100Hz = 2 << 2,
    odr_200Hz = 3 << 2,
};

enum class qmc5883l_range : uint8_t
{
    fsr_2g = 0 << 4,
    fsr_8g = 1 << 4,
};

enum class qmc5883l_over_sample_ratio : uint8_t
{
    osr_512 = 0 << 6,
    osr_256 = 1 << 6,
    osr_128 = 2 << 6,
    osr_64 = 3 << 6,
};

class qmc5883l_t
{
  private:
    I2C_HandleTypeDef *I2Cx;
    uint32_t i2c_timeout;

  public:
    /**
     * Create an instance of QMC5883L driver.
     * @param I2Cx I2C bus
     * @param i2c_timeout timeout of I2C bus
     * @note The timeout is determined via HAL_GetTick. Remember to check the priority of SysTick interrupt.
     */
    explicit qmc5883l_t(I2C_HandleTypeDef *I2Cx, uint32_t i2c_timeout = 100);
    /**
     * Initialize the QMC5883L
     * @param odr Output data rate
     * @param range Full scale range
     * @param osr Over sample ratio
     * @return succeed or not
     */
    bool init(qmc5883l_output_data_rate odr = qmc5883l_output_data_rate::odr_10Hz,
              qmc5883l_range range = qmc5883l_range::fsr_2g,
              qmc5883l_over_sample_ratio osr = qmc5883l_over_sample_ratio::osr_512);
    /**
     * Read the data of QMC5883L.
     * @return The data
     */
    qmc5883l_data_t read_data();
};