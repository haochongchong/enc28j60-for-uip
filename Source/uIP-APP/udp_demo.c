#include "udp_demo.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>	   

void udp_reback_appcall(void)
{
    static uint16_t app_to_udp_buflen=0;
    static uint8_t app_to_udp_buf[64];
    
    if(uip_newdata())
    {
        memcpy(uip_appdata,"1234567890",10);
        uip_udp_send(10);
    }
//    if(uip_poll())
//    {
//        if(app_to_udp_buflen)
//        {
////            uip_sappdata = uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];
////            uip_slen = 0;
//        }
//    }
}

void udp_demo_appcall(void)
{
    switch(uip_udp_conn->lport)//本地端口
	{
		case HTONS(8081):
			udp_reback_appcall();
			break;
		default:						  
		    break;
	}		    
	switch(uip_udp_conn->rport)//远程连接
	{
	    case HTONS(8087):
            
            break;
	    default: 
            break;
	}   
}


















