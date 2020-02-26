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
extern int atest_setup_ppp(int timeout_s);
extern int atest_close_ppp();
extern void aoe_lwip_init();
extern int atest_ping();
extern int atest_iperf();

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
    if (!tb_init(tb_null, tb_null)) return -1;

	//load configuration from file
	aoe_init_configuration();
	//try to init uart
	aoe_init_uart();
	aoe_init_log_config();

	//init pcap log process
	aoe_pcap_init();

    aoe_lwip_init();

    //setup ppp
    if(atest_setup_ppp(30))
    {
        goto exit;
    }
    //TODO: handle test work
    atest_ping();
    atest_iperf();

    //close ppp
    atest_close_ppp();

    exit:
    // exit tbox
    tb_exit();

    return 0;
}
