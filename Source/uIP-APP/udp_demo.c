#include "udp_demo.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>	   

const char * cmd_1 = "cmd-1";
const char * ack2cmd_1 = "this is a ack to cmd-1";

static void process_appdata(uint8_t* appdata, uint16_t applen)
{
	uint16_t cmdlen;
	
	if(0 == memcmp(appdata,cmd_1,(cmdlen=strlen("cmd-1"))))//如果是命令是cmd1 那么回复特定的指令
	{
		memcpy(uip_appdata,ack2cmd_1,strlen(ack2cmd_1));
		uip_udp_send(strlen(ack2cmd_1));
	}else//否则，原样回复
	{
		uip_udp_send(applen);
	}
}


static void process_appdata2(uint8_t* appdata, uint16_t applen)
{
    memcpy(uip_appdata,"appdata2",strlen("appdata2"));
    uip_udp_send(strlen("appdata2")); 
}
    



//void udp_reback_appcall(void)
//{
//    static uint16_t app_to_udp_buflen=0;
//    static uint8_t app_to_udp_buf[64];
//    

//	//当接收到数据时，数据存放在  uip_appdata 中，长度为  uip_len
//	

////    if(uip_poll())
////    {
////        if(app_to_udp_buflen)
////        {
//////            uip_sappdata = uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];
//////            uip_slen = 0;
////        }
////    }
//}

void udp_demo_appcall(void)
{
	if(uip_newdata())
    {
		switch(uip_udp_conn->lport)//本地端口
		{
			case HTONS(8081):
				process_appdata(uip_appdata, uip_len);
				break;
            case HTONS(8082):
				process_appdata2(uip_appdata, uip_len);
				break;
			default:						  
				break;
		}
	}else if(uip_poll())
	{
		switch(uip_udp_conn->rport)//远程连接
		{
			case HTONS(8087):
				
				break;
			default: 
				break;
		}  
	}		
}


















