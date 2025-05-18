#ifndef __MMA845X_H__
#define __MMA845X_H__

#include "main.h"

#define MMA845X_ADDR        (0x1D << 1)  // SA0 = HIGH
#define REG_WHO_AM_I        0x0D
#define REG_CTRL_REG1       0x2A
#define REG_OUT_X_MSB       0x01

HAL_StatusTypeDef MMA845x_Init(void);
HAL_StatusTypeDef MMA845x_ReadXYZ(int16_t *x, int16_t *y, int16_t *z);

#endif
