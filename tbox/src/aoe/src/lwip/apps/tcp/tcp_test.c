#include "aoe_config.h"

#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/mem.h>
#include <lwip/icmp.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/inet.h>
#include <lwip/inet_chksum.h>
#include <lwip/ip.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>




#define TCP_THREAD_NAME               "tcp_thread"
typedef struct
{
	
	char target_name[256];
	int port;
	int action;
}tcp_param_t;
tcp_param_t tcp_opt;


#include "lwip/tcp.h"
#define TCPREVDATALEN 4096

typedef enum{
	ES_CLOSED,
	ES_CONNECTING,
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

static u8_t tcp_send_message(void *msg, u16_t len);

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    printf("tcp_client_connected_ERROR:%d\r\n",err);
   
    
    switch(err){
        case ERR_OK:  
          es->pcb = tpcb;                                   
          es->p_tx = NULL;
          es->state = ES_CONNECTED;  
          tcp_arg(tpcb, es);                     
          tcp_recv(tpcb, tcp_client_recv);    
		  printf("\ntcp setup success\n");
          break ;
       case ERR_MEM :
           tcp_client_connection_close(tpcb, es);                
          break ;
       default :
           break ;
           
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
    tcp_recved(tpcb, p->tot_len);
 
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
        offset += a1->len;
      	a1 = a1->next;
      
    }
  
  //  memcpy(&recevRxBufferTcpEth,p->payload,p->len);
 
    tcpRecevieFlag++;
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


static void tcp_client_send(struct tcp_pcb *tpcb, struct client * es)
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


static u8_t tcp_send_message(void *msg, u16_t len){
    u8_t  count = 0;
    struct pbuf *p;  
    if(es->state != ES_CONNECTED)  return -1;
    if(es->p_tx == NULL){
        
          es->p_tx  = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);          
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
    es->state = ES_CLOSED;
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
    es->state = ES_CLOSED;
  }
}


int tcpsetup(char *target_name, int port)
{

	ip_addr_t target_addr;
	struct addrinfo hint, *res = NULL;
	struct sockaddr_in *h = NULL;
	struct in_addr ina;
	struct tcp_pcb *client_pcb;
	err_t err = ERR_OK;

	

	memset(&hint, 0, sizeof(hint));
	/* convert URL to IP */
	if (lwip_getaddrinfo(target_name, NULL, &hint, &res) != 0)
	{
		printf("tcp: unknown host %s\n", target_name);
		return -1;
	}
	memcpy(&h, &res->ai_addr, sizeof(struct sockaddr_in *));
	memcpy(&ina, &h->sin_addr, sizeof(ina));
	lwip_freeaddrinfo(res);
	if (inet_aton(inet_ntoa(ina), &target_addr) == 0)
	{
		printf("tcp: unknown host %s\n", target_name);
		return -1;
	}


	/* create new tcp pcb */
	client_pcb = tcp_new();
	if (client_pcb != NULL)
	{
		
		/* connect to destination address/port */
		err = tcp_connect(client_pcb, &target_addr, port, tcp_client_connected);
		if(err == ERR_OK)
		{
			if(es == NULL) 
				es = (struct client *)mem_malloc(sizeof(struct client));
			es->state = ES_CONNECTING;
		}
	}


	return err;
}





void tcp_thread(void *arg)
{
	if(!arg)
		return;
	
	tcp_param_t *opt = (tcp_param_t*)arg;
	if(tcpsetup(opt->target_name, opt->port) != ERR_OK)
		return;

	while(es->state != ES_CLOSED)
    {
       sys_msleep(100);  
       if(tcpRecevieFlag)
       {
       		if(opt->action == 1)
          		tcp_send_message(recevRxBufferTcpEth, strlen(recevRxBufferTcpEth));
			
          tcpRecevieFlag--;
       }
    }
	if(es != NULL)
		mem_free(es);
	es = NULL;
	printf("\nexit tcp_thread\n");
}


int cmd_tcp(int argc, char **argv)
{
	
    if (argc == 1)
    {
        printf("Please input: tcp <host address> <port> [<action>]\n");
		return -1;
    }

	tcp_opt.action = 0;
	
    if(argc >=2 )
    {
		memset(tcp_opt.target_name, 0, sizeof(tcp_opt.target_name));
		memcpy(tcp_opt.target_name, argv[1], sizeof(tcp_opt.target_name) - 1);
	}

	if(argc >=3)
		tcp_opt.port = atoi(argv[2]);

	if(argc >=4)
		tcp_opt.action = atoi(argv[3]);

	sys_thread_new(TCP_THREAD_NAME, tcp_thread, &tcp_opt, 0, 0);

    return 0;
}

unsigned char send_data[2048] = {0};
int hexstr2hex(char *s, char *d, int len)
{
	//todo
	memcpy(d, s, strlen(s));
	len = strlen(s);
	return len;
}
int cmd_tcp_send(int argc, char **argv)
{
	u16_t len = 0;
    if (argc != 2)
    {
        printf("Please input: tcp_send <data_in_hex>\n");
		return -1;
    }
	if(es && es->state == ES_CONNECTED)
	{
		len = hexstr2hex(argv[1], send_data, sizeof(send_data));
	    if(len > 0)
			tcp_send_message(send_data, len);
	}
	


    return 0;
}

int cmd_tcp_close()
{
	if(es && es->state == ES_CONNECTED)
	{
		tcp_client_connection_close();
	}
}



