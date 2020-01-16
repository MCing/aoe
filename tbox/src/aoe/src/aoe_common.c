#include <stdio.h>
#include <strings.h>
#include "parson.h"
#include "aoe_common.h"
#include "at.h"
#include "tbox/tbox.h"


#define CONFIG_FILE "option.json"

#define JSON_KW_UART			 "uart"
#define JSON_KW_UART_PORT 		 "port"
#define JSON_KW_UART_BAUD 		 "baud"

#define JSON_KW_PPP 			"ppp"
#define JSON_KW_PPP_AUTHTYPE 	"authtype"
#define JSON_KW_PPP_USER 		"user"
#define JSON_KW_PPP_PASSWORD 	"password"
#define JSON_KW_PPP_REMOTEIP	"remoteip"
#define JSON_KW_PPP_LOCALIP 	"localip"
#define JSON_KW_PPP_APN 		"apn"
//#define JSON_KW_PPP_ECHO_ADPT 	"lcp_echo_adaptive"
#define JSON_KW_PPP_ECHO_INTERVAL 	"lcp_echo_interval"
#define JSON_KW_PPP_ECHO_FAILS 		"lcp_echo_fails"

#define JSON_KW_LOG 				"debug log"
#define JSON_KW_LOG_PPP_DEBUG 		"ppp_debug"
#define JSON_KW_LOG_TCPIP_DEBUG 	"tcpip_debug"
#define JSON_KW_LOG_TRACE_DEBUG 	"trace_debug"
#define JSON_KW_LOG_TO_FILE 	    "log_to_file"
#define JSON_KW_LOG_PCAP_FILE	    "pcap_file"
#define JSON_KW_LOG_PPP_DUMP	    "ppp_frame_dump"







static UART_OPT uart_option = {"COM19", 115200};
static PPP_OPT ppp_option = { \
	 1, 
	 "user", 
	 "pwd", 
	 "", 
	 "", 
	 "cmnet",
	 0,   //LCP_ECHOINTERVAL
	 3,   //LCP_MAXECHOFAILS
	 };

static LOG_OPT log_option = {\
	0,
	0,
	0,
	0,
	0
	};

int aoe_get_uart_portno()
{
	int portno = 0;

	if(strncasecmp(uart_option.port_name, "COM", 3) == 0)
	{
		sscanf(uart_option.port_name + 3, "%d", &portno);
	}
	
	return portno;
}

UART_OPT *aoe_get_uart_opt()
{
	return &uart_option;
}

PPP_OPT *aoe_get_ppp_opt()
{
	return &ppp_option;
}

LOG_OPT *aoe_get_log_opt()
{
	return &log_option;
}


void test_print_options()
{
	printf("uart options:\n");
	printf("port_name:%s\n", uart_option.port_name);
	printf("baud:%d\n", uart_option.baud);

	printf("ppp options:\n");
	printf("auth_type:%d\n", ppp_option.auth_type);
	printf("user:%s\n", ppp_option.user);
	printf("pwd:%s\n", ppp_option.pwd);
	printf("remote_ip:%s\n", ppp_option.remote_ip);
	printf("local_ip:%s\n", ppp_option.local_ip);
	printf("apn:%s\n", ppp_option.apn);

}



int parse_uart_configuration(char *conf_file, int uartno)
{
	
	const char conf_obj[10] = {0};
	JSON_Value *root_val;
	JSON_Object *root = NULL;
	JSON_Object *conf = NULL;
	JSON_Value *val;
	int result = 0;

	if(uartno == 0)
		snprintf(conf_obj, sizeof(conf_obj), "%s", JSON_KW_UART);
	else
		snprintf(conf_obj, sizeof(conf_obj), "%s%d", JSON_KW_UART, uartno);
	do
	{
		/* try to parse JSON */
		root_val = json_parse_file_with_comments(conf_file);
		root = json_value_get_object(root_val);
		if (root == NULL) {
			printf("ERROR: %s id not a valid JSON file\n", conf_file);
			result = -1;
			break;
		}
		conf = json_object_get_object(root, conf_obj);
		if (conf == NULL) {
			printf("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj);
			result = -1;
			break;
		} else {
			printf("INFO: %s does contain a JSON object named %s, parsing parameters\n", conf_file, conf_obj);
		}

		val = json_object_get_value(conf, JSON_KW_UART_PORT); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONString) 
		{
			printf("INFO: no configuration for port name\n");
		}
		else
		{
			printf("INFO: port name:%s\n", json_value_get_string(val));
			memset(uart_option.port_name, 0, sizeof(uart_option.port_name));
			strcpy(uart_option.port_name,  json_value_get_string(val));
		}

		val = json_object_get_value(conf, JSON_KW_UART_BAUD); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for baud\n");
		}
		else
		{
			printf("INFO: baud:%d\n", (int)json_value_get_number(val));
			uart_option.baud = (int)json_value_get_number(val);
		}
	}while(0);

	if(root_val != NULL)
		json_value_free(root_val);
	
	return result;
}




int parse_ppp_configuration(char *conf_file)
{
	
	const char conf_obj[10] = JSON_KW_PPP;
	JSON_Value *root_val;
	JSON_Object *root = NULL;
	JSON_Object *conf = NULL;
	JSON_Value *val;
	int result = 0;

	do
	{
		/* try to parse JSON */
		root_val = json_parse_file_with_comments(conf_file);
		root = json_value_get_object(root_val);
		if (root == NULL) {
			printf("ERROR: %s id not a valid JSON file\n", conf_file);
			result = -1;
			break;
		}
		conf = json_object_get_object(root, conf_obj);
		if (conf == NULL) {
			printf("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj);
			result = -1;
			break;
		} else {
			printf("INFO: %s does contain a JSON object named %s, parsing parameters\n", conf_file, conf_obj);
		}

		val = json_object_get_value(conf, JSON_KW_PPP_AUTHTYPE); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_AUTHTYPE\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_AUTHTYPE:%d\n", (int)json_value_get_number(val));
			ppp_option.auth_type = (int)json_value_get_number(val);
		}

		val = json_object_get_value(conf, JSON_KW_PPP_USER); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONString) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_USER\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_USER:%s\n", json_value_get_string(val));
			memset(ppp_option.user, 0, sizeof(ppp_option.user));
			strcpy(ppp_option.user,  json_value_get_string(val));
		}

		val = json_object_get_value(conf, JSON_KW_PPP_PASSWORD); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONString) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_PASSWORD\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_PASSWORD:%s\n", json_value_get_string(val));
			memset(ppp_option.pwd, 0, sizeof(ppp_option.pwd));
			strcpy(ppp_option.pwd,  json_value_get_string(val));
		}

		val = json_object_get_value(conf, JSON_KW_PPP_REMOTEIP); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONString) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_REMOTEIP\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_REMOTEIP:%s\n", json_value_get_string(val));
			memset(ppp_option.remote_ip, 0, sizeof(ppp_option.remote_ip));
			strcpy(ppp_option.remote_ip,  json_value_get_string(val));
		}

		val = json_object_get_value(conf, JSON_KW_PPP_LOCALIP); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONString) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_LOCALIP\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_LOCALIP:%s\n", json_value_get_string(val));
			memset(ppp_option.local_ip, 0, sizeof(ppp_option.local_ip));
			strcpy(ppp_option.local_ip,  json_value_get_string(val));
		}

		val = json_object_get_value(conf, JSON_KW_PPP_APN); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONString) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_APN\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_APN:%s\n", json_value_get_string(val));
			memset(ppp_option.apn, 0, sizeof(ppp_option.apn));
			strcpy(ppp_option.apn,  json_value_get_string(val));
		}

		val = json_object_get_value(conf, JSON_KW_PPP_ECHO_INTERVAL); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_ECHO_INTERVAL\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_ECHO_INTERVAL:%d\n", (int)json_value_get_number(val));
			ppp_option.lcp_echo_interval = (int)json_value_get_number(val);
		}

		val = json_object_get_value(conf, JSON_KW_PPP_ECHO_FAILS); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_PPP_ECHO_FAILS\n");
		}
		else
		{
			printf("INFO: JSON_KW_PPP_ECHO_FAILS:%d\n", (int)json_value_get_number(val));
			ppp_option.lcp_echo_fails = (int)json_value_get_number(val);
		}
	}while(0);
	
	if(root_val)
		json_value_free(root_val);
	
	return result;
}


int parse_log_configuration(char *conf_file)
{
	
	const char conf_obj[] = JSON_KW_LOG;
	JSON_Value *root_val;
	JSON_Object *root = NULL;
	JSON_Object *conf = NULL;
	JSON_Value *val;
	int result = 0;

	do
	{
		/* try to parse JSON */
		root_val = json_parse_file_with_comments(conf_file);
		root = json_value_get_object(root_val);
		if (root == NULL) {
			printf("ERROR: %s id not a valid JSON file\n", conf_file);
			result = -1;
			break;
		}
		conf = json_object_get_object(root, conf_obj);
		if (conf == NULL) {
			printf("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj);
			result = -1;
			break;
		} else {
			printf("INFO: %s does contain a JSON object named %s, parsing parameters\n", conf_file, conf_obj);
		}

		val = json_object_get_value(conf, JSON_KW_LOG_PPP_DEBUG); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_LOG_PPP_DEBUG\n");
		}
		else
		{
			printf("INFO: JSON_KW_LOG_PPP_DEBUG:%d\n", (int)json_value_get_number(val));
			log_option.ppp_debug = (int)json_value_get_number(val);
		}

		val = json_object_get_value(conf, JSON_KW_LOG_TCPIP_DEBUG); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_LOG_TCPIP_DEBUG\n");
		}
		else
		{
			printf("INFO: JSON_KW_LOG_TCPIP_DEBUG:%d\n", (int)json_value_get_number(val));
			log_option.tcp_ip_debug = (int)json_value_get_number(val);
		}

		val = json_object_get_value(conf, JSON_KW_LOG_TRACE_DEBUG); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_LOG_TRACE_DEBUG\n");
		}
		else
		{
			printf("INFO: JSON_KW_LOG_TRACE_DEBUG:%d\n", (int)json_value_get_number(val));
			log_option.trace_debug = (int)json_value_get_number(val);
		}

		val = json_object_get_value(conf, JSON_KW_LOG_TO_FILE); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_LOG_TO_FILE\n");
		}
		else
		{
			printf("INFO: JSON_KW_LOG_TO_FILE:%d\n", (int)json_value_get_number(val));
			log_option.log_to_file = (int)json_value_get_number(val);
		}

		
		val = json_object_get_value(conf, JSON_KW_LOG_PCAP_FILE); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_LOG_PCAP_FILE\n");
		}
		else
		{
			printf("INFO: JSON_KW_LOG_PCAP_FILE:%d\n", (int)json_value_get_number(val));
			log_option.pcap_file = (int)json_value_get_number(val);
		}

		val = json_object_get_value(conf, JSON_KW_LOG_PPP_DUMP); /* fetch value (if possible) */
		if (json_value_get_type(val) != JSONNumber) 
		{
			printf("INFO: no configuration for JSON_KW_LOG_PPP_DUMP\n");
		}
		else
		{
			printf("INFO: JSON_KW_LOG_PPP_DUMP:%d\n", (int)json_value_get_number(val));
			log_option.ppp_frame_dump = (int)json_value_get_number(val);
		}
	

	}while(0);
	
	if(root_val)
		json_value_free(root_val);
	
	return result;
}



void save_config()
{
	const char *filename = CONFIG_FILE;
	JSON_Value *root = NULL;
	JSON_Value *uart = NULL;
	JSON_Value *ppp = NULL;
	JSON_Value *log = NULL;
	
	JSON_Object *obj = NULL;
	
	

	//uart config
	uart = json_value_init_object();
	obj = json_value_get_object(uart);	
	json_object_set_string(obj, JSON_KW_UART_PORT, uart_option.port_name);
	json_object_set_number(obj, JSON_KW_UART_BAUD, uart_option.baud);

	//ppp config
	ppp = json_value_init_object();
	obj = json_value_get_object(ppp);
	json_object_set_number(obj, JSON_KW_PPP_AUTHTYPE, ppp_option.auth_type);
	json_object_set_string(obj, JSON_KW_PPP_USER, ppp_option.user);
	json_object_set_string(obj, JSON_KW_PPP_PASSWORD, ppp_option.pwd);
	json_object_set_string(obj, JSON_KW_PPP_REMOTEIP, ppp_option.remote_ip);
	json_object_set_string(obj, JSON_KW_PPP_LOCALIP, ppp_option.local_ip);
	json_object_set_string(obj, JSON_KW_PPP_APN, ppp_option.apn);
	json_object_set_number(obj, JSON_KW_PPP_ECHO_INTERVAL, ppp_option.lcp_echo_interval);
	json_object_set_number(obj, JSON_KW_PPP_ECHO_FAILS, ppp_option.lcp_echo_fails);


	//log config
	log = json_value_init_object();
	obj = json_value_get_object(log);
	json_object_set_number(obj, JSON_KW_LOG_PPP_DEBUG, log_option.ppp_debug);
	json_object_set_number(obj, JSON_KW_LOG_TCPIP_DEBUG, log_option.tcp_ip_debug);
	json_object_set_number(obj, JSON_KW_LOG_TRACE_DEBUG, log_option.trace_debug);
	json_object_set_number(obj, JSON_KW_LOG_TO_FILE, log_option.log_to_file);
	json_object_set_number(obj, JSON_KW_LOG_PCAP_FILE, log_option.pcap_file);
	json_object_set_number(obj, JSON_KW_LOG_PPP_DUMP, log_option.ppp_frame_dump);
	

	//all in
	root = json_value_init_object();
	obj = json_value_get_object(root);
	json_object_set_value(obj, JSON_KW_UART, uart);
	json_object_set_value(obj, JSON_KW_PPP, ppp);
	json_object_set_value(obj, JSON_KW_LOG, log);

	json_serialize_to_file_pretty(root, filename);

}

void aoe_init_configuration()
{
	int result = 0;
	result += parse_uart_configuration(CONFIG_FILE, 0);
	result += parse_ppp_configuration(CONFIG_FILE);
	result += parse_log_configuration(CONFIG_FILE);

	printf("aoe_init_configuration result:%d\n", result);
	if(result)
		save_config();

}



//ppp
void exit_ppp_mode()
{
	at_client_get_first()->device->state = UART_AT_CLIENT;

}

void exit_at_debug_mode()
{
	at_client_get_first()->device->state = UART_AT_CLIENT;

}


int enter_ppp_mode()
{
	at_client_get_first()->device->state = UART_PPP;
	return 0;
}

int  enter_at_debug_mode()
{
	at_client_get_first()->device->state = UART_AT_DEBUG;
	return 0;
}

int ppp_at_dial()
{
	int result = -1;
	at_response_t resp = NULL;
	
	resp = at_create_resp(128, 0, 5000);
	
	if (!resp)
	{
		tb_trace_i("No memory for response structure!");
		result = -1;
		goto exit;
	}

	if(at_client_wait_connect(10000))
	{
		tb_trace_i("at_client_wait_connect fail");
		result = -1;
		goto exit;
	}

	if (at_exec_cmd(resp, "ATD*99#") != RT_EOK)
	{
		tb_trace_i("AT client send commands failed, response error or timeout !");
		result = -1;
		goto exit;
	}

	for(int i = 0; i < resp->line_counts; i++)
	{
		char *tmp_buf = NULL;
		tmp_buf = at_resp_get_line(resp, i+1);
		tb_trace_i("%s", tmp_buf);
		if(tb_strstr(tmp_buf, "CONNECT"))
		{
			result = 0;
			goto exit;
		}
	}

	exit:
	if(resp != NULL)
		at_delete_resp(resp);

	return result;


}

int ppp_mode_inused()
{
	return at_client_get_first()->device->state == UART_PPP;
}


long switch_ppp_cmd_mode()
{
	char *ecs_str = "+++";
	at_client_send(ecs_str, strlen(ecs_str));
	exit_ppp_mode();
}

long switch_ppp_data_mode()
{
	int result = -1;
	at_response_t resp = NULL;
	resp = at_create_resp(128, 0, 2000);
	
	if (!resp)
	{
		tb_trace_i("No memory for response structure!");
		result = -1;
		goto exit;
	}
	tb_trace_i("at_create_resp ok");

	if (at_exec_cmd(resp, "ATO") != RT_EOK)
	{
		tb_trace_i("AT client send commands failed, response error or timeout !");
		result = -1;
		goto exit;
	}
	//tb_trace_i("resp->line_counts:%d", resp->line_counts);


	for(int i = 0; i < resp->line_counts; i++)
	{
		char *tmp_buf = NULL;
		tmp_buf = at_resp_get_line(resp, i+1);
		tb_trace_i("%s", tmp_buf);
		if(tb_strstr(tmp_buf, "CONNECT"))
		{
			result = 0;
			goto exit;
		}
	}

	exit:
	if(resp != NULL)
		at_delete_resp(resp);
	
	if(result == 0)
		enter_ppp_mode();
	
	return result;


}


void aoe_init_uart(void)
{
	UART_OPT *uart = aoe_get_uart_opt();

	if(rt_init_device(uart->port_name, uart->baud, "8N1N"))
	{
		tb_trace_i("rt_init_device fail");
	}
	else
	{
		tb_trace_i("rt_init_device success");
		at_client_init(uart->port_name, 2048);
	}
}

void aoe_close_uart(void)
{
	UART_OPT *uart = aoe_get_uart_opt();

	at_client_t client = at_client_get_first();
	
	if(!rt_find_device(uart->port_name))
	{
		tb_trace_i("rt_find_device fail");
	}
	else
	{
		tb_trace_i("rt_find_device success");
		if(client != NULL)
		{
			tb_trace_i("rt_device_close: %d", rt_device_close(client->device));
			at_client_release(client);
		}
	}
}



void aoe_init_log_config()
{
	#define LOG_HEAD "aoe_log_"
	#define LOG_EXT_NAME ".log"
	char log_to_file = aoe_get_log_opt()->log_to_file;
	tb_tm_t mtime = {0};
    tb_localtime(tb_time(), &mtime);
	
	if(log_to_file)
	{
		tb_char_t path[256] = {0};
		tb_snprintf(path, sizeof(path), "%s%04ld%02ld%02ld_%02ld%02ld%02ld%s",LOG_HEAD,  mtime.year
            , mtime.month
            , mtime.mday
            , mtime.hour
            , mtime.minute
            , mtime.second
            ,LOG_EXT_NAME
            );
		tb_trace_mode_set(TB_TRACE_MODE_FILE);
		tb_trace_file_set_path(path, tb_true);
	}
	else
	{
		tb_trace_mode_set(TB_TRACE_MODE_PRINT);
	}
	
}


/*---------pcap----------*/
struct pcap_file_header {
	unsigned int 	magic;
	unsigned short 	version_major;
	unsigned short 	version_minor;
	int 			thiszone;     /* gmt to local correction */
	unsigned int 	sigfigs;    /* accuracy of timestamps */
	unsigned int 	snaplen;    /* max length saved portion of each pkt */
	unsigned int 	linktype;   /* data link type (LINKTYPE_*) */
};

struct timeval_d {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};
struct pcap_pkthdr {
	struct timeval_d ts;      /* time stamp */
	unsigned int caplen;     /* length of portion present */
	unsigned int len;        /* length this packet (off wire) */
};

#define PCAP_FRAME_MAX 2048
#define PCAP_QUEUE_SIZE 1024
typedef unsigned char ext_accm[32];

typedef struct
{
	ext_accm accm;
	int frame_len;
	tb_timeval_t tv;
	unsigned char ppp_frame[PCAP_FRAME_MAX];
	char dir;
}pcap_packet_t;



tb_queue_ref_t pcap_queue = NULL;
tb_file_ref_t pcap_file = NULL;
tb_mutex_ref_t pcap_queue_mutex = NULL;
tb_thread_ref_t pcap_file_thread = NULL;

void aoe_pcap_queue_put(unsigned char *data, int data_len, char dir,ext_accm accm)
{

	if(aoe_get_log_opt()->ppp_frame_dump)
	{
		char ppp_dump[4096] = {0};
		if(dir == 0)
			tb_trace_i("------ppp out dump------");
		else
			tb_trace_i("------ppp in dump------");
	
		for(int i = 0; i < data_len; i++)
			snprintf(ppp_dump + i, sizeof(ppp_dump) - i, "%02X", data[i]);
		tb_trace_i("dump:%s", ppp_dump);
		
	}

	if(!pcap_queue)
		return;
	
	pcap_packet_t new_packet;
	new_packet.dir = dir;
	new_packet.frame_len = data_len;
	tb_gettimeofday(&(new_packet.tv), tb_null);
	
	memcpy(new_packet.accm, accm, sizeof(ext_accm));
	
	if(data_len > PCAP_FRAME_MAX)
	{
		new_packet.frame_len = PCAP_FRAME_MAX;
	}
	memcpy(new_packet.ppp_frame, data, new_packet.frame_len);
	tb_mutex_enter(pcap_queue_mutex);
	tb_queue_put(pcap_queue, (tb_cpointer_t)&new_packet);
	tb_mutex_leave(pcap_queue_mutex);
	//tb_trace_i("aoe_pcap_queue_put size:%d", tb_queue_size(pcap_queue));
}
#if 1
unsigned char tmp_store_buf[2048] = {0};
int tmp_store_offset = 0;
static int aoe_convert_ppp_data(const unsigned char *data, int data_len, unsigned char *out_buf, int *out_len, ext_accm accm)
{

	#define ESCAPE_P(accm, c) ((accm)[(c) >> 3] & 1 << (c & 0x07))
	#define	PPP_ESCAPE	0x7d	/* Asynchronous Control Escape */
	#define	PPP_TRANS	0x20	/* Asynchronous transparency modifier */
	
	unsigned char *p_data = NULL;
	int len = data_len;
	unsigned char in_escaped = 0;
	unsigned char escaped = 0;
	unsigned char cur_char;
	int tmp_out_len = 0;
	if(!data)
		return -1;

	
	p_data = data;
	if(*p_data == 0x7E)
	{
		p_data++;
		len -= 1;
	}

	if(*(p_data+len-1) == 0x7E)
	{
		len -= 1;
		if(tmp_store_offset != 0)
		{
			memcpy(tmp_store_buf + tmp_store_offset, p_data, len);
			tmp_store_offset += len;
			len = tmp_store_offset;
			p_data = tmp_store_buf;
		}
		
	}
	else
	{	
		memcpy(tmp_store_buf + tmp_store_offset, p_data, len);
		tmp_store_offset += len;
		//printf("\n***** no end tmp_store_offset:%d***********\n", tmp_store_offset);
		return -1;
	}
	while(len--)
	{
		cur_char = *p_data++;
		escaped = ESCAPE_P(accm, cur_char);
		
		if (escaped && (cur_char == PPP_ESCAPE))
		{
			in_escaped = 1;
			continue;
		}
		if(in_escaped)
		{
			cur_char ^= PPP_TRANS;
			in_escaped = 0;
		}
		*out_buf++ = cur_char;
		tmp_out_len++;
		if(tmp_out_len >= out_len)
		{
			printf("aoe_convert_ppp_data overflow\n");
			return -1;
		}
	}
	*out_len = tmp_out_len;
	//printf("\nin len:%d out len:%d\n", data_len, *out_len);
	memset(tmp_store_buf, 0, sizeof(tmp_store_buf));
	tmp_store_offset = 0;
	return 0;

}
#endif
void aoe_pcap_save_packet(pcap_packet_t *packet)
{
	
	tb_byte_t *packet_data = packet->ppp_frame;
	unsigned int packet_len = packet->frame_len;
	struct pcap_pkthdr pkt_header;
	pkt_header.ts.tv_sec = packet->tv.tv_sec;
	pkt_header.ts.tv_usec = packet->tv.tv_usec;
	tb_byte_t conver_packet_data[2048] = {0};
	int conver_len = sizeof(conver_packet_data);
	tb_byte_t write_data[2048] = {0};
	char dir = packet->dir;
#if 1
	if(aoe_convert_ppp_data(packet->ppp_frame, packet->frame_len, conver_packet_data, &conver_len, packet->accm))
	{
		//printf("\n**********store in********\n");
		return ;
	}
	pkt_header.caplen = conver_len + 1;
	pkt_header.len = conver_len + 1;
	
	memcpy(write_data, &pkt_header, sizeof(struct pcap_pkthdr));
	memcpy(write_data + sizeof(struct pcap_pkthdr), &dir, 1);
	memcpy(write_data + sizeof(struct pcap_pkthdr) + 1, conver_packet_data, conver_len);

	if(pcap_file)
	{
		tb_long_t wb = tb_file_writ(pcap_file, write_data, 1 + sizeof(struct pcap_pkthdr) + conver_len);
		//printf("sizeof(pcap_pkthdr):%d sizeof(packet_data):%d wb:%d\n", sizeof(struct pcap_pkthdr), sizeof(packet_data), wb);
	}

#else
	if(*packet_data == 0x7E)
	{
		packet_data++;
		packet_len -= 1;
	}

	pkt_header.caplen = packet_len + 1;
	pkt_header.len = packet_len + 1;
	
	memcpy(write_data, &pkt_header, sizeof(struct pcap_pkthdr));
	memcpy(write_data + sizeof(struct pcap_pkthdr), &dir, 1);
	memcpy(write_data + sizeof(struct pcap_pkthdr) + 1, conver_packet_data, conver_len);
	if(pcap_file)
	{
		tb_long_t wb = tb_file_writ(pcap_file, write_data, 1 + sizeof(struct pcap_pkthdr) + packet_len);
		//printf("sizeof(pcap_pkthdr):%d sizeof(packet_data):%d wb:%d\n", sizeof(struct pcap_pkthdr), sizeof(packet_data), wb);
	}
#endif
	
	
}


void aoe_pcap_queue_get()
{
	
	if(pcap_queue && tb_queue_size(pcap_queue) > 0)
    {
    	pcap_packet_t head_packet;
		memset(&head_packet, 0, sizeof(head_packet));
    	tb_mutex_enter(pcap_queue_mutex);
    	pcap_packet_t *new_packet = (pcap_packet_t *)tb_queue_get(pcap_queue);
		memcpy(&head_packet, new_packet, sizeof(pcap_packet_t));
		tb_queue_pop(pcap_queue);
		tb_mutex_leave(pcap_queue_mutex);
   		//tb_trace_i("aoe_pcap_queue_get size:%d", tb_queue_size(pcap_queue));

		aoe_pcap_save_packet(&head_packet);
    }

}

int aoe_pcap_file_init()
{
	#define PCAP_LOG_HEAD "aoe_log_"
	#define PCAP_LOG_EXT_NAME ".pcap"
	
	tb_tm_t mtime = {0};
    tb_localtime(tb_time(), &mtime);


	tb_char_t path[256] = {0};
	tb_snprintf(path, sizeof(path), "%s%04ld%02ld%02ld_%02ld%02ld%02ld%s",PCAP_LOG_HEAD,  mtime.year
        , mtime.month
        , mtime.mday
        , mtime.hour
        , mtime.minute
        , mtime.second
        ,PCAP_LOG_EXT_NAME
        );
	
	pcap_file = tb_file_init(path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	if(pcap_file)
		tb_trace_i("init file sucess");
	else
	{
		tb_trace_i("init file fail");
		return -1;
	}

	
	
	//write pcap header
	struct pcap_file_header pcap_header;

	pcap_header.magic = 0xa1b2c3d4;//0xd4c3b2a1;
	pcap_header.version_major = 2;
	pcap_header.version_minor = 4;
	pcap_header.thiszone = 0;     
	pcap_header.sigfigs = 0;
	pcap_header.snaplen = 8192;
	pcap_header.linktype = 204; //todo

	tb_long_t wb = tb_file_writ(pcap_file, (tb_byte_t *)&pcap_header, sizeof(struct pcap_file_header));
	return 0;
}

int pcap_handle_thread(void *data)
{
	while(1)
	{
		aoe_pcap_queue_get();
		tb_msleep(10);
	}
	return 0;
}
int aoe_pcap_init()
{
	char pcap_file = aoe_get_log_opt()->pcap_file;

	if(!pcap_file)
	{
		tb_trace_i("pcap_file disable");
		return 0;
	}
	
	if(!pcap_queue_mutex)
		 pcap_queue_mutex = tb_mutex_init();

	if(!pcap_queue_mutex)
	{
		tb_trace_i("aoe_pcap_init init mutex fail ");
		return -1;
	}

	if(aoe_pcap_file_init())
	{
		tb_trace_i("aoe_pcap_init init aoe_pcap_file_init fail ");
		return -1;
	}

	pcap_queue = tb_queue_init(PCAP_QUEUE_SIZE, tb_element_mem(sizeof(pcap_packet_t), NULL, NULL));
	if(!pcap_queue)
	{
		tb_trace_i("aoe_pcap_init init pcap_queue fail ");
		return -1;
	}

	pcap_file_thread = tb_thread_init("pcap_log", pcap_handle_thread, NULL, 0);
	if(!pcap_file_thread)
	{
		tb_trace_i("aoe_pcap_init init thread fail ");
		return -1;
	}

	return 0;
}




int aoe_test(int argc, char **argv)
{
    tb_trace_i("sizeof(struct timeval_d):%d sizeof(tb_timeval_t):%d ", sizeof(struct timeval_d), sizeof(tb_timeval_t));
	ext_accm accm1 = {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	ext_accm accm2;
	memcpy(accm2, accm1, sizeof(accm1));
	tb_trace_i("sizeof accm:%d", sizeof(accm2));
	tb_trace_i("sizeof ext_accm:%d", sizeof(ext_accm));
	static int flag = 0;
	if(flag == 0)
	{
		printf("---aoe_pcap_queue_put dump accm---\n");
		for(int i = 0; i < 32; i++)
			printf("%02X", accm1[i]);
		printf("\n");
		for(int i = 0; i < 32; i++)
			printf("%02X", accm2[i]);
		printf("\n---dump accm---\n");
		//flag = 1;
	}

    return 0;
}

