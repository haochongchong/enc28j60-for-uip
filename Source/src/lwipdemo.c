#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/timers.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include <stdio.h>	

//extern functions
extern err_t ethernetif_init(struct netif *netif);
extern void process_mac(void);

//global data
struct netif enc28j60_netif;

void lwip_init_task(void)
{
	struct ip_addr ipaddr, netmask, gw;

    lwip_init();
	IP4_ADDR(&gw, 192,168,1,1);
	
	IP4_ADDR(&ipaddr, 192,168,1,37);
	IP4_ADDR(&netmask, 255,255,255,0);

    netif_add(&enc28j60_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,ethernet_input);
	netif_set_default(&enc28j60_netif);
	netif_set_up(&enc28j60_netif);
	
}

#include "loopback.h"

void lwip_demo(void *pdata)
{
	//init LwIP
	lwip_init_task();

	//setup tcp server
	tcpserver_init();
	
    loopclient_init();
	//for periodic handle
	while(1)
	{
		process_mac();
		//process LwIP timeout
		sys_check_timeouts();

		//poll all loop data on all interface
		netif_poll_all();
		
		//todo: add your own user code here

	}
}


