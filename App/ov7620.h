#ifndef _ov7620_H
#define _ov7620_H

#include "include.h"

#define H 80    
#define L 300
#define SIZE H*L

extern uint8 image_date[H][L];
extern uint32 H_count;
extern uint8 image_send;

extern void PORTB_IRQHandler();
extern void DMA0_IRQHandler();
extern void PORTA_IRQHandler();
 
#endif