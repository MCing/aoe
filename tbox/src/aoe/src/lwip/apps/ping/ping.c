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

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping receive timeout - in milliseconds */
#define PING_RCV_TIMEO 2
/** ping delay - in milliseconds */
#define PING_DELAY     1

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/* ping variables */
static u16_t ping_seq_num;
struct _ip_addr
{
    unsigned char addr0, addr1, addr2, addr3;
};

#define PING_THREAD_NAME               "ping_thread"
typedef struct
{
	
	char target_name[256];
	int ping_time;
	int ping_size;
}ping_param_t;
ping_param_t ping_opt;


/** Prepare a echo ICMP request */
static void ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++)
    {
        ((char*) iecho)[sizeof(struct icmp_echo_hdr) + i] = (char) i;
    }

      iecho->chksum = inet_chksum(iecho, len);

}

/* Ping using the socket ip */
err_t lwip_ping_send(int s, ip_addr_t *addr, int size)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    int ping_size = sizeof(struct icmp_echo_hdr) + size;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = malloc(ping_size);
    if (iecho == NULL)
    {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t) ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
#if LWIP_IPV4 && LWIP_IPV6
    to.sin_addr.s_addr = addr->u_addr.ip4.addr;
#elif LWIP_IPV4
    to.sin_addr.s_addr = addr->addr;
#elif LWIP_IPV6
#error Not supported IPv6.
#endif

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*) &to, sizeof(to));
    free(iecho);

    return (err == ping_size ? ERR_OK : ERR_VAL);
}

int lwip_ping_recv(int s, int *ttl)
{
    char buf[64];
    int fromlen = sizeof(struct sockaddr_in), len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &from, (socklen_t*) &fromlen)) > 0)
    {
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)))
        {
            iphdr = (struct ip_hdr *) buf;
            iecho = (struct icmp_echo_hdr *) (buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num)))
            {
                *ttl = iphdr->_ttl;
                return len;
            }
        }
    }

    return len;
}



/* using the lwIP custom ping */
long ping(char* target_name, unsigned long times, unsigned long size)
{
#if LWIP_VERSION_MAJOR >= 2U
    struct timeval timeout = { PING_RCV_TIMEO , PING_RCV_TIMEO % 1 };
#else
    int timeout = PING_RCV_TIMEO * 1000UL ;
#endif

    int s, ttl, recv_len;
    ip_addr_t target_addr;
    unsigned long  send_times;
    unsigned long  recv_start_tick;
    struct addrinfo hint, *res = NULL;
    struct sockaddr_in *h = NULL;
    struct in_addr ina;

    send_times = 0;
    ping_seq_num = 0;

    if (size == 0)
    {
        size = PING_DATA_SIZE;
    }

    memset(&hint, 0, sizeof(hint));
    /* convert URL to IP */
    if (lwip_getaddrinfo(target_name, NULL, &hint, &res) != 0)
    {
        printf("ping: unknown host %s\n", target_name);
        return -1;
    }
    memcpy(&h, &res->ai_addr, sizeof(struct sockaddr_in *));
    memcpy(&ina, &h->sin_addr, sizeof(ina));
    lwip_freeaddrinfo(res);
    if (inet_aton(inet_ntoa(ina), &target_addr) == 0)
    {
        printf("ping: unknown host %s\n", target_name);
        return -1;
    }
    /* new a socket */
    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0)
    {
        printf("ping: create socket failed\n");
        return -1;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	int max_time = 0;
	int min_time = 0xffff;
	long total_time_ms = 0;
	int timeout_count = 0;
    while (1)
    {
        int elapsed_time;

        if (lwip_ping_send(s, &target_addr, size) == ERR_OK)
        {
            recv_start_tick = sys_now();
            if ((recv_len = lwip_ping_recv(s, &ttl)) >= 0)
            {
                elapsed_time = (sys_now() - recv_start_tick);
                printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n", recv_len, inet_ntoa(ina), send_times + 1,
                        ttl, elapsed_time);
				total_time_ms += elapsed_time;
				min_time = (min_time > elapsed_time) ? elapsed_time : min_time;
				max_time = (max_time < elapsed_time) ? elapsed_time : max_time;
            }
            else
            {
                printf("From %s icmp_seq=%d timeout\n", inet_ntoa(ina), send_times + 1);
				timeout_count++;
            }
        }
        else
        {
            printf("Send %s - error\n", inet_ntoa(ina));
        }

        send_times++;
        if (send_times >= times)
        {
            /* send ping times reached, stop */
            break;
        }

        sys_msleep(100); /* take a delay */
    }
	//printf("\n--------------ping statistics----------------\n");
	printf("\n----------------\n min:%dms \n ave:%dms \n max:%dms \n timeout:%d times \n------------------\n",
	       min_time, total_time_ms/(send_times - timeout_count), max_time, timeout_count);
	//printf("\n---------------------------------------------\n");
	lwip_close(s);

    return 0;
}


void ping_thread(void *arg)
{
	if(!arg)
		return;
	
	ping_param_t *opt = (ping_param_t*)arg;
	
	ping(opt->target_name,  opt->ping_time, opt->ping_size);
}

#ifdef RT_USING_FINSH
#define DEFAULT_PING_TIME 4
#define DEFUALT_PING_SIZE 0

int cmd_ping(int argc, char **argv)
{
	
    if (argc == 1)
    {
        printf("Please input: ping <host address> [<times>[,<size>]]\n");
    }

	ping_opt.ping_time = DEFAULT_PING_TIME;
	ping_opt.ping_size = DEFUALT_PING_SIZE;
	
    if(argc >=2 )
    {
		memset(ping_opt.target_name, 0, sizeof(ping_opt.target_name));
		memcpy(ping_opt.target_name, argv[1], sizeof(ping_opt.target_name) - 1);
	}

	if(argc >=3)
		ping_opt.ping_time = atoi(argv[2]);

	if(argc >=4)
		ping_opt.ping_size = atoi(argv[3]);

	sys_thread_new(PING_THREAD_NAME, ping_thread, &ping_opt, 0, 0);

    return 0;
}
#endif /* RT_USING_FINSH */



