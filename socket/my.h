#ifndef MY_H
#define MY_H

/* debug stuff */
#define DEBUG_SOCKET
//#define DEBUG_MATLAB
//#define DEBUG_ACQ
#define ESSID_MAX_COUNT 64
#define SEND_BUFF_SIZE     4096
#define IW_ESSID_MAX_SIZE	32
#define BUFF_SIZE 3500
#define RECV_BUFF_SIZE 1024*12
#define SOCKET_CMD_BUFF_SIZE 128
#define UART_CMD_BUFF_SIZE 16

#define NEEDED_ESSID_COUNT 8

struct send_info
{
    char		data[IW_ESSID_MAX_SIZE+1];			/* 数据长度 */
    int  		level;		/*信号质量*/
};

struct socket_info
{
    char head[8];
    int count;
    char update;
    char buff[BUFF_SIZE];

};

struct socket_cmd_info
{
    char head[8];
    char buff[SOCKET_CMD_BUFF_SIZE];
};

struct uart_cmd_info
{
    char head[8];
    char buff[UART_CMD_BUFF_SIZE];
};




#endif // MY_H
