#include "libUART.h"
#include "rtdef.h"
#include "tbox/tbox.h"




//device
#define MAX_DEVICE_NUM 2
static rt_device_t device_table[MAX_DEVICE_NUM] = {NULL};


int rt_init_device(const char *name, int baud, const char *opt)
{
	int empty_index = -1;
	int i = 0;
	for(i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if(empty_index == -1 && device_table[i] == NULL)
			empty_index = i;

		if(device_table[i] != NULL && (strcmp(device_table[i]->name, name) == 0))
			return -1;
	}
	
	if(empty_index == -1)
	{
		return -1;
	}
	//LOG_D("rt_init_device to init empty_index:%d", empty_index);
	
	device_table[empty_index] = rt_uart_open(name, baud, opt);

	if(device_table[empty_index] == NULL)
		return -1;


	return 0;
}


rt_device_t rt_find_device(const char *name)
{
	int i = 0;
	
	if(name == NULL || (strlen(name) == 0))
		return NULL;


	for(i = 0; i < MAX_DEVICE_NUM; i++)
	{
		//LOG_D("rt_find_device i:%d", i);
		if(device_table[i] != NULL && (strcmp(device_table[i]->name, name) == 0))
			return device_table[i];
	}

	return NULL;
}


rt_size_t rt_device_write(rt_device_t dev,
                          rt_off_t    pos,
                          const void *buffer,
                          rt_size_t   size)
{

    if (dev->uart == RT_NULL)
    {
        return 0;
    }

	//LOG_D("rt_device_write size:%d buffer:%s", size, buffer);
    return libUART_send(dev->uart, (char*)buffer, size);

}


rt_size_t rt_device_read(rt_device_t dev,
					   rt_off_t    pos,
					   void 	  *buffer,
					   rt_size_t   size)
{
	//LOG_D("rt_device_read");
	int bytes = 0;
    if (dev->uart == RT_NULL)
    {
        return 0;
    }

	libUART_get_bytes_available(dev->uart, &bytes);

	//LOG_D("libUART_get_bytes_available bytes:%d size:%d", bytes, size);
	if(bytes  > size)
		bytes = size;
	
	if(bytes > 0)
	{
		//LOG_D("libUART_recv to read bytes:%d", bytes);
	  	return libUART_recv(dev->uart, buffer, bytes);
	}

    return bytes;
}


rt_device_t rt_uart_open(const char *name, int baud, const char *opt)
{
	char dev_full_name[128] = "\\\\.\\";
	rt_device_t dev = RT_NULL;
	if(name == RT_NULL)
		return RT_NULL;

	dev = (rt_device_t) rt_calloc(1, sizeof(struct rt_uart_device));
	
	if (dev == RT_NULL)
	{
		return RT_NULL;
	}
	
	memset(dev, 0, sizeof(struct rt_uart_device));
	strcat(dev_full_name, name);
	dev->uart = libUART_open(dev_full_name, baud, opt);
	if(dev->uart)
	{
		memcpy(dev->name, name, strlen(name));
		dev->baud = baud;
	}
	else
	{
		rt_free(dev);
		dev = RT_NULL;
	}

	return dev;
	
}
rt_err_t  rt_device_close(rt_device_t dev)
{
	int i = 0;

	if (dev == RT_NULL)
	{
		return RT_ERROR;
	}
	
	libUART_close(dev->uart);
	dev->uart = NULL;

	for(i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if(device_table[i] == dev)
		{
			rt_free(device_table[i]);
			device_table[i] = NULL;
			break;
		}
	}

	return RT_EOK;
}



//mutex
rt_mutex_t rt_mutex_create(const char *name, rt_uint8_t flag)
{
	//name and flag unused for now
	return tb_mutex_init();
}

rt_err_t rt_mutex_delete(rt_mutex_t mutex)
{
	tb_mutex_exit(mutex);
	return RT_EOK;
}

rt_err_t rt_mutex_take(rt_mutex_t mutex, rt_int32_t time)
{
	//time unused for now
	
	if(mutex)
	{
		tb_mutex_enter(mutex);
		return RT_EOK;
	}
	else
		return RT_ERROR;
	
		
}
rt_err_t rt_mutex_release(rt_mutex_t mutex)
{
	
	if(mutex)
	{
		tb_mutex_exit(mutex);
		return RT_EOK;
	}
	else
		return RT_ERROR;

}


//semaphore
rt_sem_t rt_sem_create(const char *name, rt_uint32_t value, rt_uint8_t flag)
{
	return tb_semaphore_init(value);
}
rt_err_t rt_sem_delete(rt_sem_t sem)
{
	if(sem)
	{
		tb_semaphore_exit(sem);
		return RT_EOK;
	}
	else
		return RT_ERROR;
	
}

rt_err_t rt_sem_take(rt_sem_t sem, rt_int32_t time)
{
	if(sem)
	{
		tb_long_t wait = tb_semaphore_wait(sem, time);
		if(wait == 0)
			return RT_ETIMEOUT;
		else if(wait == -1)
			return RT_ERROR;
		else
			return RT_EOK;
	}
	else
		return RT_ERROR;
	
}

rt_err_t rt_sem_release(rt_sem_t sem)
{
	if(sem)
	{
		tb_semaphore_post(sem, 1);
		return RT_EOK;
	}
	else
		return RT_ERROR;
	
}

//thread
//rt_thread_t
void rt_thread_exit(int thread_id)
{
	tb_thread_exit(thread_id);
}
int rt_thread_create(const char *name,
                             int (*entry)(void *parameter),
                             void       *parameter,
                             rt_uint32_t stack_size,
                             rt_uint8_t  priority,
                             rt_uint32_t tick)
{
	return tb_thread_init(name, entry, parameter, stack_size);
}


rt_err_t rt_thread_startup(rt_thread_t thread)
{
	return RT_EOK; 
}


//tick
rt_tick_t  rt_tick_from_millisecond(rt_int32_t ms)
{
	return ms;
}

rt_tick_t rt_tick_get(void)
{
	return tb_mclock();
}





