
#include "stm32f10x.h"

#include "can.h"
#include "led_pwm.h"

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_config(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    CAN_InitTypeDef         CAN_InitStructure;
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;

    // GPIO clock enable 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // Configure CAN pin: RX 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure CAN pin: TX 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // CANx Periph clock enable 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    // CAN register init 
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    // CAN cell init 
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    //CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;

    /* CAN Baudrate = 1MBps*/
    //  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    //  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    //  CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    //  CAN_InitStructure.CAN_Prescaler = 4;
    //  CAN_Init(CAN1, &CAN_InitStructure);

    // Baudrate = 125kbps
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1=CAN_BS1_2tq;
    CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler=48;
    CAN_Init(CAN1, &CAN_InitStructure);

    // CAN filter init 
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);


    // Enable the CAN RX Interrupt
    NVIC_InitTypeDef NVIC_InitStructure_CAN;
    NVIC_InitStructure_CAN.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure_CAN.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure_CAN.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure_CAN.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure_CAN);

    //  Enable CAN Interrupt
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}


void prozess_can_it(void)
{
    CanRxMsg RxMessage;
    /*RxMessage.StdId=0x00;
    RxMessage.IDE=CAN_Id_Standard; // STD -> 11bit ID | EXT -> 11+18Bit ID
    //Remote transmission request
    RxMessage.RTR=0;
    //Data length code
    RxMessage.DLC=0;
    RxMessage.Data[0]=0x00;
    RxMessage.Data[1]=0x00;*/
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

    //nicht von mir
    if( RxMessage.StdId != CAN_ID )
    {
        LED_Toggle(1);
        // data[0] == led
        // data[1] == mode
        // ...
        int id = RxMessage.Data[0];
        if( 0 <= id && id >= 3 ) //um welche led geht es
        {
            leds[id].mode = RxMessage.Data[1];
            if( leds[id].mode == 1 ) //master - slave mode
                leds[id].master = RxMessage.Data[2];
            else if( leds[id].mode == 2)
            {
                leds[id].r = RxMessage.Data[2];
                leds[id].g = RxMessage.Data[3];
                leds[id].b = RxMessage.Data[4];
            }
            else if( leds[id].mode == 3) //fade to color
            {
                leds[id].std_time = (RxMessage.Data[2]<<8)+RxMessage.Data[3];
                leds[id].target_r = RxMessage.Data[4];
                leds[id].target_g = RxMessage.Data[5];
                leds[id].target_b = RxMessage.Data[6];
            }
            else if( leds[id].mode == 4) // auto-rnd mode
            {
                leds[id].std_time = (RxMessage.Data[2]<<8)+RxMessage.Data[3];
                //startwert setzen da sonst nicht anfängt zu faden
                led.change_r = (float)((rand()% 5+1))/led.std_time;
                led.change_g = (float)((rand()% 5+1))/led.std_time;
                led.change_b = (float)((rand()% 5+1))/led.std_time;
            }
            else if( leds[id].mode == 5 || leds[id].mode == 6 )
            {
                leds[id].std_time = (RxMessage.Data[2]<<8)+RxMessage.Data[3];
            }
        }
    }
}

    /// TODO add USB send (when buffer overflow is fixed)
    /*memcpy(send_string+0 , "Can: ", 5);
    memcpy(send_string+4 , &RxMessage.StdId, 1);
    memcpy(send_string+5 , ",", 1);
    memcpy(send_string+6 , &RxMessage.Data[0], 1);
    memcpy(send_string+7 , ",", 1);
    memcpy(send_string+8 , &RxMessage.Data[1], 1);
    memcpy(send_string+9 , "\n",1);
    send_string[10] = 0x0;
    VCP_DataTx(send_string, 10);*/


