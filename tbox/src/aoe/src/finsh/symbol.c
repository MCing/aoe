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

long aoetest(int arg, void *argv);
long aoe_exit(void);
long lwip_ppp(void);
void app_notify_exit_ppp(void);
long lwip_tcp(void);
long lwip_tcpsend(void);
long switch_ppp_cmd_mode();
long switch_ppp_data_mode();

#ifdef FINSH_USING_SYMTAB
struct finsh_syscall *_syscall_table_begin  = NULL;
struct finsh_syscall *_syscall_table_end    = NULL;
struct finsh_sysvar *_sysvar_table_begin    = NULL;
struct finsh_sysvar *_sysvar_table_end      = NULL;
#else
struct finsh_syscall _syscall_table[] =
{
    {"hello", hello},
    {"version", version},
    {"aoetest", aoetest},
    {"ppp", lwip_ppp},
    {"ppp_cmd", switch_ppp_cmd_mode},
    {"ppp_data", switch_ppp_data_mode},
    {"ppp_exit", app_notify_exit_ppp},
    {"tcp", lwip_tcp},
	{"tcpsend", lwip_tcpsend},
	{"exit", aoe_exit},
    {"list", list},

};
struct finsh_syscall *_syscall_table_begin = &_syscall_table[0];
struct finsh_syscall *_syscall_table_end   = &_syscall_table[sizeof(_syscall_table) / sizeof(struct finsh_syscall)];

struct finsh_sysvar *_sysvar_table_begin  = NULL;
struct finsh_sysvar *_sysvar_table_end    = NULL;
#endif

#endif /* RT_USING_FINSH */

