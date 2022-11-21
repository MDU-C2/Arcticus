#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <math.h>

struct ctrl_msg {
    u_int8_t switch_signal_0; /*for motor 1*/
    u_int8_t switch_signal_1; /*for motor 1*/
    u_int8_t switch_signal_2; /*for motor 2*/
    u_int8_t switch_signal_3; /*for motor 2*/
    u_int16_t pwm_motor1;
    u_int16_t pwm_motor2;
};