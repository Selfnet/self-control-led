
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


/*
Field name	                        Length (bits)	Purpose
Start-of-frame	                    1	Denotes the start of frame transmission
Identifier A	                    11	First part of the (unique) identifier for the data which also represents the message priority
Substitute remote request (SRR)	    1	Must be recessive (1). Optional
Identifier extension bit (IDE)	    1	Must be recessive (1). Optional
Identifier B	                    18	Second part of the (unique) identifier for the data which also represents the message priority
Remote transmission request (RTR)	1	Must be dominant (0)
Reserved bits (r0, r1)	            2	Reserved bits (it must be set dominant (0), but accepted as either dominant or recessive)
Data length code (DLC)*	            4	Number of bytes of data (0–8 bytes)
Data field	                        0–64 (0-8 bytes)	Data to be transmitted (length dictated by DLC field)
CRC	                                15	Cyclic redundancy check
CRC delimiter	                    1	Must be recessive (1)
ACK slot	                        1	Transmitter sends recessive (1) and any receiver can assert a dominant (0)
ACK delimiter	                    1	Must be recessive (1)
End-of-frame (EOF)	                7	Must be recessive (1)
*/


// *** CAN Id Functions
int getSender(uint32_t ExtId)
{
    return (ExtId>>(11+7)) & 0x7ff;
}

int getRecipient(uint32_t ExtId)
{
    return (ExtId>>(7)) & 0x7ff;
}

int getTyp(uint32_t ExtId)
{
    return (ExtId & 0x7f);
}

void setSender(uint32_t *ExtId , int recipient)
{
    *ExtId |= (((uint32_t)recipient) & 0x7ff)<<(11+7);
}

void setRecipient(uint32_t *ExtId , int recipient)
{
    *ExtId |= (((uint32_t)recipient) & 0x7ff)<<7;
}

void setTyp(uint32_t *ExtId , int recipient)
{
    *ExtId |= (((uint32_t)recipient) & 0x7f);
}


// *** erklärung zu can vars ***
//Sender        = RxMessage.ExtId & 0b11111111111000000000000000000 (11Bit)
//Empfaenger    = RxMessage.ExtId & 0b00000000000111111111110000000 (11Bit)
//Type          = RxMessage.ExtId & 0b00000000000000000000001111111 (7Bit)
//ID-Type       = RxMessage.IDE (CAN_Id_Standard or CAN_Id_Extended) DEFAULT=1 (immer extended)
//get_set?      = RxMessage.RTR: (1-> SendData (seter) | 0-> Request Data (geter))

// ethernet bytes:
// SENDER0 | SENDER1 | EMPFAENGER0 | EMPFAENGER1 | TYPE | SEND-REQUEST | DATA0 - DATA7

void send_pong(CanRxMsg RxMessage)
{
    //ping request
    if(getTyp(RxMessage.ExtId) == 0x8 && RxMessage.RTR == CAN_RTR_Remote    )
    {
        CanTxMsg TxMessage;
        TxMessage.IDE = CAN_ID_EXT;                                 //immer extended can frames
        setRecipient(&TxMessage.ExtId , getSender(RxMessage.ExtId));//empfaenger = absender vom Ping
        setSender(&TxMessage.ExtId , NODE_CAN_ID);                  //sender (hoeherwaertige bits)
        //setTyp(&TxMessage.ExtId, 0);                              //type  = 0 (antwort auf ping) [eh schon 0]
        TxMessage.RTR = CAN_RTR_Data;                               // daten senden

        // alle empfangen daten zurueckschicken
        TxMessage.DLC = RxMessage.DLC;
        int i;
        for(i = 0 ; i < RxMessage.DLC ; i++)
        {
            TxMessage.Data[i] = RxMessage.Data[i];
        }
        CAN_Transmit(CAN1, &TxMessage);
    }
}

void process_can_msg(CanRxMsg RxMessage, int id)
{
    if(0 <= id && id <= 3)
    {
        leds[id].mode = getTyp(RxMessage.ExtId);

        if( leds[id].mode == 1 ) //master - slave mode
            leds[id].master = RxMessage.Data[0];
        else if( leds[id].mode == 2) //set color
        {
            leds[id].r = RxMessage.Data[0];
            leds[id].g = RxMessage.Data[1];
            leds[id].b = RxMessage.Data[2];
        }
        else if( leds[id].mode == 3) //fade to color
        {
            leds[id].std_time = (RxMessage.Data[0]<<8)+RxMessage.Data[1];
            leds[id].target_r = RxMessage.Data[2];
            leds[id].target_g = RxMessage.Data[3];
            leds[id].target_b = RxMessage.Data[4];
            leds[id].time = 0; //start fadeing
        }
        else if( leds[id].mode == 4) //set rnd color
        {
            leds[id].std_time = (RxMessage.Data[0]<<8)+RxMessage.Data[1];
        }
        else if( leds[id].mode == 5 ) // auto fade rnd mode
        {
            leds[id].std_time = (RxMessage.Data[0]<<8)+RxMessage.Data[1];
            //startwert setzen da sonst nicht anfängt zu faden
            led.change_r = (float)((rand()% 5+1))/led.std_time;
            led.change_g = (float)((rand()% 5+1))/led.std_time;
            led.change_b = (float)((rand()% 5+1))/led.std_time;
        }
        else if( leds[id].mode == 6 ) //strobe
        {
            leds[id].change_r = (RxMessage.Data[0]<<8)+RxMessage.Data[1];
            leds[id].std_time = (RxMessage.Data[2]<<8)+RxMessage.Data[3];
        }
        else if( leds[id].mode == 7 ) //circle
        {
            leds[id].std_time = (RxMessage.Data[0]<<8)+RxMessage.Data[1];
        }
        else if( leds[id].mode == 9 ) //setHSV color
        {
            //convert hsv to rgb and set rgb values to led
            HSV2RGB( &leds[id] , (float)((RxMessage.Data[0]<<8)+(RxMessage.Data[1])), ((float)RxMessage.Data[2]), ((float)RxMessage.Data[5]));
        }
        else if( leds[id].mode == 10 ) // fade to master (start when master is ready)
        {
            leds[id].master = RxMessage.Data[0];
            leds[id].std_time = (RxMessage.Data[1]<<8)+RxMessage.Data[2];
            leds[id].time = 0;
        }
    }
}

void prozess_can_it(void)
{
    CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

    if(RxMessage.IDE == CAN_Id_Standard)
    {
        //einfach ignorieren, vllt spaeter auch noch was machen...
    }
    else //das was wir wollen , Extended CanRxMsg
    {
        //msg ist an mich ( board ID )
        if( getRecipient(RxMessage.ExtId) == NODE_CAN_ID || getRecipient(RxMessage.ExtId) == NODE_V0_CAN_ID 
            || getRecipient(RxMessage.ExtId) == NODE_V1_CAN_ID || getRecipient(RxMessage.ExtId) == NODE_V2_CAN_ID || getRecipient(RxMessage.ExtId) == NODE_V3_CAN_ID )
        {
            if( getTyp(RxMessage.ExtId) == 0x8)
                send_pong(RxMessage);
            //alle sonderfaelle sind abgearbeitet --> type = modus
            else
            {
                LED_Toggle(1);
                if(getRecipient(RxMessage.ExtId) == NODE_CAN_ID)
                {
                    process_can_msg(RxMessage, 0);
                    process_can_msg(RxMessage, 1);
                    process_can_msg(RxMessage, 2);
                    process_can_msg(RxMessage, 3);
                }
                else if(getRecipient(RxMessage.ExtId) == NODE_V0_CAN_ID)
                {
                    process_can_msg(RxMessage, 0);
                }
                else if(getRecipient(RxMessage.ExtId) == NODE_V1_CAN_ID)
                {
                    process_can_msg(RxMessage, 1);
                }
                else if(getRecipient(RxMessage.ExtId) == NODE_V2_CAN_ID)
                {
                    process_can_msg(RxMessage, 2);
                }
                else if(getRecipient(RxMessage.ExtId) == NODE_V3_CAN_ID)
                {
                    process_can_msg(RxMessage, 3);
                }
            } // end else ping
        } //end an mich
    } //end else extended
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

