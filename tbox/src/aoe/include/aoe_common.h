#ifndef _AOE_COMMON_H_
#define _AOE_COMMON_H_



typedef struct
{
	char port_name[10];
	int baud;
}UART_OPT;

typedef struct
{
	int auth_type;
	char user[128];
	char pwd[128];
	char remote_ip[16];
	char local_ip[16];
	char apn[128];
	//unsigned int lcp_echo_adaptive;
	unsigned char lcp_echo_interval;
	unsigned char lcp_echo_fails;
}PPP_OPT;

typedef struct
{
	char ppp_debug;
	char tcp_ip_debug;
	char trace_debug;
}LOG_OPT;



extern void aoe_init_configuration();
extern UART_OPT *aoe_get_uart_opt();
extern PPP_OPT *aoe_get_ppp_opt();
extern int aoe_get_uart_portno();
extern LOG_OPT *aoe_get_log_opt();



#define AOE_PPP_DEBUG (aoe_get_log_opt()->ppp_debug ? 0xff : 0)

#define AOE_TCPIP_DEBUG (aoe_get_log_opt()->tcp_ip_debug ? 0xff : 0)

#define AOE_TRACE_DEBUG (aoe_get_log_opt()->trace_debug ? 0xff : 0)


//uart management
typedef enum
{
	UART_IDLE = 0,
	UART_AT_CLIENT,
	UART_AT_DEBUG,
	UART_PPP,

	UART_MAX
}UART_APP_STATE;

//uart mode switch
extern int  enter_at_debug_mode();
extern void exit_at_debug_mode();


#endif

