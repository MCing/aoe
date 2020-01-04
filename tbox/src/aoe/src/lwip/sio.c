#include <lwip/opt.h>
#include <lwip/sys.h>
#include <lwip/sio.h>

#include <stdio.h>
#include <stdarg.h>

#include "libUART.h"


#include "lwipcfg_msvc.h"

#include "rtdef.h"


static int sio_abort = 0;


/**
 * SIO_DEBUG: Enable debugging for SIO.
 */
#ifndef SIO_DEBUG
//#define SIO_DEBUG    LWIP_DBG_OFF 
#define SIO_DEBUG    LWIP_DBG_ON
#endif


/**
 * Opens a serial device for communication.
 * 
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
	char comport[128] = {0};
	snprintf(comport, sizeof(comport), "COM%d", devnum);
    return rt_find_device(comport);
}

/**
 * Sends a single character to the serial device.
 * 
 * @param c character to send
 * @param fd serial device handle
 * 
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
    rt_device_write((rt_device_t)fd, 0, &c, 1);
}

/**
 * Receives a single character from the serial device.
 * 
 * @param fd serial device handle
 * 
 * @note This function will block until a character is received.
 */
u8_t sio_recv(sio_fd_t fd)
{
	u8_t byte;
    libUART_recv(((rt_device_t)fd)->uart, &byte, 1);
    return byte;
}

/**
 * Reads from the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 * 
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    return rt_device_read((rt_device_t)fd, 0, data, len);
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 * 
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
    return rt_device_read((rt_device_t)fd, 0, data, len);
}

/**
 * Writes to the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data to send
 * @param len length (in bytes) of data to send
 * @return number of bytes actually sent
 * 
 * @note This function will block until all data can be sent.
 */
u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
    return rt_device_write((rt_device_t)fd, 0, data, len);
}

/**
 * Aborts a blocking sio_read() call.
 * @todo: This currently ignores fd and aborts all reads
 * 
 * @param fd serial device handle
 */
void sio_read_abort(sio_fd_t fd)
{
    LWIP_UNUSED_ARG(fd);
    LWIP_DEBUGF(SIO_DEBUG, ("sio_read_abort() !!!!!...\n"));
    sio_abort = 1;
    return;
}
