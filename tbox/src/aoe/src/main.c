/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox/tbox.h"
#include <stdio.h>
#include <stdlib.h>

#include "unitest/test_demo.h"

#ifdef RT_USING_FINSH
#include "shell.h"
#endif

#include "rtdef.h"
#include "aoe_common.h"

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




/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
    if (!tb_init(tb_null, tb_null)) return -1;
	tb_trace_i("hello tbox!");
	//debug to file 
	//tb_trace_mode_set(TB_TRACE_MODE_FILE);
	//tb_trace_file_set_path("tbox.log", tb_false);

	//load configuration from file
	aoe_init_configuration();
	//try to init uart
	aoe_init_uart();
	
	//start shell
	#ifdef RT_USING_FINSH
	/* initialize finsh */
	finsh_system_init();
	#endif

	
	while(1){tb_sleep(1);}
    // exit tbox
    tb_trace_i("exit tbox!");
    tb_exit();

    return 0;
}
