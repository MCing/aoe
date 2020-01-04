#include "tbox/tbox.h"
#include <stdio.h>
#include <stdlib.h>
#include "libUART.h"
#include "at.h"
#include "util.h"



//uart test
#ifdef __unix__
#include <unistd.h>
#define UART_DEV    "/dev/ttyUSB0"
#elif _WIN32
#include <Windows.h>
#define UART_DEV    "\\\\.\\COM3"
#endif

#define CONFIG_UART "COM3"
int unit_test_uartlib()
{
    uart_t *uart;
    char* buf;
    int bytes;
    tb_trace_i("--------unit_test_uartlib-----------");
    tb_trace_i("%s %s\r\n", libUART_get_libname(), libUART_get_libversion());
	tb_trace_i("port:%s", UART_DEV);
    uart = libUART_open(UART_DEV, UART_BAUD_115200, "8N1N");
    
    if (!uart)
        return -1;

	#if 1
    tb_trace_i("TX: %d byte(s)\n", libUART_puts(uart, "AT+GMR\r\n"));
    
	tb_msleep(500);

    libUART_get_bytes_available(uart, &bytes);
    tb_trace_i("RX: %d byte(s) available\n", bytes);
    buf = (char *) malloc(bytes + 1);
    libUART_recv(uart, buf, bytes);
    buf[bytes] = '\0';
    tb_trace_i("RX: %s\n", buf);
    free(buf);
	#endif
    libUART_close(uart);
    return 0;
}

int unit_test_uartdevice()
{
	int result = 0;
	rt_device_t device = NULL;
	unsigned char recv_buf[1024] = {0};
	tb_trace_i("--------unit_test_uartdevice-----------");
	if(rt_init_device(CONFIG_UART, 115200, "8N1N"))
	{
		tb_trace_i("rt_init_device fail");
		return result;
	}
	tb_trace_i("rt_init_device ok");
	device = rt_find_device(CONFIG_UART);

	if(!device)
	{
		tb_trace_i("rt_find_device fail");
		return result;
	}
	tb_trace_i("rt_find_device ok");

	
	result = rt_device_write(device, 0, "AT+GMR\r\n", strlen("AT+GMR\r\n"));
	tb_trace_i("rt_device_write result:%d", result);

	tb_msleep(500);
	
	result = rt_device_read(device, 0, recv_buf, sizeof(recv_buf));
	tb_trace_i("rt_device_read result:%d buf:%s", result, recv_buf);

	rt_device_close(device);

	return 1;
	
}


//at test

void uint_at_test()
{
	int result = 0;
	tb_trace_i("------------uint_at_test----------");
	if(rt_init_device(CONFIG_UART, 115200, "8N1N"))
	{
		tb_trace_i("rt_init_device fail");
		return;
	}
	tb_trace_i("rt_init_device ok");
	result = at_client_init(CONFIG_UART, 1024);

	if(result)
	{
		tb_trace_i("at_client_init fail");
		return;
	}
	tb_trace_i("at_client_init ok");

	at_response_t resp = NULL;
	resp = at_create_resp(128, 0, 5000);
	if (!resp)
    {
        tb_trace_i("No memory for response structure!");
        goto exit;
    }
	tb_trace_i("at_create_resp ok");

	if (at_exec_cmd(resp, "AT+IVSN") != RT_EOK)
    {
        tb_trace_i("AT client send commands failed, response error or timeout !");
        goto exit;
    }

	
	tb_trace_i("at_exec_cmd resp count:%d result:", resp->line_counts);
	for(int i = 0; i < resp->line_counts; i++)
	{
		char *tmp_buf = NULL;
		tmp_buf = at_resp_get_line(resp, i+1);
		tb_trace_i("%s", tmp_buf);
	}

    /* 命令发送成功 */
    tb_trace_i("AT Client send commands to AT Server success!");

    exit:
	if(resp != NULL)
   		at_delete_resp(resp);

	at_client_release(at_client_get_first());

}



void uint_at_checkstarttime()
{
	int result = 0;
	tb_trace_i("------------uint_at_checkstarttime----------");
	if(rt_init_device(CONFIG_UART, 115200, "8N1N"))
	{
		tb_trace_i("rt_init_device fail");
		return;
	}
	tb_trace_i("rt_init_device ok");
	result = at_client_init(CONFIG_UART, 1024);

	if(result)
	{
		tb_trace_i("at_client_init fail");
		return;
	}

	at_response_t resp = NULL;
	resp = at_create_resp(128, 0, 5000);
	if (!resp)
    {
        tb_trace_i("No memory for response structure!");
        goto exit;
    }

	//check AT 
	at_resp_set_info(resp, 128, 0, 100);
	do
	{
		if (at_exec_cmd(resp, "AT") == RT_EOK)
	    {
	        break;
	    }
	}
	while (1);
	
	at_resp_set_info(resp, 128, 0, 1000);
	if (at_exec_cmd(resp, "AT+CFUN=1,1") != RT_EOK)
    {
        tb_trace_i("AT client send commands failed, response error or timeout !");
        goto exit;
    }

	//start
	tb_trace_i("start %lld", util_current_time());
	do
	{
		if (at_exec_cmd(resp, "AT+IVSN") == RT_EOK)
	    {
	        break;
	    }
	}
	while (1);
	tb_trace_i("end %lld", util_current_time());
	//end

    exit:
	if(resp != NULL)
   		at_delete_resp(resp);

	at_client_release(at_client_get_first());

}







