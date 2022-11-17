#ifndef SERVER_H
#define SERVER_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <pthread.h>

struct ctrl_msg {
    u_int8_t switch_signal_0;
    u_int8_t switch_signal_1;
    u_int8_t switch_signal_2;
    u_int8_t switch_signal_3;
    u_int8_t pwm_motor1;
    u_int8_t pwm_motor2;
};


#endif