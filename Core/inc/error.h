/*
********************************************************************************
*    Filename:   error.h
*  Created on:   2022.05.12
*      Author:   Сергей П.
* Description:   обработка ошибок
********************************************************************************
*/

#ifndef INC_ERROR_H_
#define INC_ERROR_H_

#define ERR_I2C_RD_DMA_TOUT_TC              (3U)	 /*ошибка таймаута i2c*/
#define ERR_I2C_RD_DMA_TOUT_DMA_TCIF3       (4U)	 /*ошибка таймаута i2c*/
#define ERR_I2C_RD_DMA_TOUT_STOP            (5U)	 /*ошибка таймаута i2c*/
#define ERR_I2C_WR_BYTE_TOUT_STOP           (7U)	 /*ошибка таймаута i2c*/
#define ERR_I2C_WR_BYTE_TOUT_TXIS           (8U)	 /*ошибка таймаута i2c*/
#define ERR_I2C_RD_BYTE_TOUT_STOP           (9U)	 /*ошибка таймаута i2c*/
#define I2C_WR_VR_BYTE_VERIFY_ERROR         (10U)        /*считан не тот байт, который записан при записи спроверкой*/

void  ErrHandler (uint16_t error_code);                  /* обработка ошибок.*/

#endif  /* INC_ERROR_H_ */
/*end file*/
