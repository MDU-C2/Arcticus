#include "Joystick.h"
int lin_map(float value, float x_0, float y_0, float x_1, float y_1){
    float k = (y_1 - y_0)/(x_1 - x_0);
    float m = y_0 - k*x_0;
    return k*value + m;
}

int map_right(float x, float y){
    if (y < 0){//map -100 -> 1024, 0 -> 200
        return lin_map(y, -100, 1024, 0, 200);
    }
    if (y >= 0 && x >= 0) { //map 0 -> 200, 100 -> 1024
        return lin_map(y, 0, 200, 100, 1024);
    }
    else if( y >= 0 && x < 0) {//map 0 -> 1024, 100 ->200
        return lin_map(y, 0, 1024, 100, 200);
    }
}
float map_left(float x){
    if (x >= 0) { //map 0 -> 200, 100->1024
        return lin_map(x, 0, 200, 100, 1024);
    }
    else { //map -100 ->200, -0.1 -> 1024
        return lin_map(x, -100, 200, 0, 1024);
    }
}

int main(void){
    struct ctrl_msg control_signal = {};
    int back[] = { 0, 1};
    int forward[] = { 1, 0};
    //a window
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Joystick Use", sf::Style::Default);
    sf::Event e;

    //sf::RectangleShape square;
    //square.setFillColor(sf::Color(255, 0, 0, 255));
    //square.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    //square.setOutlineColor(sf::Color(0, 0, 0, 255));
    //square.setSize(sf::Vector2f(50.f, 50.f));

    //get information about the joystick
    sf::Joystick::Identification id = sf::Joystick::getIdentification(0);
    std::cout << "\nVendor ID: " << id.vendorId << "\nProduct ID: " << id.productId << std::endl;
    sf::String controller("Joystick Use: " + id.name);
    window.setTitle(controller);//easily tells us what controller is connected

    window.setVisible(false);


     //query joystick for settings if it's plugged in...
    if (sf::Joystick::isConnected(0)){
        // check how many buttons joystick number 0 has
        unsigned int buttonCount = sf::Joystick::getButtonCount(0);

        // check if joystick number 0 has a Z axis
        bool hasZ = sf::Joystick::hasAxis(0, sf::Joystick::Z);

        std::cout << "Button count: " << buttonCount << std::endl;
        std::cout << "Has a z-axis: " << hasZ << std::endl;

    }
           
    bool move = false;//indicate whether motion is detected
    //int turbo = 1;//indicate whether player is using button for turbo speed ;)

    //for movement
    sf::Vector2f speed = sf::Vector2f(0.f,0.f);

    //time
    sf::Clock tickClock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
    sf::Time duration = sf::Time::Zero;

    bool running = true;

 

    while (running){
        sf::Joystick::isButtonPressed(0, 0);
        while (window.pollEvent(e)){
            /*if (e.type == sf::Event::KeyPressed){
                switch (e.key.code){
                case sf::Keyboard::Escape:
                {
                                             window.close();
                                             return 0;
                }
                    break;
                default:
                    break;
                }
            }*/

           // if (e.type == sf::Event::JoystickMoved){
                move = true;

                std::cout << "X axis: " << speed.x << std::endl;
                std::cout << "Y axis: " << speed.y << std::endl;

                if (speed.y < 0){ //drive forward 
                    control_signal.switch_signal_0 = forward[0];
                    control_signal.switch_signal_1 = forward[1];
                    control_signal.switch_signal_2 = forward[0];
                    control_signal.switch_signal_3 = forward[1];
                }

                else { //drive backwards
                    control_signal.switch_signal_0 = back[0];
                    control_signal.switch_signal_1 = back[1];
                    control_signal.switch_signal_2 = back[0];
                    control_signal.switch_signal_3 = back[1];
                }
                /*inMin = 0;
                inMax = 100;
                outMin = 200;
                outMax = 1024;

                r = rot(speed.x^2 + speed.y^2)

                strength = (r-inMin)/(inMax-inMin) *(outMax-outMin) + outMin;*/

                control_signal.pwm_motor1 = map_left(speed.x);
                control_signal.pwm_motor2 = map_right(speed.x, speed.y);
                std::cout << "pwm1: " << control_signal.pwm_motor1 << std::endl;
                std::cout << "pwm2: " << control_signal.pwm_motor2 << std::endl;



             //   }

             sleep(0.5);
             speed = sf::Vector2f(sf::Joystick::getAxisPosition(0, sf::Joystick::X), sf::Joystick::getAxisPosition(0, sf::Joystick::Y));
            }

            /*else{
                move = false;
            }

            if (sf::Joystick::isButtonPressed(0, 0)){//"A" button on the XBox 360 controller
                turbo = 2;
            }

            if (!sf::Joystick::isButtonPressed(0, 0)){
                turbo = 1;
            }

            if(sf::Joystick::isButtonPressed(0,1)){//"B" button on the XBox 360 controller
                window.close();
                return 0;
            }*/
        //}
        
        //check state of joystick
        

        /*timeSinceLastUpdate += tickClock.restart();
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;

            //update the position of the square according to input from joystick
            //CHECK DEAD ZONES - OTHERWISE INPUT WILL RESULT IN JITTERY MOVEMENTS WHEN NO INPUT IS PROVIDED
            //INPUT RANGES FROM -100 TO 100
            //A 15% DEAD ZONE SEEMS TO WORK FOR ME - GIVE THAT A SHOT
            if (speed.x > 15.f || speed.x < -15.f || speed.y > 15.f || speed.y < -15.f)
                square.move(turbo*speed.x*TimePerFrame.asSeconds(), turbo*speed.y*TimePerFrame.asSeconds());
        }

        window.clear(sf::Color(255, 0, 255));
        window.draw(square);
        window.display();*/
        }
    
    return 0;
}
