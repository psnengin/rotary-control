/*******************************************************************************
*    Filename:   start_init.c
*  Created on:   2022.05.12
*      Author:   Сергей П.
* Description:   стартовая инициализация периферии контроллера
 *******************************************************************************
*/

#include <stm32f030x6.h>
#include "init.h"

void  InitHsiRcClock (void);
void  InitTim14 (void);
void  InitGpio (void);
void  InitTim3 (void);
void  InitI2c (void);
void  InitWwdt (void);
void  InitPA0Irq (void);

/*
********************************************************************************
* Инициализация периферии контроллера
* ******************************************************************************
*/
void  InitStart (void)
{
    InitHsiRcClock();               /*Инициализация источника тактирования*/
    InitWwdt();                     /*Инициализация WD таймера*/
    InitTim14 ();                   /*Инициализация таймера TIM14*/
    InitGpio();                     /*конфигурация GPIO*/
    InitTim3();                     /*Инициализация таймера TIM3*/
    InitI2c();                      /*Инициализация шины I2C и канала DMA*/
    InitPA0Irq();                   /*инициализация прерывания PA0*/
}


/*
********************************************************************************
*   Инициализация источника тактирования
*   HSI 8 МГц, Делитель AHB=1, делитель APB=1. На периферию 8Мгц
** *****************************************************************************
*/
void  InitHsiRcClock (void)
{
    RCC -> CR |= RCC_CR_HSION;                                 /*разрешить HSI 8 Мгц*/ 
                                         
    while(((RCC->CR & RCC_CR_HSIRDY) == RCC_CR_HSIRDY) != 1);  /* ждать пока частота стабилизируется*/

    RCC->CFGR &= ~RCC_CFGR_HPRE;                               /*AHB делитель не используется*/
    RCC->CFGR &= ~RCC_CFGR_PPRE;                               /*APB делитель не используется*/
    RCC->CFGR &= ~RCC_CFGR_SW;	                               /*set system clock switch to HSI as system clock*/ 
                                                               
    while(((RCC->CFGR & RCC_CFGR_SWS_0) || (RCC->CFGR & RCC_CFGR_SWS_1)) == 1);  /*проверка флага SWS 00-HSI*/
}

/*
********************************************************************************
*   конфигурация таймера TIM14
** *****************************************************************************
*/
void  InitTim14 (void)
{
    RCC->APB1ENR|=RCC_APB1ENR_TIM14EN;    /*разрешить тактирование TIM14*/

    TIM14->PSC   = 79U;                   /*10us = (PSK+1)/FAPB = (79+1)/8MHz*/
    TIM14->ARR   = 10000U;                /*100ms прерывание*/
    TIM14->DIER |= TIM_DIER_UIE;          /*разрешили прерывание*/
    TIM14->CR1  |= TIM_CR1_CEN;

    NVIC_SetPriority(TIM14_IRQn, 21);
}
/*
********************************************************************************
*   конфигурация GPIO
** *****************************************************************************
*/
void InitGpio(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;               /*разрешить тактирование PORTA*/

/*PA6 выход ШИМ */
    GPIOA->MODER   &= ~GPIO_MODER_MODER6;            /*очистка битов выбора режима*/
    GPIOA->MODER   |= GPIO_MODER_MODER6_1;           /*GPIO PA6 альтернативная функция*/
    GPIOA->AFR[0]  |= 0x01<<GPIO_AFRL_AFSEL6_Pos;    /*PA6 альт.функция AF1 TIM3CH1*/
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT_6;             /*OTYPER0 = 0 - Output push-pull*/
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR6;        /*OSPEEDR[1:0] = 00 - Low speed*/
    GPIOA->PUPDR   &= ~GPIO_PUPDR_PUPDR6;            /*No pull-up, pull-down*/

/*PA9 SCL, PA10 SDA, I2C */
    GPIOA->MODER   &= ~GPIO_MODER_MODER10;           /*очистка битов выбора режима*/
    GPIOA->MODER   |= GPIO_MODER_MODER10_1;          /*альтернативная функция*/
    GPIOA->AFR[1]  |= 0x04<<GPIO_AFRH_AFSEL10_Pos;   /*AF4 I2C SDA (datasheet p34)*/
    GPIOA->OTYPER  |= GPIO_OTYPER_OT_10;             /*OTYPER0 = 1 - Открытый сток*/

    GPIOA->MODER   &= ~GPIO_MODER_MODER9;            /*очистка битов выбора режима*/
    GPIOA->MODER   |= GPIO_MODER_MODER9_1;           /*альтернативная функция*/
    GPIOA->AFR[1]  |= 0x04<<GPIO_AFRH_AFSEL9_Pos;    /*AF4 I2C SCL(datasheet p34)*/
    GPIOA->OTYPER  |= GPIO_OTYPER_OT_9;              /*OTYPER1 = 1 - Открытый сток*/

/*PA0 вход прерывания */
    GPIOA->MODER   &= ~GPIO_MODER_MODER0;            /* PA0 вход*/ 
    GPIOA->PUPDR   &= ~GPIO_PUPDR_PUPDR0;
    GPIOA->PUPDR   |= GPIO_PUPDR_PUPDR0_0;           /*pull-up*/

/*неиспользуемые выводы*/
    GPIOA->MODER   |= GPIO_MODER_MODER1              /*не используемые выводы*/
                   |  GPIO_MODER_MODER2              /*в аналоговый режим*/
                   |  GPIO_MODER_MODER3
                   |  GPIO_MODER_MODER4
                   |  GPIO_MODER_MODER5
                   |  GPIO_MODER_MODER7
                   |  GPIO_MODER_MODER8;

    GPIOF->MODER   |= GPIO_MODER_MODER0              /*не используемые выводы*/
                   |  GPIO_MODER_MODER1;             /*в аналоговый режим*/
}

/*
********************************************************************************
*  Инициализация таймера TIM 3 	CH1-PA6;	PWM активное состояние 1,
** *****************************************************************************
*/
void InitTim3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   /*разрешить тактирование TIM3*/

    TIM3->PSC     = 0;                    /*предварительный счетчик не используется*/
    TIM3->ARR     = 7225;                 /*f_ШИМ примерно 1кГц  7225 = 340*340/16 см. OutControl()*/

    TIM3->CCMR1  |= TIM_CCMR1_OC1M_1      /*выбрать PWM mode 1*//* CH1  активен пока  CLK<CCR1*/
                 |  TIM_CCMR1_OC1M_2
                 |  TIM_CCMR1_OC1PE;      /*разрешить preload регистр CCR1*/

    TIM3->CCER   |= TIM_CCER_CC1E;        /*сконфигурировать канал как выход*/
    TIM3->CCR1    = 0;                    /* значение ШИМ */

    TIM3->CR1    |= TIM_CR1_CEN;          /*запустить счетчик*/
}

/*
********************************************************************************
*  Инициализация шины I2C и канала DMA
** *****************************************************************************
*/
void  InitI2c (void)
{
    RCC->APB1ENR  |= RCC_APB1ENR_I2C1EN;    /*разрешть тактирование I2C1*/
    I2C1->TIMINGR  = 0x010420F13;           /*стр 650 рефер. ман. standart-mode 100 кГц*/

    RCC->AHBENR   |= RCC_AHBENR_DMA1EN;     /*разрешить тактирование DMA*/
}
/*
********************************************************************************
*  Инициализация WD таймера
** *****************************************************************************
*/
void  InitWwdt (void)
{   
#if 0                                                        /*раскомментировать в режиме отладки*/
    RCC->APB2ENR   |= RCC_APB2ENR_DBGMCUEN;
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_WWDG_STOP;
#endif

    RCC->APB1ENR  |= RCC_APB1ENR_WWDGEN;                     /*разрешть тактирование I2C1*/
    WWDG->CFR      = WWDG_REFRESH;                           /*максимальное окно*/
    WWDG->CFR     |= WWDG_CFR_WDGTB_1 | WWDG_CFR_WDGTB_0;    /*делитель 8,  тикWD = 4096*8*PCLK*, период 262ms*/

    WWDG->CR       = WWDG_REFRESH;                           /*взвести WD*/
    WWDG->CR      |= WWDG_CR_WDGA;                           /*разрешить WD*/
}

/*
********************************************************************************
* нициализация вывода PA0 как вход прерывания
* ******************************************************************************
 */
void  InitPA0Irq (void)
{
    EXTI->FTSR        |=EXTI_FTSR_TR0;             /*прерывание по заднему фронту*/
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR1_EXTI0;    /*EXTI1 порт A  PA0*/
    EXTI->IMR         |=EXTI_IMR_IM0 ;             /*разрешить запрос на прерывание для линии 0*/

    NVIC_SetPriority (EXTI0_1_IRQn, 20);
    NVIC_EnableIRQ(EXTI0_1_IRQn);                  /*разрешить прерывание*/
}

/*end file*/
