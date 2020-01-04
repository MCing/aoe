/*
	hal level for rt-thread to tbox
 */

#ifndef __HAL_DEF_H__
#define __HAL_DEF_H__
#include "aoe_config.h"
#include "tbox/tbox.h"
#include "libUART.h"



#define AT_USING_CLIENT


#ifdef AOE_REFACTORING
//in tbox
//#define RT_ASSERT(x)  tb_assert
#define RT_ASSERT(x)  do{}while(0)

#define rt_realloc  tb_ralloc
#define rt_calloc tb_nalloc
#define rt_free tb_free
#define  rt_memset tb_memset
#define rt_strncpy tb_strncpy

#define rt_kprintf tb_printf  //debug

#define rt_strlen tb_strlen

#define rt_memmove tb_memmov


#define LOG_E tb_trace_i
#define LOG_D tb_trace_i
#define LOG_I tb_trace_i



//in std C
#define rt_snprintf snprintf
#define rt_strcmp strcmp


//rt-thread define
#define RT_NAME_MAX_DUP 128


//aoe define
#define UART_READ_POLL_TIME_MS 5

//define for rt-thread finsh
#define RT_ALIGN_SIZE	8
#define RT_NAME_MAX	    8
#define RT_CONSOLEBUF_SIZE 128


#endif /*AOE_REFACTORING*/




#endif
