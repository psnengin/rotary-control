/*
********************************************************************************
*    Filename:   i2с.c
*  Created on:   2022.05.12
*      Author:   Сергей П.
* Description:   чтение / запись по i2c
********************************************************************************
*/

#include <stm32f030x6.h>
#include "error.h"
#include "i2c.h"

#define I2C_TIMEOUT_DMA_US       (20000U)                /*таймаут в микросекундах  20ms*/
#define I2C_TIMEOUT_DMA_US_TICK  (I2C_TIMEOUT_DMA_US/10) /*таймаут в тиках таймера*/

#define I2C_TIMEOUT_US           (20000U)                /*таймаут в микросекундах  20ms*/
#define I2C_TIMEOUT_US_TICK      (I2C_TIMEOUT_US/10)	 /*таймаут в тиках таймера*/

uint8_t I2cTimout (uint16_t start_cnt, uint16_t timeout);

/*
********************************************************************************
* Функция чтения мастером по шине I2C одного байта
* ******************************************************************************
* param[in]   slave_addr  : адрес датчика на шине.
* param[in]   read_addr   : адрес внутреннего регистра для чтения.
* param[in]   *rd_val     : указатель на переменную для считанного байта.
*
* [out]       возвращает код ошибки или 0, если ошибок нет.
* ******************************************************************************
*/
uint16_t I2cRdByte (uint8_t slave_addr, uint8_t read_addr, uint8_t *rd_val)
{
    uint16_t err	      = 0;      /*возвращаемый код ошибки*/
    uint16_t start_timout = 0;      /*хранит начальное значение таймера для таймаута*/


    I2C1->CR1  &= 0x0000;                                          /*сброс настроек*/
    I2C1->CR2  &= 0x0000;                                          /*сброс настроек*/
    I2C1->RXDR &= 0x0000;                                          /*очистить регистр приема*/

    I2C1->CR1  |= I2C_CR1_PE;                                      /*разрешить I2C*/

    I2C1->CR2 	|= (slave_addr << 1)                               /*задать адрес slave на шине I2C*/
                |  0x1 << I2C_CR2_NBYTES_Pos                       /*передать один байт - адрес для чтения*/
                |  I2C_CR2_AUTOEND;                                /*автостоп по окончании*/

    I2C1->TXDR   = read_addr;                                      /*подготовить адрес регистра*/

    I2C1->CR2   |= I2C_CR2_START;                                  /*начать передачу*/

    start_timout = TIM14->CNT;                                     /*сохранить начало отсчета таймаута*/
    while ((I2C1->ISR & I2C_ISR_STOPF) != I2C_ISR_STOPF)           /*ждать stop бит*/
       {
           if(I2cTimout (start_timout, I2C_TIMEOUT_US_TICK) !=0)   /*таймаут*/
           {
               err = ERR_I2C_RD_BYTE_TOUT_STOP;                    /*выход с кодом ошибки*/
               return err;
           }
       }

    I2C1->ICR |= I2C_ICR_STOPCF;                                   /*очистить флаг*/
    I2C1->CR2 &= 0x0000;                                           /*сброс настроек*/

    I2C1->CR2 |= I2C_CR2_RD_WRN                                    /*мастер чтение*/
              | (slave_addr << 1)                                  /*загрузить адрес датчика*/
              | 0x1 << I2C_CR2_NBYTES_Pos                          /*принять один байт*/
              | I2C_CR2_AUTOEND;                                   /*автостоп по окончании*/

    I2C1->CR2 |=I2C_CR2_START;                                     /*начать обмен*/

    start_timout = TIM14->CNT;                                     /*сохранить начало отсчета таймаута*/
    while ((I2C1->ISR & I2C_ISR_STOPF) != I2C_ISR_STOPF)           /*ждать stop бит*/
        {
            if(I2cTimout (start_timout, I2C_TIMEOUT_US_TICK) !=0)  /*таймаут*/
            {
                err = ERR_I2C_RD_BYTE_TOUT_STOP;                   /*выход с кодом ошибки*/
                return err;
            }
        }

    I2C1->ICR |= I2C_ICR_STOPCF;                                   /*очистить флаг*/
    I2C1->CR1 &= ~I2C_CR1_PE;                                      /*запретить  I2C*/
    I2C1->CR2 &= 0x0000;                                           /*очистить CR2*/

    *rd_val = I2C1->RXDR;                                          /*передать считанное значение*/
    I2C1->RXDR &= 0x0000;                                          /*очистить регистр приема*/

    return err;                                                    /*вернуть код ошибки*/
}
/*
********************************************************************************
* Функция записи мастером по шине I2C одного байта
* ******************************************************************************
* param[in]   slave_addr : адрес датчика на шине
* param[in]   wr_addr : адрес внутреннего регистра для записи.
* param[in]   wr_data : данные для записи.
*
* [out]       возвращает код ошибки или 0, если ошибок не обнаружено.
* ******************************************************************************
*/
uint16_t I2cWrByte (uint8_t slave_addr, uint8_t wr_addr, uint8_t wr_data)
{

    uint16_t err	      = 0;      /*возвращаемый код ошибки*/
    uint16_t start_timout = 0;      /*хранит начальное значение таймера для таймаута*/


    I2C1->CR1  &= 0x0000;                                      /*сброс настроек*/
    I2C1->CR2  &= 0x0000;                                      /*сброс настроек*/
    I2C1->CR1  |= I2C_CR1_PE;                                  /*разрешить I2C*/


    I2C1->CR2 	|= (slave_addr << 1)                           /*задать адрес slave на шине I2C*/
                |  0x2 << I2C_CR2_NBYTES_Pos                   /*передать два байта*/
                |  I2C_CR2_AUTOEND;                            /*автостоп включить*/
    I2C1->TXDR  = wr_addr;                                     /*подготовить адрес регистра LIS2DH*/

    I2C1->CR2  |=I2C_CR2_START;                                /*начать передачу*/

    start_timout = TIM14->CNT;                                 /*сохранить начало отсчета таймаута*/
    while ((I2C1->ISR&I2C_ISR_TXIS) != I2C_ISR_TXIS)           /*ждать освобождения TXDR*/
    {
        if(I2cTimout (start_timout, I2C_TIMEOUT_US_TICK) !=0)  /*таймаут*/
        {
            err = ERR_I2C_WR_BYTE_TOUT_TXIS;                   /*выход с кодом ошибки*/
            return err;
        }
    }
    I2C1->TXDR = wr_data;                                      /*загрузить данные*/

    start_timout = TIM14->CNT;                                 /*сохранить начало отсчета таймаута*/
    while ((I2C1->ISR&I2C_ISR_STOPF)!=I2C_ISR_STOPF)           /*ждать stop бит*/
    {
        if(I2cTimout (start_timout, I2C_TIMEOUT_US_TICK) !=0)  /*таймаут*/
        {
            err = ERR_I2C_WR_BYTE_TOUT_STOP;                   /*выход с кодом ошибки*/
            return err;
        }
    }

    I2C1->ICR |= I2C_ICR_STOPCF;                               /*очистить флаг*/
    I2C1->CR1 &= ~I2C_CR1_PE;                                  /*запретить  I2C*/
    I2C1->CR2 &= 0x0000;                                       /*очистить CR2*/

    return 0;
}

/*
********************************************************************************
* Функция чтения мастером по шине I2C с использованием DMA.
* ******************************************************************************
* param[in]  readParam :  структура параметров обмена содержит адрес датчика на
*                        шине, внутренний адрес датчика, количество считываемых
*                        байт, указатель на массив для приема данных. см. i2c.h
* [out]                  возвращает код ошибки или 0, если ошибок не обнаружено.
* ******************************************************************************
* Справочник:	RM0091 стр647, AN2824 стр10, ES0219 (p6)
********************************************************************************
*/
uint16_t  I2cRdDma (struct I2cBufStruct readParam)
{
    uint16_t err	      = 0;        /*возвращаемый код ошибки*/
    uint16_t start_timout = 0;	      /*хранит начальное значение таймера для таймаута*/

	                                                                        /*настройка DMA*/
    DMA1_Channel3->CCR   = 0UL;
    DMA1_Channel3->CPAR  = (uint32_t) (&(I2C1->RXDR));                      /*Адрес регистра чтения I2C*/
    DMA1_Channel3->CMAR  = (uint32_t) (readParam.buffer);                   /*Адрес массива для сохранения данных из I2C*/
    DMA1_Channel3->CNDTR = readParam.numberBytes;                           /*размер приемного буфера*/
    DMA1_Channel3->CCR 	|= DMA_CCR_MINC | DMA_CCR_PSIZE_1                   /*разрешить инкремент адреса в приемном массиве,*/
                        | DMA_CCR_MSIZE_1;                                  /*размерность входных и выходных данных DMA только 32бита см.ES0219 (p6)*/

                                                                            /*настройка I2C*/
    I2C1->CR1 	&= 0x0000;                                                  /*сброс настроек I2C*/
    I2C1->CR2 	&= 0x0000;
    I2C1->RXDR 	&= 0x0000;
    I2C1->TXDR 	&= 0x0000;
    I2C1->CR1   |= I2C_CR1_RXDMAEN;                                         /*разрешить передачу через DMA*/
    I2C1->CR1   |= I2C_CR1_PE;                                              /*разрешить I2C*/
    I2C1->CR2 	|= (readParam.slaveAddress<<1)                              /*задать адрес slave на шине I2C*/
                |  1<<I2C_CR2_NBYTES_Pos                                    /*передать один байт - адрес регистра*/
                |  I2C_CR2_AUTOEND;                                         /*автостоп по окончании*/
    I2C1->TXDR   = readParam.startRegisterAdress;                           /*загрузить адрес регистра  датчика c которого начинаем чтение*/

    I2C1->CR2   |= I2C_CR2_START;                                           /*начать передачу*/

    start_timout  = TIM14->CNT;                                             /*старт ожидания используется в расчете таймаута*/
    while ((I2C1->ISR&I2C_ISR_STOPF) != I2C_ISR_STOPF)                      /*ждать пока стоп или таймаут произойдет*/
    {
        if(I2cTimout (start_timout, I2C_TIMEOUT_US_TICK) != 0)              /*таймаут*/
        {
            err = ERR_I2C_RD_DMA_TOUT_STOP;                                 /*выход с кодом ошибки*/
            return err;
        }
    }

    I2C1->ICR          |= I2C_ICR_STOPCF;                                   /*очистить флаг стоп*/
    I2C1->CR2          &= 0x0000;                                           /*очистить CR2*/
    I2C1->TXDR         &= 0x0000;
    DMA1_Channel3->CCR |= DMA_CCR_EN;                                       /*разрешить канал DMA*/
    I2C1->CR2          |= (readParam.slaveAddress << 1)                     /*задать адрес slave на шине I2C*/
                       |  I2C_CR2_RD_WRN	                                /*режим передачи чтение*/
                       |  (readParam.numberBytes << I2C_CR2_NBYTES_Pos);    /*задать количество передаваемых байт*/
                                                                            /*стоп бит не формировать, NACK автоматически*/

                                                                            /*прием данных*/
    I2C1->CR2   |= I2C_CR2_START;                                           /*рестарт, начать передачу*/

    start_timout = TIM14->CNT;                                              /*сохранить значение таймера в начале отсчета таймаута*/
    while ((DMA1->ISR & DMA_ISR_TCIF3) != DMA_ISR_TCIF3)                    /*ждать пока DMA закончит чтение*/
    {
        if(I2cTimout (start_timout, I2C_TIMEOUT_DMA_US_TICK) != 0)          /*таймаут*/
        {
            err = ERR_I2C_RD_DMA_TOUT_DMA_TCIF3;                            /*выход с кодом ошибки*/
            return err;
        }
    }

    DMA1_Channel3->CCR &= ~DMA_CCR_EN;                                      /*запретить канал DMA*/
    DMA1->IFCR         |= DMA_IFCR_CTCIF3 | DMA_IFCR_CGIF3                  /*сбросить флаги событий DMA*/
                       |  DMA_IFCR_CHTIF3 | DMA_IFCR_CTEIF3;

    I2C1->CR2  |=I2C_CR2_STOP;                                              /*формировать стоп бит*/

    start_timout = TIM14->CNT;                                              /*сохранить значение таймера*/
    while ((I2C1->ISR&I2C_ISR_STOPF) != I2C_ISR_STOPF)                      /*ждать пока стоп произойдет*/
    {
        if(I2cTimout (start_timout, I2C_TIMEOUT_US_TICK)!=0)                /*таймаут*/
        {
            err = ERR_I2C_RD_DMA_TOUT_STOP;                                 /*выход с кодом ошибки*/
            return err;    
        }
    }

    I2C1->ICR  |= I2C_ICR_STOPCF;                                           /*очистить флаг стоп*/
    I2C1->CR1  &=0x0000;                                                    /*запретить  I2C, запретить передачу через DMA*/
    I2C1->CR2  &=0x0000;                                                    /*очистить регистры I2C*/
    I2C1->RXDR &=0x0000;
    I2C1->TXDR &=0x0000;

    return err;
}

/*
********************************************************************************
* Функция записи мастером по шине I2C одного байта с проверкой
* ******************************************************************************
* param[in]   slave_addr : адрес датчика на шине
* param[in]   wr_addr : адрес внутреннего регистра для записи.
* param[in]   wr_data : данные для записи.
* [out]       возвращает код ошибки или 0, если ошибок не обнаружено.
* ******************************************************************************
*/
uint16_t I2cWrVrByte (uint8_t slave_addr, uint8_t wr_addr, uint8_t wr_data)
{
    uint16_t err    = 0;             /*возвращаемый код ошибки*/
    uint8_t rd_byte = 0;             /*контрольное значение*/

    err = I2cWrByte(slave_addr, wr_addr, wr_data);                   /*записать байт в регистр*/
    if (err != 0) {return err;}                                      /*во время записи возникла ошибка*/
    err = I2cRdByte(slave_addr, wr_addr, &rd_byte);                  /*прочитать регистр байт*/
    if (err != 0) {return err;}                                      /*во время чтения возникла ошибка*/
    if  (wr_data != rd_byte) {return I2C_WR_VR_BYTE_VERIFY_ERROR;}   /*данные не совпадают*/
    return err;
}
/*
********************************************************************************
* Функция таймаута. Отслеживает окончание заданного интервала. Использует  тай-
* мер TIM14. Учитывает перезагрузку счетчика.
* ******************************************************************************
* param[in]  start_cnt : значение  счетчика TIM14 в момент начала отсчета
* param[in]  timeout   : значение таймаута в тиках счетчика TIM14
* [out]      1 если интервал таймаута истек, 0 если нет.
* ******************************************************************************
*/
uint8_t I2cTimout (uint16_t start_cnt, uint16_t timeout)
{
    uint32_t end_cnt     = 0;          /*конечное значение счетчика*/
    uint32_t carrent_cnt = 0;          /*текущее значение счетчика*/
    uint32_t reload      = 0;          /*значение перезагрузки счетчика*/


    reload      = TIM14->ARR;                                           /*значение перезагрузки счетчика*/
    carrent_cnt = TIM14->CNT;                                           /*текущее значение счетчика*/
    end_cnt     = (uint32_t) start_cnt + timeout;                       /*конечное значение без учета переполнения*/

    if 	((end_cnt < reload) && (carrent_cnt >= end_cnt))                /*если переполнения нет и счетчик вышел за верхнюю границу*/
    {
        return 1;                                                       /*значит таймаут*/
    }
    if((!(end_cnt < reload)) && (carrent_cnt < start_cnt)               /*если переполнение есть и счетчик меньше стартового значения*/
                             && (carrent_cnt >= (end_cnt - reload)))    /*и счетчик досчитал до конечного значения*/
    {
        return 1;                                                       /*значит таймаут*/
    }
    return 0;                                                           /*еще не досчитал*/
}
/*
********************************************************************************
* Функция отключения  i2c и DMA, используется в обработчике ошибок
* ******************************************************************************
*/
void  I2cOff (void)
{
	 I2C1->CR1          &= 0x0000;                                 /*запретить I2C*/
	 I2C1->CR2          &= 0x0000;
	 DMA1_Channel3->CCR &= ~DMA_CCR_EN;                            /*запретить канал DMA*/
	 DMA1->IFCR         |= DMA_IFCR_CTCIF3 | DMA_IFCR_CGIF3        /*сбросить флаги событий DMA*/
	                    |  DMA_IFCR_CHTIF3 | DMA_IFCR_CTEIF3;
}

/*проверка правильности значений таймаутов*/
#if (I2C_TIMEOUT_US_TICK > 5000)
#error "error: I2C_TIMEOUT_US_TICK > 5000"
#endif

#if (I2C_TIMEOUT_DMA_US_TICK > 5000)
#error "error: 2C_TIMEOUT_DMA_US_TICK > 5000"
#endif

/*end file*/
