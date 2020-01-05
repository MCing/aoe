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


void save_config()
{
	const char *filename = CONFIG_FILE;
	JSON_Value *root = NULL;
	JSON_Value *uart = NULL;
	JSON_Value *ppp = NULL;
	
	JSON_Object *obj = NULL;
	
	root = json_value_init_object();
	uart = json_value_init_object();
	ppp = json_value_init_object();

	obj = json_value_get_object(uart);	
	json_object_set_string(obj, JSON_KW_UART_PORT, uart_option.port_name);
	json_object_set_number(obj, JSON_KW_UART_BAUD, uart_option.baud);


	obj = json_value_get_object(ppp);
	json_object_set_number(obj, JSON_KW_PPP_AUTHTYPE, ppp_option.auth_type);
	json_object_set_string(obj, JSON_KW_PPP_USER, ppp_option.user);
	json_object_set_string(obj, JSON_KW_PPP_PASSWORD, ppp_option.pwd);
	json_object_set_string(obj, JSON_KW_PPP_REMOTEIP, ppp_option.remote_ip);
	json_object_set_string(obj, JSON_KW_PPP_LOCALIP, ppp_option.local_ip);
	json_object_set_string(obj, JSON_KW_PPP_APN, ppp_option.apn);
	json_object_set_number(obj, JSON_KW_PPP_ECHO_INTERVAL, ppp_option.lcp_echo_interval);
	json_object_set_number(obj, JSON_KW_PPP_ECHO_FAILS, ppp_option.lcp_echo_fails);


	obj = json_value_get_object(root);
	json_object_set_value(obj, JSON_KW_UART, uart);
	json_object_set_value(obj, JSON_KW_PPP, ppp);

	
	json_serialize_to_file_pretty(root, filename);

}

void aoe_init_configuration()
{
	int result = 0;
	result += parse_uart_configuration(CONFIG_FILE, 0);
	result += parse_ppp_configuration(CONFIG_FILE);

	if(result)
		save_config();

}




//ppp
void exit_ppp_mode()
{
	at_client_get_first()->device->state = UART_AT_CLIENT;

}

int enter_ppp_mode()
{
	at_client_get_first()->device->state = UART_PPP;
}


int ppp_at_dial()
{
	int result = -1;
	at_response_t resp = NULL;
	resp = at_create_resp(128, 3, 10000);  //result in line 3
	
	if (!resp)
	{
		tb_trace_i("No memory for response structure!");
		result = -1;
		goto exit;
	}
	tb_trace_i("at_create_resp ok");

	if (at_exec_cmd(resp, "ATD*99#") != RT_EOK)
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

	return result;


}

int ppp_mode_inused()
{
	return at_client_get_first()->device->state == UART_PPP;
}




