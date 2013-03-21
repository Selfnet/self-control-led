
#ifndef __IO_HELPER_H__
#define __IO_HELPER_H__

#include "stm32f10x.h"


#define UI32_DELAY_TIME 0x1FFFFF

#define BUTTON_1_PIN    GPIO_Pin_0
#define BUTTON_1_PORT   GPIOA
#define BUTTON_1_CLK    RCC_APB2Periph_GPIOA

#define KEY_PRESSED     0x00
#define KEY_NOT_PRESSED 0x01

//gruen
#define LED1_PIN        GPIO_Pin_12
#define LED1_GPIO_PORT  GPIOC
#define LED1_GPIO_CLK   RCC_APB2Periph_GPIOC

//Buttons
void button_init(void);
uint32_t Button_GetState(uint32_t);

// Can functions
void CAN_config(void);

// Led
void LED_init(void);
void LED_On(uint32_t lednr);
void LED_Off(uint32_t lednr);
void LED_Toggle(uint32_t lednr);

#endif
