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


extern void aoe_init_configuration();
extern UART_OPT *aoe_get_uart_opt();
extern PPP_OPT *aoe_get_ppp_opt();
extern int aoe_get_uart_portno();



//uart management
typedef enum
{
	UART_IDLE = 0,
	UART_AT_CLIENT,
	UART_AT_DEBUG,
	UART_PPP,

	UART_MAX
}UART_APP_STATE;




#endif

