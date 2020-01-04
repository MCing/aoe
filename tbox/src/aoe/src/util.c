#include "tbox/tbox.h"
#include <stdio.h>
#include <stdlib.h>



//get current  time since 1 Jan 1601 in ms
tb_hong_t  util_current_time()
{
	 // get the time
    tb_timeval_t tv = {0};
	 tb_hong_t val = 0;
    if (tb_gettimeofday(&tv, tb_null))
    {
        val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }

	return val;
}

