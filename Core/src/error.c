/*
********************************************************************************
*    Filename:   error.c
*  Created on:   2022.05.12
*      Author:   Сергей П.
* Description:   обработка ошибок
********************************************************************************
*/
#include <stm32f030x6.h>
#include "error.h"
#include "i2c.h"

/*
********************************************************************************
* Функция обработки ошибок. Пока только выключает I2C
* ******************************************************************************
* param[in]  error_code 
* ******************************************************************************
*/
void ErrHandler(uint16_t error_code)
{
    switch (error_code)
    {
        case ERR_I2C_RD_DMA_TOUT_TC:
             I2cOff ();
             break;

        case ERR_I2C_RD_DMA_TOUT_DMA_TCIF3:
             I2cOff ();
             break;

        case ERR_I2C_RD_DMA_TOUT_STOP:
             I2cOff ();
             break;

        case ERR_I2C_WR_BYTE_TOUT_TXIS:
             I2cOff ();
             break;

        case ERR_I2C_WR_BYTE_TOUT_STOP:
             I2cOff ();
             break;

        case ERR_I2C_RD_BYTE_TOUT_STOP:
             I2cOff ();
             break;

        case I2C_WR_VR_BYTE_VERIFY_ERROR:
             break;

        default:
             break;
        }
}

/*end file*/

