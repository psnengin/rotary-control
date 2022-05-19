/*
********************************************************************************
*    Filename:   i2с.h
*  Created on:   2022.05.12
*      Author:   Сергей П.
* Description:   чтение / запись по i2c
********************************************************************************
*/
#ifndef INC_I2C_H_
#define INC_I2C_H_

#define LIS2DH_ADDR        (0x19U)    /*адрес датчика на шине I2C*/
#define LIS2DH_OUT_X_L     (0x28U)    /*адрес регистра начала чтения*/

/*******************************************************************************
*  Структура содержит параметры для настройки последовательного чтения
*  нескольких байт по I2C. Входной параметр для функции
*  uint16_t  I2cRdDma (struct I2cBufStruct readParam)
*******************************************************************************/
struct 	I2cBufStruct
{
    uint8_t 	slaveAddress;             /*адрес опрашиваемого slave устройства на шине I2C, <=127*/
    uint8_t 	startRegisterAdress;      /*[0-6]бит адрес регистра с которого начинается опрос, [7]бит=1 разрешает автоинкремент адреса*/
    uint8_t 	numberBytes;              /*количество последовательно считываемых байт*/
    uint32_t 	*buffer;                  /*ссылка на массив размерностью numberBytes*/
};

uint16_t  I2cRdByte (uint8_t slave_addr, uint8_t read_addr, uint8_t *rd_val); /*чтение мастером  одного байта по шине I2C*/
uint16_t  I2cWrByte (uint8_t slave_addr, uint8_t wr_addr, uint8_t wr_data);   /*запись мастером  одного байта по шине I2C*/
uint16_t  I2cRdDma (struct I2cBufStruct readParam);                           /*чтение мастером по шине I2C с использованием DMA*/
uint16_t  I2cWrVrByte (uint8_t slave_addr, uint8_t wr_addr, uint8_t wr_data); /*запись мастером одного байта по шине I2C с проверкой*/
void  I2cOff (void);                                                          /*отключение  i2c и DMA, используется в обработчике ошибок*/

#if (LIS2DH_ADDR != 0x19U) && (LIS2DH_ADDR != 0x18U)
	#error "error: LIS2DH_ADDR  mast be  0x18 or 0x19"
#endif
#if LIS2DH_OUT_X_L != 0x28U
#error "error: LIS2DH_OUT_X_L  mast be  0x28"
#endif

#endif /* INC_I2C_H_ */
/*end file*/
