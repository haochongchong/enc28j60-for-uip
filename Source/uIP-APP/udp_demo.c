#include "udp_demo.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>	   






void udp_demo_appcall(void)
{
	memcpy(uip_appdata,"1234567890",10);
	uip_udp_send(10);
}










