/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-03-22     Bernard      first version
 */
 
//#include <rtthread.h>
#include "rtdef.h"

#ifdef RT_USING_FINSH

#include "finsh.h"

long hello(void);
long version(void);
long list(void);
long list_thread(void);
long list_sem(void);
long list_mutex(void);
long list_fevent(void);
long list_event(void);
long list_mailbox(void);
long list_msgqueue(void);
long list_mempool(void);
long list_timer(void);

long aoe_exit(void);
long lwip_ppp(void);
void app_notify_exit_ppp(void);
long lwip_tcp(void);
long lwip_tcpsend(void);
long switch_ppp_cmd_mode();
long switch_ppp_data_mode();
void at(int argc, char **argv);
int cmd_ping(int argc, char **argv);

void aoe_init_configuration();
void aoe_close_uart(void);
void aoe_init_uart(void);

int cmd_iperf(int argc, char **argv);

int aoe_test(int argc, char **argv);
int cmd_tcp(int argc, char **argv);
int cmd_tcp_send(int argc, char **argv);
int cmd_tcp_close();



#ifdef FINSH_USING_SYMTAB
struct finsh_syscall *_syscall_table_begin  = NULL;
struct finsh_syscall *_syscall_table_end    = NULL;
struct finsh_sysvar *_sysvar_table_begin    = NULL;
struct finsh_sysvar *_sysvar_table_end      = NULL;
#else
struct finsh_syscall _syscall_table[] =
{
    {"hello", hello, "hello world"},
    {"version", version, "show version of aoe and build time"},
    {"ppp", lwip_ppp, "setup ppp call"},
    {"ppp_cmd", switch_ppp_cmd_mode, "switch to at online cmd mode(+++)"},
    {"ppp_data", switch_ppp_data_mode, "switch to ppp data mode(ATO)"},
    {"ppp_exit", app_notify_exit_ppp, "terminal ppp"},
    {"tcp", lwip_tcp, "tcp test"},
	{"tcpsend", lwip_tcpsend, "tcp send data test"},
	{"exit", aoe_exit, "exit aoe"},
	{"at", at, "enter at debug mode"},
    {"ping", cmd_ping, "ping program"},
    {"iperf", cmd_iperf, "iperf test"},
    {"tcp_setup", cmd_tcp, "tcp test"},
    {"tcp_send", cmd_tcp_send, "tcp test"},
    {"tcp_close", cmd_tcp_close, "tcp test"},

    //{"uart_state", uart_state},
    {"uart_init", aoe_init_uart, "init uart"},
    {"uart_close", aoe_close_uart, "close uart"},
    {"reconfig", aoe_init_configuration, "update config from file"},

    {"test",aoe_test, "just for test"},
    {"list", list, "list all command supported"},

};
struct finsh_syscall *_syscall_table_begin = &_syscall_table[0];
struct finsh_syscall *_syscall_table_end   = &_syscall_table[sizeof(_syscall_table) / sizeof(struct finsh_syscall)];

struct finsh_sysvar *_sysvar_table_begin  = NULL;
struct finsh_sysvar *_sysvar_table_end    = NULL;
#endif

#endif /* RT_USING_FINSH */

