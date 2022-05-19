/*
********************************************************************************
*    Filename:   accel_lis2dh.h
*  Created on:   2022.05.12
*      Author:   Сергей П.
* Description:   работа с акселерометром LIS2DH
********************************************************************************
*/
#ifndef INC_ACCEL_LIS2DH_H_
#define INC_ACCEL_LIS2DH_H_

uint16_t  AccSetSlSt (void);      /*конфигурирует акселерометр LIS2DH для состояния SLEEP_STATE*/
uint16_t  AccSetWkSt (void);      /*конфигурирует акселерометр LIS2DH для состояния WORK_STATE*/
uint16_t  AccRdAngle (void);      /*получает данные с акселерометра, возвращает угол в градусах [0-360]*/

#endif /* INC_ACCEL_LIS2DH_H_ */
/*end file*/


