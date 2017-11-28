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
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  /* Configure one bit for preemption priority */
  /* 优先级组 说明了抢占优先级所用的位数，和子优先级所用的位数   在这里是1， 7 */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				 //外部中断2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //抢占优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //子优先级0  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //使能
  NVIC_Init(&NVIC_InitStructure);

  																  //用于选择EXTI1外部中断的输入源是PA1。
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);     //外部中断配置,网卡中断

  EXTI_InitStructure.EXTI_Line = EXTI_Line1;					  //配置外部中断1
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //下降沿触发
  //EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
/****************************************************************************
* 名    称：void Usart1_Init(void)
* 功    能：串口1初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void Usart1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 		//使能串口1时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//复用开漏输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A端口 

  USART_InitStructure.USART_BaudRate = 115200;						//速率115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);							//配置串口参数函数   
   /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);	
  
}

/****************************************************************************
* 名    称：void RCC_Configuration(void)
* 功    能：系统时钟配置为72MHZ， 外设时钟配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void RCC_Configuration(void)
{
   SystemInit(); 
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO  , ENABLE);  
 
}


/****************************************************************************
* 名    称：void GPIO_Configuration(void)
* 功    能：通用IO口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
void GPIO_Configuration(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
  	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //LED1控制
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 //LED2, LED3控制
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					 //SST25VF016B SPI片选
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_7;		 //PB12---VS1003 SPI片选（V2.1) 
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //PB7---触摸屏芯片XPT2046 SPI 片选
  
  /* 禁止SPI1总线上的其他设备 */
  GPIO_SetBits(GPIOB, GPIO_Pin_7);						     //触摸屏芯片XPT2046 SPI 片选禁止  
  GPIO_SetBits(GPIOB, GPIO_Pin_12);						     //VS1003 SPI片选（V2.1)禁止 
  GPIO_SetBits(GPIOC, GPIO_Pin_4);						     //SST25VF016B SPI片选禁止  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	         	 	//ENC28J60接收完成中断引脚 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//内部上拉输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);		 


//  GPIO_ResetBits(GPIOE, GPIO_Pin_1);						 //复位ENC28J60
//  Delay(0xAFFF);					   
//  GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 	             
//  Delay(0xAFFF);	
}


/****************************************************************************
* 名    称：int main(void)
* 功    能：程序入口
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
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
	
	RCC_Configuration();				 //系统时钟设置
	NVIC_Configuration();
	GPIO_Configuration();                //IO口初始化
	Usart1_Init();                       //串口1初始化

	/* 配置systic作为20ms中断 */
	if (SysTick_Config(20*72000))		//时钟节拍中断，20ms一次
    { 
    	/* error handle*/ 
     	while (1);
    }

	/* 向串口1发送开机字符 */
    printf("****       (C) COPYRIGHT 2013 制造者科技    *******\r\n");    	  //。
    printf("*                                                 *\r\n");  
    printf("*                                                 *\r\n");    	  	
    printf("*     MCU平台:STM32F103VET6                       *\r\n");    	
    printf("*     以太网硬件:ENC28J60                         *\r\n");    	  	
    printf("*     固件库：3.5                                 *\r\n");    	
    printf("*     例程版本: 0.2                               *\r\n");     
    printf("*                                                 *\r\n");    	 	
    printf("***************************************************\r\n");  	

	SPI1_Init();	      //SPI口初始化，用于网卡间传输数据

	tapdev_init();
	uip_init();				//uIP初始化	  
	
	
	uip_ipaddr(ipaddr, 192,168,1,16);	//设置本地设置IP地址
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,1,1); 	//设置网关IP地址(其实就是你路由器的IP地址)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//设置网络掩码
	uip_setnetmask(ipaddr);

	//uip_listen(HTONS(1200));			//监听1200端口,用于TCP Server
	//uip_listen(HTONS(80));				//监听80 端口,用于Web Server
  	//tcp_client_reconnect();	   		    //尝试连接到TCP Server端,用于TCP Client
	
//================================================================================================

//1. 首先初始化所有的UDP链接
	for( i = 0; i< UIP_UDP_CONNS; i++)
	{
		if(&uip_udp_conns[i] != NULL)
		{
			uip_udp_remove(&uip_udp_conns[i]);
		}
	}
	
	
//2. 新建一个链接,为这个链接指定远程IP和远程端口         远程端口：8087   远程IP：192.168.1.17
	uip_ipaddr(ipaddr, 192,168,1,17);
	UIP_udp_conn = uip_udp_new(&ipaddr, HTONS(8087));
//3. 将生成的链接绑定的一个指定的本地端口上              本地端口：8081
	if(UIP_udp_conn != NULL) {
		uip_udp_bind(UIP_udp_conn, HTONS(8081));
	}
//================================================================================================	
	while (1)
	{
		uip_polling();	//处理uip事件，必须插入到用户程序的循环体中  
	}
//		if(tcp_server_tsta!=tcp_server_sta)//TCP Server状态改变
//		{															 
//			if(tcp_server_sta&(1<<7))
//			{
//				printf("TCP Server Connected   ");
//			}
//			else
//			{
//				printf("TCP Server Disconnected");
//			}
// 			if(tcp_server_sta&(1<<6))	//收到新数据
//			{
//    			printf("TCP Server RX:%s\r\n",tcp_server_databuf);//打印数据
//				tcp_server_sta&=~(1<<6);		//标记数据已经被处理	
//			}
//			tcp_server_tsta=tcp_server_sta;
//		}
//		if(1)//TCP Server 请求发送数据
//		{
//			if(tcp_server_sta&(1<<7))	//连接还存在
//			{
//				sprintf((char*)tcp_server_databuf,"TCP Server OK %d\r\n",tcnt);	 
//				tcp_server_sta|=1<<5;//标记有数据需要发送
//				tcnt++;
//			}
//		}
//		if(tcp_client_tsta!=tcp_client_sta)//TCP Client状态改变
//		{															 
//			if(tcp_client_sta&(1<<7))
//			{
//				printf("TCP Client Connected   ");
//			}
//			else
//			{
//				printf("TCP Client Disconnected");
//			}
// 			if(tcp_client_sta&(1<<6))	//收到新数据
//			{
//    			printf("TCP Client RX:%s\r\n",tcp_client_databuf);//打印数据
//				tcp_client_sta&=~(1<<6);		//标记数据已经被处理	
//			}
//			tcp_client_tsta=tcp_client_sta;
//		}
//		if(0)//TCP Client 请求发送数据
//		{
//			if(tcp_client_sta&(1<<7))	//连接还存在
//			{
//				sprintf((char*)tcp_client_databuf,"TCP Client OK %d\r\n",tcnt);	 
//				tcp_client_sta|=1<<5;//标记有数据需要发送
//				tcnt++;
//			}
//		}
//	} 
//	
	//lwip_demo(NULL);	  //初始化内核，启动LwIP相关

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
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}				 
	uip_len=tapdev_read();	//从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义
	if(uip_len>0) 			//有数据
	{   
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(HBUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//uip_process(UIP_UDP_TIMER);
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}else if (HBUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)
			{
				tapdev_send();//需要发送数据,则通过tapdev_send发送	 
			}
		}
	}else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			//uip_udp_periodic(i);	//处理UDP通信事件
            do 
            { 
                uip_udp_conn = &uip_udp_conns[i]; 
                uip_process(UIP_UDP_TIMER); 
            } while (0);
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#endif 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
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
