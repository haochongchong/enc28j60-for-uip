#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "rtc.h" 
#include "adc.h"
#include "tsensor.h"
#include "enc28j60.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"				   
#include "math.h" 	
#include "string.h"	 
//ALIENTEKս��STM32������ʵ��52
//ENC28J60���� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  
void uip_polling(void);	 												
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	 		  
extern u32 LCD_Pow(u8 m,u8 n);
//����->�ַ���ת������
//��num����(λ��Ϊlen)תΪ�ַ���,�����buf����
//num:����,����
//buf:�ַ�������
//len:����
void num2str(u16 num,u8 *buf,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		buf[i]=(num/LCD_Pow(10,len-i-1))%10+'0';
	}
}
//��ȡSTM32�ڲ��¶ȴ��������¶�
//temp:����¶��ַ������׵�ַ.��"28.3";
//temp,���ٵ���5���ֽڵĿռ�!
void get_temperature(u8 *temp)
{			  
	u16 t;
	float temperate;		   
	temperate=Get_Adc_Average(ADC_CH_TEMP,10);			 
	temperate=temperate*(3.3/4096);			    											    
	temperate=(1.43-temperate)/0.0043+25;	//�������ǰ�¶�ֵ
	t=temperate*10;//�õ��¶�
	num2str(t/10,temp,2);							   
	temp[2]='.';temp[3]=t%10+'0';temp[4]=0;	//�����ӽ�����
}
//��ȡRTCʱ��
//time:���ʱ���ַ���,����:"2012-09-27 12:33"
//time,���ٵ���17���ֽڵĿռ�!
void get_time(u8 *time)
{	
	RTC_Get();
	time[4]='-';time[7]='-';time[10]=' ';
	time[13]=':';time[16]=0;			//�����ӽ�����
	num2str(calendar.w_year,time,4);	//���->�ַ���
	num2str(calendar.w_month,time+5,2); //�·�->�ַ���	 
	num2str(calendar.w_date,time+8,2); 	//����->�ַ���
	num2str(calendar.hour,time+11,2); 	//Сʱ->�ַ���
	num2str(calendar.min,time+14,2); 	//����->�ַ���								   	  									  
}

 int main(void)
 {	 
	u8 key;
	u8 tcnt=0;
	u8 tcp_server_tsta=0XFF;
	u8 tcp_client_tsta=0XFF;
 	uip_ipaddr_t ipaddr;

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	
	KEY_Init();				//��ʼ������
	RTC_Init();				//��ʼ��RTC
	Adc_Init();				//��ʼ��ADC	  

	POINT_COLOR=RED;		//����Ϊ��ɫ	   
	LCD_ShowString(60,10,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,30,200,16,16,"ENC28J60 TEST");	
	LCD_ShowString(60,50,200,16,16,"ATOM@ALIENTEK");
 	while(tapdev_init())	//��ʼ��ENC28J60����
	{								   
		LCD_ShowString(60,70,200,16,16,"ENC28J60 Init Error!");	 
		delay_ms(200);
 		LCD_Fill(60,70,240,86,WHITE);//���֮ǰ��ʾ
	};		
	uip_init();				//uIP��ʼ��	  
	LCD_ShowString(60,70,200,16,16,"KEY0:Server Send Msg");	 
	LCD_ShowString(60,90,200,16,16,"KEY2:Client Send Msg");	  
	LCD_ShowString(60,110,200,16,16,"IP:192.168.1.16");	   						  	 
	LCD_ShowString(60,130,200,16,16,"MASK:255.255.255.0");	   						  	 
	LCD_ShowString(60,150,200,16,16,"GATEWAY:192.168.1.1");	   						  	 
	
 	LCD_ShowString(30,200,200,16,16,"TCP RX:");	   						  	 
	LCD_ShowString(30,220,200,16,16,"TCP TX:");	
	   						  	   						  	 
	LCD_ShowString(30,270,200,16,16,"TCP RX:");	   						  	 
	LCD_ShowString(30,290,200,16,16,"TCP TX:");	   
LCD_ShowString(30,290,200,16,16,"TCP TX:");	   	
	POINT_COLOR=BLUE;	   
 
 	uip_ipaddr(ipaddr, 192,168,1,16);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,1,1); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//������������
	uip_setnetmask(ipaddr);

	uip_listen(HTONS(1200));			//����1200�˿�,����TCP Server
	uip_listen(HTONS(80));				//����80�˿�,����Web Server
  	tcp_client_reconnect();	   		    //�������ӵ�TCP Server��,����TCP Client
	while (1)
	{
		uip_polling();	//����uip�¼���������뵽�û������ѭ������  
		key=KEY_Scan(0);
		if(tcp_server_tsta!=tcp_server_sta)//TCP Server״̬�ı�
		{															 
			if(tcp_server_sta&(1<<7))LCD_ShowString(30,180,200,16,16,"TCP Server Connected   ");
			else LCD_ShowString(30,180,200,16,16,"TCP Server Disconnected");
 			if(tcp_server_sta&(1<<6))	//�յ�������
			{
				LCD_Fill(86,200,240,216,WHITE);	//���֮ǰ��ʾ
				LCD_ShowString(86,200,154,16,16,tcp_server_databuf);
    			printf("TCP Server RX:%s\r\n",tcp_server_databuf);//��ӡ����
				tcp_server_sta&=~(1<<6);		//��������Ѿ�������	
			}
			tcp_server_tsta=tcp_server_sta;
		}
		if(key==KEY_RIGHT)//TCP Server ����������
		{
			if(tcp_server_sta&(1<<7))	//���ӻ�����
			{
				sprintf((char*)tcp_server_databuf,"TCP Server OK %d\r\n",tcnt);	 
				LCD_Fill(86,220,240,236,WHITE);		//���֮ǰ��ʾ
				LCD_ShowString(86,220,154,16,16,tcp_server_databuf);//��ʾ��ǰ��������
				tcp_server_sta|=1<<5;//�����������Ҫ����
				tcnt++;
			}
		}
		if(tcp_client_tsta!=tcp_client_sta)//TCP Client״̬�ı�
		{															 
			if(tcp_client_sta&(1<<7))LCD_ShowString(30,250,200,16,16,"TCP Client Connected   ");
			else LCD_ShowString(30,250,200,16,16,"TCP Client Disconnected");
 			if(tcp_client_sta&(1<<6))	//�յ�������
			{
				LCD_Fill(86,270,240,286,WHITE);	//���֮ǰ��ʾ
				LCD_ShowString(86,270,154,16,16,tcp_client_databuf);
    			printf("TCP Client RX:%s\r\n",tcp_client_databuf);//��ӡ����
				tcp_client_sta&=~(1<<6);		//��������Ѿ�������	
			}
			tcp_client_tsta=tcp_client_sta;
		}
		if(key==KEY_LEFT)//TCP Client ����������
		{
			if(tcp_client_sta&(1<<7))	//���ӻ�����
			{
				sprintf((char*)tcp_client_databuf,"TCP Client OK %d\r\n",tcnt);	 
				LCD_Fill(86,290,240,306,WHITE);		//���֮ǰ��ʾ
				LCD_ShowString(86,290,154,16,16,tcp_client_databuf);//��ʾ��ǰ��������
				tcp_client_sta|=1<<5;//�����������Ҫ����
				tcnt++;
			}
		}
		delay_ms(1);
	}  
} 
//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.
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
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
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
			uip_udp_periodic(i);	//����UDPͨ���¼�
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
