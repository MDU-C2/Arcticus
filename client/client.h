#ifndef CLIENT_H
#define CLIENT_H

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

#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <math.h>

#include "opencv2/opencv.hpp"
#include <iostream>
#include "base64.h"
#include "opencv2/highgui.hpp"
#include <vector>
#include <signal.h>
#include <chrono>
#include <iostream>

struct ctrl_msg {
    u_int8_t switch_signal_0;
    u_int8_t switch_signal_1;
    u_int8_t switch_signal_2;
    u_int8_t switch_signal_3;
    u_int16_t pwm_motor1;
    u_int16_t pwm_motor2;
};
#endif
