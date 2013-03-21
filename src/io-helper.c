
#include "io-helper.h"


/**
  * @brief  Configures Button NVIC and EXTI Line.
  * @retval None
  */
void button_interrupt_init(void)
{
    // *** Button1 ***
    //EXTI structure to init EXT
    EXTI_InitTypeDef EXTI_InitStructure;
    //Connect EXTI Line to Button Pin
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); //PortA.0 --> Btn1
    //Configure Button EXTI line
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    //select interrupt mode
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //generate interrupt on rising edge
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    //enable EXTI line
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //send values to registers
    EXTI_Init(&EXTI_InitStructure);

    //configure NVIC
    //NVIC structure to set up NVIC controller
    NVIC_InitTypeDef NVIC_InitStructure;
    //select NVIC channel to configure
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //set priority to lowest
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    //set subpriority to lowest
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    //enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //update NVIC registers
    NVIC_Init(&NVIC_InitStructure);


    // *** Pin für Licht-Relay configurieren ****
//    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Enable the Licht-Relay-Bin Clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* Configure the Licht-Relay-Bin */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configures Button GPIO
  * @retval None
  */
void button_init(void)
{
    //Button1
    GPIO_InitTypeDef GPIO_InitStructure_Button1;

    /* Enable the BUTTON Clock */
    RCC_APB2PeriphClockCmd(BUTTON_1_CLK | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure Button pin as input floating */
    GPIO_InitStructure_Button1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure_Button1.GPIO_Pin = BUTTON_1_PIN;
    GPIO_Init( BUTTON_1_PORT , &GPIO_InitStructure_Button1);

    //enables interrupts
    button_interrupt_init();
}


/**
  * @brief  Returns the selected Button state.
  * @retval The Button GPIO pin value.
  */
uint32_t Button_GetState(uint32_t btnnr)
{
    if(btnnr == 1)
        return GPIO_ReadInputDataBit(BUTTON_1_PORT, BUTTON_1_PIN); //pullup
    return 0;
}



/**
  * @brief  Configures LED GPIO.
  * @retval None
  */
void LED_init(void)
{
    //** LED1 **
    GPIO_InitTypeDef  GPIO_InitStructure_Led1;

    /* Enable the GPIO_LED Clock */
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK, ENABLE);

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure_Led1.GPIO_Pin = LED1_PIN;
    GPIO_InitStructure_Led1.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure_Led1.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure_Led1);

}


/**
  * @brief  Turns LEDx on.
  * @retval None
  */
void LED_On(uint32_t lednr)
{
    if(lednr == 1)
        LED1_GPIO_PORT->BRR = LED1_PIN;
}


/**
  * @brief  Turns LEDx on.
  * @retval None
  */
void LED_Off(uint32_t lednr)
{
    if(lednr == 1)
        LED1_GPIO_PORT->BSRR = LED1_PIN;
}


/**
  * @brief  Turns LEDx on.
  * @retval None
  */
void LED_Toggle(uint32_t lednr)
{
    if(lednr == 1)
        LED1_GPIO_PORT->ODR ^= LED1_PIN;
}

