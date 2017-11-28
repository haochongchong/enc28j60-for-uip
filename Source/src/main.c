#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include <stdio.h>	 
#include  <stdarg.h>




#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"	




//extern functions	
extern void SPI1_Init(void);
extern void lwip_demo(void *pdata);

void GPIO_Configuration(void);
void RCC_Configuration(void);

unsigned int system_tick_num = 0;
unsigned int sys_now(void)
{
    return system_tick_num;
}


/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  /* Configure one bit for preemption priority */
  /* ���ȼ��� ˵������ռ���ȼ����õ�λ�����������ȼ����õ�λ��   ��������1�� 7 */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				 //�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //�����ȼ�0  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ��
  NVIC_Init(&NVIC_InitStructure);

  																  //����ѡ��EXTI1�ⲿ�жϵ�����Դ��PA1��
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);     //�ⲿ�ж�����,�����ж�

  EXTI_InitStructure.EXTI_Line = EXTI_Line1;					  //�����ⲿ�ж�1
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //�ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //�½��ش���
  //EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
/****************************************************************************
* ��    �ƣ�void Usart1_Init(void)
* ��    �ܣ�����1��ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Usart1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 		//ʹ�ܴ���1ʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A�˿� 

  USART_InitStructure.USART_BaudRate = 115200;						//����115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);							//���ô��ڲ�������   
   /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);	
  
}

/****************************************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ������Ϊ72MHZ�� ����ʱ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void RCC_Configuration(void)
{
   SystemInit(); 
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO  , ENABLE);  
 
}


/****************************************************************************
* ��    �ƣ�void GPIO_Configuration(void)
* ��    �ܣ�ͨ��IO������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void GPIO_Configuration(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
  	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //LED1����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 //LED2, LED3����
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					 //SST25VF016B SPIƬѡ
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_7;		 //PB12---VS1003 SPIƬѡ��V2.1) 
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //PB7---������оƬXPT2046 SPI Ƭѡ
  
  /* ��ֹSPI1�����ϵ������豸 */
  GPIO_SetBits(GPIOB, GPIO_Pin_7);						     //������оƬXPT2046 SPI Ƭѡ��ֹ  
  GPIO_SetBits(GPIOB, GPIO_Pin_12);						     //VS1003 SPIƬѡ��V2.1)��ֹ 
  GPIO_SetBits(GPIOC, GPIO_Pin_4);						     //SST25VF016B SPIƬѡ��ֹ  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	         	 	//ENC28J60��������ж����� 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//�ڲ���������
  GPIO_Init(GPIOA, &GPIO_InitStructure);		 


//  GPIO_ResetBits(GPIOE, GPIO_Pin_1);						 //��λENC28J60
//  Delay(0xAFFF);					   
//  GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 	             
//  Delay(0xAFFF);	
}


/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ��������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
extern uint8_t tapdev_init(void);
static uip_ipaddr_t ipaddr;
static void uip_polling(void);	
uint16_t tcnt;
struct uip_udp_conn_constr *UIP_udp_conn;

int main(void)
{
	uint8_t i;
	u8 tcp_server_tsta=0XFF;
	u8 tcp_client_tsta=0XFF;
	
	RCC_Configuration();				 //ϵͳʱ������
	NVIC_Configuration();
	GPIO_Configuration();                //IO�ڳ�ʼ��
	Usart1_Init();                       //����1��ʼ��

	/* ����systic��Ϊ20ms�ж� */
	if (SysTick_Config(20*72000))		//ʱ�ӽ����жϣ�20msһ��
    { 
    	/* error handle*/ 
     	while (1);
    }

	/* �򴮿�1���Ϳ����ַ� */
    printf("****       (C) COPYRIGHT 2013 �����߿Ƽ�    *******\r\n");    	  //��
    printf("*                                                 *\r\n");  
    printf("*                                                 *\r\n");    	  	
    printf("*     MCUƽ̨:STM32F103VET6                       *\r\n");    	
    printf("*     ��̫��Ӳ��:ENC28J60                         *\r\n");    	  	
    printf("*     �̼��⣺3.5                                 *\r\n");    	
    printf("*     ���̰汾: 0.2                               *\r\n");     
    printf("*                                                 *\r\n");    	 	
    printf("***************************************************\r\n");  	

	SPI1_Init();	      //SPI�ڳ�ʼ�������������䴫������

	tapdev_init();
	uip_init();				//uIP��ʼ��	  
	
	
	uip_ipaddr(ipaddr, 192,168,1,16);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,1,1); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//������������
	uip_setnetmask(ipaddr);

	//uip_listen(HTONS(1200));			//����1200�˿�,����TCP Server
	//uip_listen(HTONS(80));				//����80 �˿�,����Web Server
  	//tcp_client_reconnect();	   		    //�������ӵ�TCP Server��,����TCP Client
	
//================================================================================================

//1. ���ȳ�ʼ�����е�UDP����
	for( i = 0; i< UIP_UDP_CONNS; i++)
	{
		if(&uip_udp_conns[i] != NULL)
		{
			uip_udp_remove(&uip_udp_conns[i]);
		}
	}
	
	
//2. �½�һ������,Ϊ�������ָ��Զ��IP��Զ�̶˿�         Զ�̶˿ڣ�8087   Զ��IP��192.168.1.17
	uip_ipaddr(ipaddr, 192,168,1,17);
	UIP_udp_conn = uip_udp_new(&ipaddr, HTONS(8087));
//3. �����ɵ����Ӱ󶨵�һ��ָ���ı��ض˿���              ���ض˿ڣ�8081
	if(UIP_udp_conn != NULL) {
		uip_udp_bind(UIP_udp_conn, HTONS(8081));
	}
//================================================================================================	
	while (1)
	{
		uip_polling();	//����uip�¼���������뵽�û������ѭ������  
	}
//		if(tcp_server_tsta!=tcp_server_sta)//TCP Server״̬�ı�
//		{															 
//			if(tcp_server_sta&(1<<7))
//			{
//				printf("TCP Server Connected   ");
//			}
//			else
//			{
//				printf("TCP Server Disconnected");
//			}
// 			if(tcp_server_sta&(1<<6))	//�յ�������
//			{
//    			printf("TCP Server RX:%s\r\n",tcp_server_databuf);//��ӡ����
//				tcp_server_sta&=~(1<<6);		//��������Ѿ�������	
//			}
//			tcp_server_tsta=tcp_server_sta;
//		}
//		if(1)//TCP Server ����������
//		{
//			if(tcp_server_sta&(1<<7))	//���ӻ�����
//			{
//				sprintf((char*)tcp_server_databuf,"TCP Server OK %d\r\n",tcnt);	 
//				tcp_server_sta|=1<<5;//�����������Ҫ����
//				tcnt++;
//			}
//		}
//		if(tcp_client_tsta!=tcp_client_sta)//TCP Client״̬�ı�
//		{															 
//			if(tcp_client_sta&(1<<7))
//			{
//				printf("TCP Client Connected   ");
//			}
//			else
//			{
//				printf("TCP Client Disconnected");
//			}
// 			if(tcp_client_sta&(1<<6))	//�յ�������
//			{
//    			printf("TCP Client RX:%s\r\n",tcp_client_databuf);//��ӡ����
//				tcp_client_sta&=~(1<<6);		//��������Ѿ�������	
//			}
//			tcp_client_tsta=tcp_client_sta;
//		}
//		if(0)//TCP Client ����������
//		{
//			if(tcp_client_sta&(1<<7))	//���ӻ�����
//			{
//				sprintf((char*)tcp_client_databuf,"TCP Client OK %d\r\n",tcnt);	 
//				tcp_client_sta|=1<<5;//�����������Ҫ����
//				tcnt++;
//			}
//		}
//	} 
//	
	//lwip_demo(NULL);	  //��ʼ���ںˣ�����LwIP���

	while (1)
	{
	    //exit(0);	
	}
}

#define HBUF ((struct uip_eth_hdr *)&uip_buf[0])	 
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}				 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���
	if(uip_len>0) 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(HBUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//uip_process(UIP_UDP_TIMER);
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (HBUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)
			{
				tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
			}
		}
	}else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			//uip_udp_periodic(i);	//����UDPͨ���¼�
            do 
            { 
                uip_udp_conn = &uip_udp_conns[i]; 
                uip_process(UIP_UDP_TIMER); 
            } while (0);
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}















__asm void EnableInt(unsigned int data)
{

    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

}
__asm unsigned int DisableInt(void)
{

    MSR     PRIMASK, R0;
    BX      LR

}
