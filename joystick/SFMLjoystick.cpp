#include "Joystick.h"
int lin_map(float value, float x_0, float y_0, float x_1, float y_1){
    int y;
    float k = (y_1 - y_0)/(x_1 - x_0);
    float m = y_0 - k*x_0;
    y = k*value + m;
    if (y > 1024) {
        y = 1024;
    }
    return y;
}

int main(void){
    struct ctrl_msg control_signal = {};
    int back[] = { 0, 1};
    int forward[] = { 1, 0};

    /* window */
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Joystick Use", sf::Style::Default);
    sf::Event e;

    sf::Joystick::Identification id = sf::Joystick::getIdentification(0);
    std::cout << "\nVendor ID: " << id.vendorId << "\nProduct ID: " << id.productId << std::endl;
    sf::String controller("Joystick Use: " + id.name);
    window.setTitle(controller);

    window.setVisible(false);


    /* query joystick for settings if it's plugged in */
    if (sf::Joystick::isConnected(0)){
        /* check how many buttons joystick number 0 has */
        unsigned int button_count = sf::Joystick::getButtonCount(0);

        /* check if joystick number 0 has a Z axis */
        bool haz_z = sf::Joystick::hasAxis(0, sf::Joystick::Z);

        std::cout << "Button count: " << button_count << std::endl;
        std::cout << "Has a z-axis: " << haz_z << std::endl;

    }


    /* for movement */
    sf::Vector2f speed = sf::Vector2f(0.f,0.f);

    bool running = true;

    while (running){
        sf::Joystick::isButtonPressed(0, 0);
        while (window.pollEvent(e)){
                std::cout << "X axis: " << speed.x << std::endl;
                std::cout << "Y axis: " << speed.y << std::endl;

                if (speed.y < 0){ /* drive forward */
                    control_signal.switch_signal_0 = forward[0];
                    control_signal.switch_signal_1 = forward[1];
                    control_signal.switch_signal_2 = forward[0];
                    control_signal.switch_signal_3 = forward[1];
                }

                else { /* drive backwards */
                    control_signal.switch_signal_0 = back[0];
                    control_signal.switch_signal_1 = back[1];
                    control_signal.switch_signal_2 = back[0];
                    control_signal.switch_signal_3 = back[1];
                }
                int abs_vel = sqrt(speed.x*speed.x + speed.y*speed.y);
                int abs_mapped = lin_map(abs_vel, 0, 200, 100, 1024);
                if (speed.x > 0) {
                    control_signal.pwm_motor1 = ((100 - speed.x)/100)*abs_mapped;
                    control_signal.pwm_motor2 = abs_mapped;
                } else {
                    control_signal.pwm_motor2 = ((100 + speed.x)/100)*abs_mapped;
                    control_signal.pwm_motor1 = abs_mapped;
                }
                //speed_y+= speed_x_temp;
               
                std::cout << "pwm1: " << control_signal.pwm_motor1 << std::endl;
                std::cout << "pwm2: " << control_signal.pwm_motor2 << std::endl;


             sleep(0.5);
             speed = sf::Vector2f(sf::Joystick::getAxisPosition(0, sf::Joystick::X), sf::Joystick::getAxisPosition(0, sf::Joystick::Y));
            }
        }
    
    return 0;
}
