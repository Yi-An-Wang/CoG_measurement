#include "mma845x.h"

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef MMA845x_Init(void)
{
    uint8_t data;

    // 將感測器設為 Standby 模式才能改設定
    HAL_I2C_Mem_Read(&hi2c1, MMA845X_ADDR, REG_CTRL_REG1, 1, &data, 1, HAL_MAX_DELAY);
    data &= ~(0x01);  // 清除 ACTIVE bit
    HAL_I2C_Mem_Write(&hi2c1, MMA845X_ADDR, REG_CTRL_REG1, 1, &data, 1, HAL_MAX_DELAY);

    // 設定量測範圍與濾波器（略，可設定 0x0E 等）

    // 啟動感測器
    HAL_I2C_Mem_Read(&hi2c1, MMA845X_ADDR, REG_CTRL_REG1, 1, &data, 1, HAL_MAX_DELAY);
    data |= 0x01;  // 設定 ACTIVE bit
    return HAL_I2C_Mem_Write(&hi2c1, MMA845X_ADDR, REG_CTRL_REG1, 1, &data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA845x_ReadXYZ(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buffer[6];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, MMA845X_ADDR, REG_OUT_X_MSB, 1, buffer, 6, HAL_MAX_DELAY);
    if (status != HAL_OK) return status;

    *x = ((int16_t)(buffer[0] << 8 | buffer[1])) >> 4;
    *y = ((int16_t)(buffer[2] << 8 | buffer[3])) >> 4;
    *z = ((int16_t)(buffer[4] << 8 | buffer[5])) >> 4;
    return HAL_OK;
}
