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







static UART_OPT uart_option = {"COM19", 115200};
static PPP_OPT ppp_option = { \
	 1, 
	 "user", 
	 "pwd", 
	 "", 
	 "", 
	 "cmnet",
	 1,   //LCP_ECHOINTERVAL
	 3,   //LCP_MAXECHOFAILS
	 };

static LOG_OPT log_option = {\
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

tb_file_ref_t test_file = NULL;

void init_file(int argc, char **argv)
{
	if(argc != 2)
		return;
	char filename[256] = {0};
	strcat(filename, argv[1]);
	test_file = tb_file_init(filename, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_APPEND);
	if(test_file)
		printf("init file sucess");
	else
	{
		printf("init file fail");
		return;
	}

	struct pcap_file_header pcap_header;

	pcap_header.magic = 0xa1b2c3d4;//0xd4c3b2a1;
	pcap_header.version_major = 2;
	pcap_header.version_minor = 4;
	pcap_header.thiszone = 0;     
	pcap_header.sigfigs = 0;
	pcap_header.snaplen = 8192;
	pcap_header.linktype = 204;

	tb_long_t wb = tb_file_writ(test_file, (tb_byte_t *)&pcap_header, sizeof(struct pcap_file_header));
	printf("wb:%d\n", wb);
	
}
void write_file()
{
	
	tb_byte_t packet_data[] = {0xff,0x03,0xc0,0x21,0x01,0x01,0x00 ,0x14 ,0x02 ,0x06 ,0x00 ,0x00 ,0x00 ,0x00 ,0x05 ,0x06,0xea ,0x67 ,0x6b ,0xc5 ,0x07 ,0x02 ,0x08 ,0x02 ,0x35 ,0x8b};
	struct pcap_pkthdr pkt_header;
	pkt_header.ts.tv_sec = 1577275287;
	pkt_header.ts.tv_usec = 0;
	pkt_header.caplen = sizeof(packet_data)+1;
	pkt_header.len = sizeof(packet_data)+1;
	tb_byte_t write_data[2048] = {0};
	char dir = 0;
	memcpy(write_data, &pkt_header, sizeof(struct pcap_pkthdr));
	memcpy(write_data + sizeof(struct pcap_pkthdr), &dir, 1);
	memcpy(write_data + sizeof(struct pcap_pkthdr) + 1, packet_data, sizeof(packet_data));
	
	if(test_file)
	{
		tb_long_t wb = tb_file_writ(test_file, write_data, 1 + sizeof(struct pcap_pkthdr) + sizeof(packet_data));
		printf("sizeof(pcap_pkthdr):%d sizeof(packet_data):%d wb:%d\n", sizeof(struct pcap_pkthdr), sizeof(packet_data), wb);
	}
}
void close_file()
{
	if(test_file)
		tb_file_exit(test_file);

	test_file = NULL;
}



