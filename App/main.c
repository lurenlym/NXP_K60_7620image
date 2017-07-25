
#include "common.h"
#include "include.h"
#include "ov7620.h"


//函数声明
void PORTA_IRQHandler();
void PORTB_IRQHandler();
void DMA0_IRQHandler();
void send();
void sccb();
uint8 image_date[H][L]={0};
uint32 H_count=0;
uint8 image_send=0;

uint32 time_temp=0;



void  main()
{
  
    DisableInterrupts;
    set_vector_handler(PORTA_VECTORn , PORTA_IRQHandler);  
    set_vector_handler(PORTB_VECTORn , PORTB_IRQHandler); 
    set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);
    gpio_init(PTB8,GPI,0);
    port_init (PTB8, IRQ_RISING | PF | ALT1 | PULLDOWN  ); //hang
    
    disable_irq(PORTB_IRQn);
    gpio_init(PTA27,GPI,0);
    port_init (PTA27, DMA_RISING| PF | ALT1 | PULLDOWN  );
    
    //disable_irq(DMA0_IRQn);
    gpio_init(PTA29,GPI,0);
    port_init (PTA29, IRQ_FALLING | ALT1 | PULLDOWN  );//chang
   
    enable_irq(PORTA_IRQn);
    uart_init(UART4,115200);
    EnableInterrupts;
     pit_time_start(PIT0);
    while(1)
    {
      
      if(image_send==1)
      {     
      
      send();
      
      
      PORTB_ISFR |= (1<<8);
      PORTA_ISFR |= (1<<29);
      //enable_irq(PORTB_IRQn);                  
      enable_irq(PORTA_IRQn);
      pit_time_start(PIT0);
      image_send=0;
     
      
      }
    }
}

void send()
{
      int i=0,j=0;

      //以下四句话是固定格式，是串口和上位机软件之间的协议
      uart_putchar(UART4,0x01);
      uart_putchar(UART4,0xfe);
      
      for(i=0;i<H;i++)
      {
        for(j=0;j<L;j++)
        {
          
            uart_putchar(UART4,image_date[i][j]);

        }

      } 
      uart_putchar(UART4,0xfe);
      uart_putchar(UART4,0x01);
      //uart_putchar(UART1,0x01);
     
}


void PORTA_IRQHandler()
{
    if(PORTA_ISFR & (1<<29))//PTB0
    {
      PORTA_ISFR |= (1<<29);                                
      image_send= 0; 
      dma_portx2buff_init(DMA_CH0, (void *)&PTB_B0_IN, image_date, PTA27, DMA_BYTE1,L,DADDR_KEEPON); 
      enable_irq(PORTB_IRQn);
      disable_irq(PORTA_IRQn);
      image_send= 0; 
    }
}


void PORTB_IRQHandler()
{
    if(PORTB_ISFR & (1<<8))  //PTA29                             
    {
      PORTB_ISFR |= (1<<8);                                
      if(++H_count%2==0)
      {
      DMA_EN(DMA_CH0);
      }
    }
}
void DMA0_IRQHandler()
{
    DMA_DIS(DMA_CH0);
    DMA_IRQ_CLEAN(DMA_CH0);                               
    
    if(H_count==160) 
    { 
      disable_irq(PORTB_IRQn);
      disable_irq(PORTA_IRQn);
      image_send=1;
      time_temp=pit_time_get(PIT0);
      pit_close(PIT0);
      H_count=0;
    }
}
void sccb()
{
  SCCB_GPIO_init();
  SCCB_WriteByte(0x12,0x80);
}