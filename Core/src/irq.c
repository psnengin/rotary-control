/*******************************************************************************
*    Filename:   irq.c
*  Created on:   2022.05.12
*     Author:   Сергей П.
* Description:   обработчики прерываний
********************************************************************************
*/

#include <stm32f030x6.h>
#include "irq.h"
#include "accel_lis2dh.h"

uint8_t flag_pa0_irq  = 0;                      /*флаг прерывания от GPIO PA0*/

/*******************************************************************************
* Обработчик прерывания от GPIO PA0 (задний фронт). Сигнал прерывания формирует
* акселерометр LIS2DH в следующих случаях:
*   1. Акселерометр настроен функцией AccSetWkSt(). Прерывание формируется при
* поступлении в выходные регистры датчика новых данных.
*   2. Акселерометр настроен функцией AccSetSlSt(). Прерывание формируется при
* изменении положения датчика.
********************************************************************************
*/
void  EXTI0_1_IRQHandler (void)
{
    EXTI->PR     |= EXTI_PR_PR0;                           /*сброс прерывания*/
    flag_pa0_irq  = IRQ_PA0_FLAG_ON;                       /*флаг установить*/
   
}

/*******************************************************************************
* Обработчик прерывания от TIM14.
* Период t= 100ms.
* Активируется только в режиме SLEEP_STATE для перезагрузки WWDT.
********************************************************************************
*/
void TIM14_IRQHandler ()
{
    TIM14->SR &= ~TIM_SR_UIF;                               /*сброс прерывания*/

}

/*end file*/
