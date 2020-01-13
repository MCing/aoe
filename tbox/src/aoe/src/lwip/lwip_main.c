//
// Created by d.eroshenkov on 04.05.2018.
//

/* C runtime includes */
#include <stdio.h>
#include <time.h>
#include "aoe_common.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/mdns.h"
#include "netif/ppp/ppp_opts.h"

#if LWIP_USING_NAT
#include "lwip_nat/ipv4_nat.h"
#endif

#if PPP_SUPPORT
/* PPP includes */
#include "lwip/sio.h"
#include "netif/ppp/pppapi.h"

#if !NO_SYS && !LWIP_PPP_API
#error With NO_SYS==0, LWIP_PPP_API==1 is required.
#endif
#endif /* PPP_SUPPORT */

#if AOE_REFACTORING
extern void exit_ppp_mode();
extern int enter_ppp_mode();
extern int ppp_at_dial();
extern int ppp_mode_inused();
#endif

/* include the port-dependent configuration */
#include "lwipcfg_msvc.h"

/** Define this to 1 to enable a PCAP interface as default interface. */
#ifndef USE_PCAPIF
#define USE_PCAPIF 1
#endif

/** Define this to 1 to enable a PPP interface. */
#ifndef USE_PPP
#define USE_PPP 1
#endif



#if USE_ETHERNET

#include "pcapif.h"
//#include "interfaces.h"

#endif /* USE_ETHERNET */

#ifndef USE_DHCP
#define USE_DHCP    LWIP_DHCP
#endif
#ifndef USE_AUTOIP
#define USE_AUTOIP  LWIP_AUTOIP
#endif

/* globales variables for netifs */
#if USE_ETHERNET
/* THE ethernet interface */
struct netif netif;
#if LWIP_DHCP
/* dhcp struct for the ethernet netif */
struct dhcp netif_dhcp;
#endif /* LWIP_DHCP */
#if LWIP_AUTOIP
/* autoip struct for the ethernet netif */
struct autoip netif_autoip;
#endif /* LWIP_AUTOIP */
#endif /* USE_ETHERNET */
#if USE_PPP
/* THE PPP PCB */
ppp_pcb *ppp;
/* THE PPP interface */
struct netif ppp_netif;
/* THE PPP descriptor */
u8_t sio_idx = 3;
sio_fd_t ppp_sio;
#endif /* USE_PPP */

#if LWIP_USING_NAT
ip_nat_entry_t nat_entry;
static void initializeNAT();
#endif

//#include "interfaces.h"

#if USE_PPP
#include <stdbool.h>
#include <lwip/autoip.h>
#include <lwip/dhcp.h>
#include <synchapi.h>

//static bool natInitialized;
//static bool interfaceInitialized;
bool dhcp;
volatile bool pppConnected;
static bool callClosePpp;
static bool lwipInitialized;
bool dhcpConfigured;

static void pppLinkStatusCallback(ppp_pcb *pcb, int errCode, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);
	printf("pppLinkStatusCallback: errCode:%d\n", errCode);

    switch (errCode)
    {
        case PPPERR_NONE:
        {             /* No error. */
            printf("pppLinkStatusCallback: PPPERR_NONE\n");
#if LWIP_IPV4
            printf("   our_ipaddr  = %s\n", ip4addr_ntoa(netif_ip4_addr(pppif)));
            printf("   his_ipaddr  = %s\n", ip4addr_ntoa(netif_ip4_gw(pppif)));
            printf("   netmask     = %s\n", ip4addr_ntoa(netif_ip4_netmask(pppif)));
#endif /* LWIP_IPV4 */
#if LWIP_DNS
            printf("   dns1        = %s\n", ipaddr_ntoa(dns_getserver(0)));
            printf("   dns2        = %s\n", ipaddr_ntoa(dns_getserver(1)));
#endif /* LWIP_DNS */
#if PPP_IPV6_SUPPORT
            printf("   our6_ipaddr = %s\n", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */

            pppConnected = true;


			#if LWIP_USING_NAT
			initializeNAT();
			#endif

            break;
        }
        case PPPERR_PARAM:
        {           /* Invalid parameter. */
            printf("pppLinkStatusCallback: PPPERR_PARAM\n");
            break;
        }
        case PPPERR_OPEN:
        {            /* Unable to open PPP session. */
            printf("pppLinkStatusCallback: PPPERR_OPEN\n");
            break;
        }
        case PPPERR_DEVICE:
        {          /* Invalid I/O device for PPP. */
            printf("pppLinkStatusCallback: PPPERR_DEVICE\n");
            break;
        }
        case PPPERR_ALLOC:
        {           /* Unable to allocate resources. */
            printf("pppLinkStatusCallback: PPPERR_ALLOC\n");
            break;
        }
        case PPPERR_USER:
        {            /* User interrupt. */
            printf("pppLinkStatusCallback: PPPERR_USER\n");
            break;
        }
        case PPPERR_CONNECT:
        {         /* Connection lost. */
            printf("pppLinkStatusCallback: PPPERR_CONNECT\n");
            break;
        }
        case PPPERR_AUTHFAIL:
        {        /* Failed authentication challenge. */
            printf("pppLinkStatusCallback: PPPERR_AUTHFAIL\n");
            break;
        }
        case PPPERR_PROTOCOL:
        {        /* Failed to meet protocol. */
            printf("pppLinkStatusCallback: PPPERR_PROTOCOL\n");
            break;
        }
        case PPPERR_PEERDEAD:
        {        /* Connection timeout */
            printf("pppLinkStatusCallback: PPPERR_PEERDEAD\n");
            pppConnected = false;
            callClosePpp = true;
            break;
        }
        case PPPERR_IDLETIMEOUT:
        {     /* Idle Timeout */
            printf("pppLinkStatusCallback: PPPERR_IDLETIMEOUT\n");
            break;
        }
        case PPPERR_CONNECTTIME:
        {     /* Max connect time reached */
            printf("pppLinkStatusCallback: PPPERR_CONNECTTIME\n");
            break;
        }
        case PPPERR_LOOPBACK:
        {        /* Loopback detected */
            printf("pppLinkStatusCallback: PPPERR_LOOPBACK\n");
            break;
        }
        default:
        {
            printf("pppLinkStatusCallback: unknown errCode %d\n", errCode);
            break;
        }
    }

	//debug
	if(errCode != PPPERR_NONE)
	{
		pppapi_free(ppp);
        puts("ppp closed.\n");
        ppp = NULL;
		exit_ppp_mode();
	}
}

#if PPPOS_SUPPORT

static u32_t ppp_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(ctx);
    return sio_write(ppp_sio, data, len);
}

#endif /* PPPOS_SUPPORT */
#endif /* USE_PPP */

#if LWIP_NETIF_STATUS_CALLBACK

ip4_addr_t ourAddr = {0};
ip4_addr_t hisAddr = {0};

u_long ADAPTER_ADDR = 0;
u_long ADAPTER_MASK = 0;
u_long ADAPTER_GW = 0;
u_long ADAPTER_DNS = 0;

static void status_callback(struct netif *state_netif)
{
    if (netif_is_up(state_netif))
    {
#if LWIP_IPV4
        printf("status_callback==UP, local interface IP is %s\n", ip4addr_ntoa(netif_ip4_addr(state_netif)));
#if LWIP_USING_NAT

        initializeNAT();
#endif
#if LWIP_MDNS_RESPONDER
        mdns_resp_netif_settings_changed(state_netif);
//        }
#endif
    }
    else
    {
        printf("status_callback==DOWN\n");

#if LWIP_USING_NAT
        ip_nat_remove(&nat_entry);
#endif
    }

}

#endif /* LWIP_IPV4 */
#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_LINK_CALLBACK

static void link_callback(struct netif *state_netif)
{
    if (netif_is_link_up(state_netif))
    {
        printf("link_callback==UP\n");
    }
    else
    {
        printf("link_callback==DOWN\n");
    }
}

#endif /* LWIP_NETIF_LINK_CALLBACK */


/* This function initializes all network interfaces */
static int initializeInterfaces(void)
{
//#if LWIP_IPV4 && USE_ETHERNET
    ip4_addr_t ipaddr, netmask, gw;
//#endif /* LWIP_IPV4 && USE_ETHERNET */
	PPP_OPT *ppp_config = aoe_get_ppp_opt();

    static bool interfaceInitialized = false;

#if USE_DHCP || USE_AUTOIP
    err_t err;
#endif

    if (!callClosePpp)
    {
		sio_idx = aoe_get_uart_portno();
        printf("ppp_connect: COM%d\n", (int) sio_idx);
        ppp_sio = sio_open(sio_idx);
        if (ppp_sio == NULL)
        {
            printf("sio_open error\n");
			return -1;
        }
        else
        {
        	printf("sio_open ok\n");
			//TODO set apn AT+CGDCONT
			//if(ppp_config->apn)

			if(ppp_at_dial())
			{
				printf("enter ppp mode fail\n");
				return -1;
			}
			enter_ppp_mode();
            /* Initiate PPP client connection. */
            ppp = pppapi_pppos_create(&ppp_netif, ppp_output_cb, pppLinkStatusCallback, NULL);
            if (ppp == NULL)
            {
                printf("pppos_create error\n");
				return -1;
            }

            /* Initiate PPP server connection. */
            ip4_addr_t addr;

            /* Set our address */
			//TODO
			//if(ppp_config->remote_ip)
            //ppp_set_ipcp_ouraddr(ppp, &ourAddr);

            /* Set peer(his) address */
			//TODO
            //ppp_set_ipcp_hisaddr(ppp, &hisAddr);

            /* Set secondary DNS server */

           ppp_set_usepeerdns(ppp, 1);

            /* Auth configuration, this is pretty self-explanatory */
			
        	ppp_set_auth(ppp, ppp_config->auth_type, ppp_config->user, ppp_config->pwd);
			pppapi_set_default(ppp);
			ppp_set_echo_opt(ppp, ppp_config->lcp_echo_interval,ppp_config->lcp_echo_fails);
            /* Require peer to authenticate */
			//TODO to figure out
        	//ppp_set_auth_required(ppp, 1);

/*
 * Only for PPPoS, the PPP session should be up and waiting for input.
 *
 * Note: for PPPoS, ppp_connect() and ppp_listen() are actually the same thing.
 * The listen call is meant for future support of PPPoE and PPPoL2TP server
 * mode, where we will need to negotiate the incoming PPPoE session or L2TP
 * session before initiating PPP itself. We need this call because there is
 * two passive modes for PPPoS, ppp_set_passive and ppp_set_silent.
 */
            //ppp_set_silent(ppp, 1);
            
/*
 * Initiate PPP listener (i.e. wait for an incoming connection), can only
 * be called if PPP session is in the dead state (i.e. disconnected).
 */
            //ppp_listen(ppp);
            pppapi_connect(ppp, 0);

        }
    }
	return 0;
}


#if LWIP_USING_NAT

static void initializeNAT()
{
    static bool natInitialized = false;

    if (!natInitialized)
    {
        if (!pppConnected)
        {
            return;
        }

        if (dhcp && !dhcp_supplied_address(&netif))
        {
            return;
        }

        ADAPTER_ADDR = netif.ip_addr.addr;
        createRoute(hisAddr.addr, ADAPTER_ADDR);

        printf("\n\n\n!!!NAT!!!\n\n\n");
        nat_entry.out_if = (struct netif *) &netif;
        nat_entry.in_if = (struct netif *) &ppp_netif;
        nat_entry.source_net = hisAddr;
        nat_entry.dest_net.addr = ADAPTER_ADDR;
        nat_entry.dest_netmask.addr = ADAPTER_MASK;
        IP4_ADDR(&nat_entry.source_netmask, 255, 255, 255, 0);
        printf("nat_entry.source_net.addr = %u\n", nat_entry.source_net.addr);
        printf("nat_entry.dest_net.addr = %u\n", nat_entry.dest_net.addr);
        printf("nat_entry.dest_netmask.addr = %u\n", nat_entry.dest_netmask.addr);
        ip_nat_add(&nat_entry);
        printf("nat_entry.source_netmask.addr = %u\n", nat_entry.source_netmask.addr);

        natInitialized = true;
    }
}

#endif


//#if LWIP_DNS_APP && LWIP_DNS
//static void
//dns_found(const char *name, const ip_addr_t *addr, void *arg)
//{
//  LWIP_UNUSED_ARG(arg);
//  printf("%s: %s\n", name, addr ? ipaddr_ntoa(addr) : "<not found>");
//}
//
//static void
//dns_dorequest(void *arg)
//{
//  const char* dnsname = "3com.com";
//  ip_addr_t dnsresp;
//  LWIP_UNUSED_ARG(arg);
//
//  if (dns_gethostbyname(dnsname, &dnsresp, dns_found, 0) == ERR_OK) {
//    dns_found(dnsname, &dnsresp, 0);
//  }
//}
//#endif /* LWIP_DNS_APP && LWIP_DNS */
//
//#if LWIP_LWIPERF_APP
//static void
//lwiperf_report(void *arg, enum lwiperf_report_type report_type,
//  const ip_addr_t* local_addr, u16_t local_port, const ip_addr_t* remote_addr, u16_t remote_port,
//  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
//{
//  LWIP_UNUSED_ARG(arg);
//  LWIP_UNUSED_ARG(local_addr);
//  LWIP_UNUSED_ARG(local_port);
//
//  printf("IPERF report: type=%d, remote: %s:%d, total bytes: %lu, duration in ms: %lu, kbits/s: %lu\n",
//    (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec);
//}
//#endif /* LWIP_LWIPERF_APP */

#if LWIP_MDNS_RESPONDER

static void srv_txt(struct mdns_service *service, void *txt_userdata)
{
    err_t res = mdns_resp_add_service_txtitem(service, "path=/", 6);
    LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);
    LWIP_UNUSED_ARG(txt_userdata);
}

#endif

/* This function initializes applications */
static void apps_init(void)
{
//#if LWIP_DNS_APP && LWIP_DNS
//    /* wait until the netif is up (for dhcp, autoip or ppp) */
//  sys_timeout(5000, dns_dorequest, NULL);
//#endif /* LWIP_DNS_APP && LWIP_DNS */
//
//#if LWIP_CHARGEN_APP && LWIP_SOCKET
//    chargen_init();
//#endif /* LWIP_CHARGEN_APP && LWIP_SOCKET */
//
//#if LWIP_PING_APP && LWIP_RAW && LWIP_ICMP
//    ping_init();
//#endif /* LWIP_PING_APP && LWIP_RAW && LWIP_ICMP */
//
//#if LWIP_NETBIOS_APP && LWIP_UDP
//    netbiosns_init();
//#ifndef NETBIOS_LWIP_NAME
//#if LWIP_NETIF_HOSTNAME
//  netbiosns_set_name(netif_default->hostname);
//#else
//  netbiosns_set_name("NETBIOSLWIPDEV");
//#endif
//#endif
//#endif /* LWIP_NETBIOS_APP && LWIP_UDP */
//
//#if LWIP_HTTPD_APP && LWIP_TCP
//#ifdef LWIP_HTTPD_APP_NETCONN
//    http_server_netconn_init();
//#else /* LWIP_HTTPD_APP_NETCONN */
//    httpd_init();
//#endif /* LWIP_HTTPD_APP_NETCONN */
//#endif /* LWIP_HTTPD_APP && LWIP_TCP */



//#if LWIP_MDNS_RESPONDER
//    mdns_resp_init();
//#if LWIP_NETIF_HOSTNAME
//    mdns_resp_add_netif(netif_default, netif_default->hostname, 3600);
//#else
//    mdns_resp_add_netif(netif_default, "lwip", 3600);
//#endif
//    mdns_resp_add_service(netif_default, "lwipweb", "_http", DNSSD_PROTO_TCP, HTTPD_SERVER_PORT, 3600, srv_txt, NULL);
//#endif




//#if LWIP_NETIO_APP && LWIP_TCP
//    netio_init();
//#endif /* LWIP_NETIO_APP && LWIP_TCP */
//
//#if LWIP_RTP_APP && LWIP_SOCKET && LWIP_IGMP
//    rtp_init();
//#endif /* LWIP_RTP_APP && LWIP_SOCKET && LWIP_IGMP */
//
//#if LWIP_SNTP_APP && LWIP_SOCKET
//    sntp_init();
//#endif /* LWIP_SNTP_APP && LWIP_SOCKET */
//
//#if LWIP_SHELL_APP && LWIP_NETCONN
//    shell_init();
//#endif /* LWIP_SHELL_APP && LWIP_NETCONN */
//#if LWIP_TCPECHO_APP
//#if LWIP_NETCONN && defined(LWIP_TCPECHO_APP_NETCONN)
//    tcpecho_init();
//#else /* LWIP_NETCONN && defined(LWIP_TCPECHO_APP_NETCONN) */
//    tcpecho_raw_init();
//#endif
//#endif /* LWIP_TCPECHO_APP && LWIP_NETCONN */
//#if LWIP_UDPECHO_APP && LWIP_NETCONN
//    udpecho_init();
//#endif /* LWIP_UDPECHO_APP && LWIP_NETCONN */
//#if LWIP_LWIPERF_APP
//    lwiperf_start_tcp_server_default(lwiperf_report, NULL);
//#endif
//#if LWIP_SOCKET_EXAMPLES_APP && LWIP_SOCKET
//    socket_examples_init();
//#endif /* LWIP_SOCKET_EXAMPLES_APP && LWIP_SOCKET */
//#ifdef LWIP_APP_INIT
//    LWIP_APP_INIT();
//#endif
}

/* This function initializes this lwIP test. When NO_SYS=1, this is done in
 * the main_loop context (there is no other one), when NO_SYS=0, this is done
 * in the tcpip_thread context */
static void tcpipInitCallback(void *arg)
{ /* remove compiler warning */
    sys_sem_t *init_sem;
    LWIP_ASSERT("arg != NULL", arg != NULL);
    init_sem = (sys_sem_t *) arg;

    /* init randomizer again (seed per thread) */
    srand((unsigned int) time(0));

//    /* init network interfaces */
//    initializeInterfaces();

    /* init apps */
//    apps_init();

    sys_sem_signal(init_sem);
}

int lwipInit()
{
    if (!lwipInitialized)
    {

        err_t err = 0;
        sys_sem_t init_sem = {0};
        /* initialize lwIP stack, network interfaces and applications */
        err = sys_sem_new(&init_sem, 0);
        LWIP_ASSERT("failed to create init_sem", err == ERR_OK);
        LWIP_UNUSED_ARG(err);
        tcpip_init(tcpipInitCallback, &init_sem);
        /* we have to wait for initialization to finish before
         * calling update_adapter()! */
        sys_sem_wait(&init_sem);
        sys_sem_free(&init_sem);

        lwipInitialized = true;
    }

    /* init network interfaces */
    return initializeInterfaces();
}

void app_notify_exit_ppp()
{
	callClosePpp = 1;
}

/* This is somewhat different to other ports: we have a main loop here:
 * a dedicated task that waits for packets to arrive. This would normally be
 * done from interrupt context with embedded hardware, but we don't get an
 * interrupt in windows for that :-) */
int lwipLoop(void *param)
{


    int count = 0;
    u8_t rxbuf[1024] = {0};

    if(lwipInit())
    {
    	printf("lwipInit error\n");
		return -1;
	}

    /* MAIN LOOP for driver update (and timers if NO_SYS) */
    while (1)
    {
        sys_msleep(50);

		if(ppp_mode_inused() == 0)
			continue;

        /* try to read characters from serial line and pass them to PPPoS */
        count = sio_read(ppp_sio, (u8_t *) rxbuf, 1024);
	
        if (count > 0)
        {
            pppos_input_tcpip(ppp, rxbuf, count);
        }
        else
        {
            /* nothing received, give other tasks a chance to run */
            sys_msleep(1);
        }

        if (callClosePpp && ppp)
        {
            /* make sure to disconnect PPP before stopping the program... */
            callClosePpp = false;
            pppapi_close(ppp, 0);
            //pppapi_free(ppp);
            //puts("ppp closed.\n");
            //ppp = NULL;
            break;
        }
    }


}

#include "lwip/tcp.h"
#define TCPREVDATALEN 4096

typedef enum{
    ES_CONNECTED,
    ES_CLOSING,
} es_state;

struct client{
    struct tcp_pcb *pcb;
    struct pbuf *p_tx;
    es_state state;
};

struct client *es;

int tcpRecevieFlag;

unsigned char recevRxBufferTcpEth[TCPREVDATALEN];

static void tcp_client_connection_close();
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

u8_t tcp_send_message(void *msg, u16_t len);

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    printf("tcp_client_connected_ERROR:%d\r\n",err);
   
    if(es == NULL) es = (struct client *)mem_malloc(sizeof(struct client));
    switch(err){
        case ERR_OK:  
          es->pcb = tpcb;                                  /*¨¦¨¨??¦Ì¡À?¡ã¦Ì?¡Á¡ä¨¬??a¨¢??¨®¡Á¡ä¨¬??*/  
          es->p_tx = NULL;
          es->state = ES_CONNECTED;  
          tcp_arg(tpcb, es);                       /*¡ã?¦Ì¡À?¡ã¦Ì?2?¨ºy¡ä????¨´¨®D¦Ì???¦Ì¡Âo¡¥¨ºy?   */ 
          tcp_recv(tpcb, tcp_client_recv);        
          break ;
       case ERR_MEM :
           tcp_client_connection_close(tpcb, es);                
          break ;
       default :
           break ;
           
   }
   return err;
}


err_t tcp_client_connect(const u8_t *destip, u16_t port)
{
  struct tcp_pcb *client_pcb;
  ip4_addr_t DestIPaddr;
  err_t err = ERR_OK;
 
  /* create new tcp pcb */
  client_pcb = tcp_new();
//  client_pcb->so_options |= SOF_KEEPALIVE;
 #if 0
  client_pcb->keep_idle = 50000;	   // ms
  client_pcb->keep_intvl = 20000;	   // ms
  client_pcb->keep_cnt = 5; 
 #endif	   
  if (client_pcb != NULL)
  {
 
   
    IP4_ADDR( &DestIPaddr, *destip, *(destip+1),*(destip+2), *(destip+3) );
    /* connect to destination address/port */
    err = tcp_connect(client_pcb,&DestIPaddr,port,tcp_client_connected);
//    tcp_err(client_pcb,tcp_errf);   
  }
  return err;
  
}
#if 1
static void tcp_errf(void *arg,err_t err){
    printf("\r\ntcp_errf be called...\r\n");
    if(es == NULL){
        es = (struct client *)mem_malloc(sizeof(struct client));
        es = (struct client *)arg;
    }
    if(err == ERR_OK ){
          /* No error, everything OK. */
          return ;
     }   
    switch(err)
    { 
        case ERR_MEM:                                            /* Out of memory error.     */
            printf("\r\n ERR_MEM   \r\n");
            break;  
        case ERR_BUF:                                            /* Buffer error.            */
            printf("\r\n ERR_BUF   \r\n");
            break;
        case ERR_TIMEOUT:                                       /* Timeout.                 */
            printf("\r\n ERR_TIMEOUT   \r\n");
            break;
        case ERR_RTE:                                            /* Routing problem.         */      
            printf("\r\n ERR_RTE   \r\n");
            break;
        case ERR_ISCONN:                                          /* Already connected.       */
            printf("\r\n ERR_ISCONN   \r\n");
            break;
        case ERR_ABRT:                                           /* Connection aborted.      */
            printf("\r\n ERR_ABRT   \r\n");
            break;
        case ERR_RST:                                            /* Connection reset.        */     
            printf("\r\n ERR_RST   \r\n");
            break;
        case ERR_CONN:                                           /* Not connected.           */
            printf("\r\n ERR_CONN   \r\n");
            break;
        case ERR_CLSD:                
            printf("\r\n ERR_CLSD   \r\n");
            break;
        case ERR_VAL:                                            /* Illegal value.           */
           printf("\r\n ERR_VAL   \r\n");
           return;
        case ERR_ARG:                                            /* Illegal argument.        */
            printf("\r\n ERR_ARG   \r\n");
            return;
        case ERR_USE:                                            /* Address in use.          */
           printf("\r\n ERR_USE   \r\n");
           return; 
        case ERR_IF:                                             /* Low-level netif error    */
            printf("\r\n ERR_IF   \r\n");
            break;
        case ERR_INPROGRESS:                                     /* Operation in progress    */
            printf("\r\n ERR_INPROGRESS   \r\n");
            break;
    }
    es->state  = ES_CLOSING;
}

#endif


static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
  printf("tcp_client_recv_ERROR:%d\r\n",err);
  struct client *es;
  err_t ret_err;
  LWIP_ASSERT("arg != NULL",arg != NULL);
  es = (struct client *)arg;
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p_tx == NULL)
    {
       /* we're done sending, close connection */
       tcp_client_connection_close(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_CONNECTED)
  {  
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);                   //??¨¨?p?D¦Ì?¨ºy?Y?¡ê
 
    memset(recevRxBufferTcpEth,0x00,TCPREVDATALEN);
    printf("recvlen total :%d\r\n",p->tot_len);
    if(p->len > TCPREVDATALEN){    
        p->len= TCPREVDATALEN;    
    }
    struct pbuf *a1 = p;
    struct pbuf *a2 = p;
    int offset = 0;
    while (a1 != NULL) {

        memcpy(recevRxBufferTcpEth + offset,a1->payload,a1->len);
           printf("recvlen:%d\r\n",a1->len);
        offset += a1->len;
      a1 = a1->next;
      
    }
  
  //  memcpy(&recevRxBufferTcpEth,p->payload,p->len);
 
    tcpRecevieFlag++;                    //??tcp¨¦¨¨???a1.?¦Ì?¡Â?¨®¨º?¦Ì?¨¢?¡¤t????¦Ì?¨ºy?Y?¡ê
    pbuf_free(p);                   
    ret_err = ERR_OK;   
  }
  /* data received when connection already closed */
  
    else
    {
      /* Acknowledge data reception */
      tcp_recved(tpcb, p->tot_len);
      
      /* free pbuf and do nothing */
      pbuf_free(p);
      ret_err = ERR_OK;
    }
    return ret_err;
  }


void tcp_client_send(struct tcp_pcb *tpcb, struct client * es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK; 
  while ((wr_err == ERR_OK) &&
         (es->p_tx != NULL) && 
         (es->p_tx->len <= tcp_sndbuf(tpcb)))
  {
    /* get pointer on pbuf from es structure */
    ptr = es->p_tx;
 
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK)
    { 
      /* continue with next pbuf in chain (if any) */
      es->p_tx = ptr->next;
      
      if(es->p_tx != NULL)
      {
      /* increment reference count for es->p */
        pbuf_ref(es->p_tx);
      }
      pbuf_free(ptr);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later, defer to poll */
     es->p_tx = ptr;                                                    
   }
   else
   {
     es->p_tx = ptr;    
     /* other problem ?? */
   }
  }
}


u8_t tcp_send_message(void *msg, u16_t len){
    u8_t  count = 0;
    struct pbuf *p;  
    if(es->state != ES_CONNECTED)  return -1;
    if(es->p_tx == NULL){
        
          es->p_tx  = pbuf_alloc(PBUF_TRANSPORT,len,PBUF_RAM);          
          pbuf_take( es->p_tx , (char*)msg, len);
    }
    tcp_client_send(es->pcb,es);
    return 1;
}


static void tcp_err_close()
{
  /* remove callbacks */
  tcp_recv(es->pcb, NULL);
  
  if (es != NULL)
  {
  	if(es->p_tx != NULL){
		pbuf_free(es->p_tx);
  	}
	if (es->pcb != NULL) {
		 tcp_close(es->pcb);
	}
    mem_free(es);
  }
}


static void tcp_client_connection_close()
{
  /* remove callbacks */
  tcp_recv(es->pcb, NULL);
  if (es != NULL)
  {
  	if(es->p_tx != NULL){
		pbuf_free(es->p_tx);
  	}
	if (es->pcb != NULL) {
		  /* close tcp connection */
		 tcp_close(es->pcb);
	}
    mem_free(es);
    es = NULL;
  }
}

long lwip_tcpsend()
{
    char *msgtest="31123134342342342342342342342";
    tcp_send_message(msgtest,strlen(msgtest));    
}

int lwipTcpClient(void *param)
{


    int count = 0;
    u8_t rxbuf[1024] = {0};
    char ip[] = {58,60,184,213};
    printf("test 12345\n");
    tcp_client_connect(ip,12001);
    while(1)
    {
      sys_msleep(100);  
      if(tcpRecevieFlag)
        {
          tcp_send_message(recevRxBufferTcpEth, strlen(recevRxBufferTcpEth));
          tcpRecevieFlag--;
        }
    }
}

