/**
 *
 * File Name: libUART.h
 * Title    : libUART header
 * Project  : libUART
 * Author   : Copyright (C) 2018-1019 Johannes Krottmayer <krjdev@gmail.com>
 * Created  : 2018-05-21
 * Modified : 2019-11-30
 * Revised  : 
 * Version  : 0.4.0.0
 * License  : ISC (see file LICENSE.txt)
 *
 * NOTE: This code is currently below version 1.0, and therefore is considered
 * to be lacking in some functionality or documentation, or may not be fully
 * tested. Nonetheless, you can expect most functions to work.
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBUART_LIBUART_H
#define LIBUART_LIBUART_H

#ifdef _WIN32
#include <Windows.h>
#ifdef LIBUART_EXPORTS
#define LIBUART_API __declspec(dllexport)
#else
#define LIBUART_API __declspec(dllimport)
#endif
#endif

struct _uart;

typedef struct _uart uart_t;

enum e_baud {
#ifdef __unix__
    UART_BAUD_0 = 0,
    UART_BAUD_50 = 50,
    UART_BAUD_75 = 75,
#endif
    UART_BAUD_110 = 110,
#ifdef __unix__
    UART_BAUD_134 = 134,
    UART_BAUD_150 = 150,
    UART_BAUD_200 = 200,
#endif
    UART_BAUD_300 = 300,
    UART_BAUD_600 = 600,
    UART_BAUD_1200 = 1200,
#ifdef __unix__
    UART_BAUD_1800 = 1800,
#endif
    UART_BAUD_2400 = 2400,
    UART_BAUD_4800 = 4800,
    UART_BAUD_9600 = 9600,
#ifdef _WIN32
    UART_BAUD_14400 = 14400,
#endif
    UART_BAUD_19200 = 19200,
    UART_BAUD_38400 = 38400,
    UART_BAUD_57600 = 57600,
    UART_BAUD_115200 = 115200,
#ifdef _WIN32
    UART_BAUD_128000 = 128000,
    UART_BAUD_256000 = 256000
#elif __unix__
    UART_BAUD_230400 = 230400,
    UART_BAUD_460800 = 460800,
    UART_BAUD_500000 = 500000,
    UART_BAUD_576000 = 576000,
    UART_BAUD_921600 = 921600,
    UART_BAUD_1000000 = 1000000,
    UART_BAUD_1152000 = 1152000,
    UART_BAUD_1500000 = 1500000,
    UART_BAUD_2000000 = 2000000,
    UART_BAUD_2500000 = 2500000,
    UART_BAUD_3000000 = 3000000,
    UART_BAUD_3500000 = 3500000,
    UART_BAUD_4000000 = 4000000
#endif
};

enum e_parity {
    UART_PARITY_NO,
    UART_PARITY_ODD,
    UART_PARITY_EVEN
};

enum e_flow {
    UART_FLOW_NO,
    UART_FLOW_SOFTWARE,
    UART_FLOW_HARDWARE
};

enum e_pins {
    UART_PIN_RTS,   /* Request to Send (out) */
    UART_PIN_CTS,   /* Clear to Send (in) */
    UART_PIN_DSR,   /* Data Set Ready (in) */
    UART_PIN_DCD,   /* Data Carrier Detect (in) */
    UART_PIN_DTR,   /* Data Terminal Ready (out) */
    UART_PIN_RI     /* Ring Indicator (in) */
};

#define UART_PIN_LOW        0
#define UART_PIN_HIGH       1

#ifdef __unix__
extern uart_t *libUART_open(const char *dev, int baud, const char *opt);
extern void libUART_close(uart_t *uart);
extern int libUART_send(uart_t *uart, char *send_buf, int len);
extern int libUART_recv(uart_t *uart, char *recv_buf, int len);
extern int libUART_puts(uart_t *uart, char *msg);
extern int libUART_getc(uart_t *uart, char *c);
extern int libUART_set_baud(uart_t *uart, int baud);
extern int libUART_get_baud(uart_t *uart, int *baud);
extern int libUART_get_fd(uart_t *uart, int *fd);
extern int libUART_get_dev(uart_t *uart, char **dev);
extern int libUART_set_databits(uart_t *uart, int data_bits);
extern int libUART_get_databits(uart_t *uart, int *data_bits);
extern int libUART_set_parity(uart_t *uart, int parity);
extern int libUART_get_parity(uart_t *uart, int *parity);
extern int libUART_set_stopbits(uart_t *uart, int stop_bits);
extern int libUART_get_stopbits(uart_t *uart, int *stop_bits);
extern int libUART_set_flowctrl(uart_t *uart, int flow_ctrl);
extern int libUART_get_flowctrl(uart_t *uart, int *flow_ctrl);
extern int libUART_set_pin(uart_t *uart, int pin, int state);
extern int libUART_get_pin(uart_t *uart, int pin, int *state);
extern int libUART_get_bytes_available(uart_t *uart, int *num);
extern void libUART_set_error(int enable);
extern char *libUART_get_libname(void);
extern char *libUART_get_libversion(void);
extern char *libUART_get_libcopyright(void);
#elif _WIN32
extern  uart_t *libUART_open(const char *dev, int baud, const char *opt);
extern  void libUART_close(uart_t *uart);
extern  int libUART_send(uart_t *uart, char *send_buf, int len);
extern  int libUART_recv(uart_t *uart, char *recv_buf, int len);
extern  int libUART_puts(uart_t *uart, char *msg);
extern  int libUART_getc(uart_t *uart, char *c);
extern  int libUART_set_baud(uart_t *uart, int baud);
extern  int libUART_get_baud(uart_t *uart, int *baud);
extern  int libUART_get_handle(uart_t *uart, HANDLE *handle);
extern  int libUART_get_dev(uart_t *uart, char **dev);
extern  int libUART_set_databits(uart_t *uart, int data_bits);
extern  int libUART_get_databits(uart_t *uart, int *data_bits);
extern  int libUART_set_parity(uart_t *uart, int parity);
extern  int libUART_get_parity(uart_t *uart, int *parity);
extern  int libUART_set_stopbits(uart_t *uart, int stop_bits);
extern  int libUART_get_stopbits(uart_t *uart, int *stop_bits);
extern  int libUART_set_flowctrl(uart_t *uart, int flow_ctrl);
extern  int libUART_get_flowctrl(uart_t *uart, int *flow_ctrl);
extern  int libUART_set_pin(uart_t *uart, int pin, int state);
extern  int libUART_get_pin(uart_t *uart, int pin, int *state);
extern  int libUART_get_bytes_available(uart_t *uart, int *num);
extern  void libUART_set_error(int enable);
extern  char *libUART_get_libname(void);
extern  char *libUART_get_libversion(void);
extern  char *libUART_get_libcopyright(void);
#endif
#endif
#ifdef __cplusplus
}
#endif
