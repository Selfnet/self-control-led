
#ifndef __CAN_H__
#define __CAN_H__

#define CAN_ID 0x11

// Can functions
void CAN_config(void);
void prozess_can_it(void);

int getSender(uint32_t ExtId);
int getRecipient(uint32_t ExtId);
int getTyp(uint32_t ExtId);
void setSender(uint32_t *ExtId , int recipient);
void setRecipient(uint32_t *ExtId , int recipient);
void setTyp(uint32_t *ExtId , int recipient);

//NODE_CAN_ID = 64 = 0x40 = 0b1000000
#define NODE_CAN_ID 0x40
#define NODE_V0_CAN_ID 0x41
#define NODE_V1_CAN_ID 0x42
#define NODE_V2_CAN_ID 0x43
#define NODE_V3_CAN_ID 0x44

#endif
