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


/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
    if (!tb_init(tb_null, tb_null)) return -1;
	tb_trace_i("hello tbox!");

	//load configuration from file
	aoe_init_configuration();
	//try to init uart
	aoe_init_uart();
	aoe_init_log_config();

	//init pcap log process
	aoe_pcap_init();
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
