#include "server.h"


using namespace std::chrono;
using Clock = std::chrono::steady_clock;

using namespace cv;
using namespace std;
#define START 4
#define MAX_LEN 65535
int socket_desc;

/*Declaring variables for motor*/
/* motor 1 */
int enA = 26; /* BCM 12 || Physical 32 */
int inA1 = 4; /* BCM 23 || Physical 16 */
int inA2 = 5; /* BCM 24 || Physical 18 */

/* motor 2 */
int enB = 23; /* BCM 13 || Pysical 33 */
int inB1 = 0; /* BCM 17 || Pysical 11 */
int inB2 = 2; /* BCM 27 || Pysical 13 */



/*Configuring the PWM*/
void config_pwm(void)
{
    wiringPiSetup();
    /* Motor 1 */
    pinMode(enA, PWM_OUTPUT);    /*set GPIO as output */
    pinMode(inA1, OUTPUT);
    pinMode(inA2, OUTPUT);

    /* Motor 2 */
    pinMode(enB, PWM_OUTPUT);    /*set GPIO as output */
    pinMode(inB1, OUTPUT);
    pinMode(inB2, OUTPUT);
}
static volatile int keep_running = true;
void handler(int arg) {
    pwmWrite(enA, 0);
    pwmWrite(enB, 0);
    keep_running = false;
    exit(1);
}

void* receive_ctrl_msg(void* arg) {
    struct sockaddr_in* to_addr = (struct sockaddr_in*)arg;
    struct ctrl_msg* msg = (struct ctrl_msg*)malloc(sizeof(struct ctrl_msg));
    while (keep_running) {
        /* receive message */
        socklen_t len = sizeof(to_addr);
        int bytes = recvfrom(socket_desc, msg, sizeof(*msg), 0, (struct sockaddr*)&to_addr, &len);

        /*Tic*/
        auto tic_rcv_ctrl_msg = Clock::now(); //First timestamp, before receiving control message

        if (bytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        /*Assign direction to the motors*/
        digitalWrite(inA1, msg->switch_signal_0); /* For motor1*/
        digitalWrite(inA2, msg->switch_signal_1);

        digitalWrite(inB1, msg->switch_signal_2); /* For motor2 */
        digitalWrite(inB2, msg->switch_signal_3);

        /*Assign PWM values to the motors*/
        if (keep_running == true) {
            pwmWrite(enA, msg->pwm_motor1); /* Motor 1 */
            pwmWrite(enB, msg->pwm_motor2); /* Motor 2 */
            printf("pwm 1: %d pwm2: %d\n", msg->pwm_motor1, msg->pwm_motor2);
        }
        else {
            break;
        }

 
        /*Toc*/
        auto toc_rcv_ctrl_msg = Clock::now(); //Second timestamp, after receiving control message
        std::cout << "Elapsed time receiving ctrl msg: " << duration_cast<milliseconds>(toc_rcv_ctrl_msg - tic_rcv_ctrl_msg).count() << std::endl; // Print difference in milliseconds

        /*Save to .csv file*/
        std::ofstream myFile2("rcv_ctrl_msg_timestamp.csv", std::ios::app);
        myFile2 << duration_cast<milliseconds>(toc_rcv_ctrl_msg - tic_rcv_ctrl_msg).count() << endl;
    }
    free(msg);
    return NULL;
}
void* send_video(void* arg) {
    struct sockaddr_in* to_addr = (struct sockaddr_in*)arg;
    int bytes;

    /*Create a video capturing object*/
    cv::VideoCapture video(0);

    /*Check if camera is opened*/
    if (video.isOpened() == false) {
        exit(1);
    }

    /*Create a class to save the frame to*/
    cv::Mat frame;
    /*Encoding, frame-> jpg -> base 64*/
    std::vector<uchar> buf;
    std::vector<int> param(2);
    param[0] = cv::IMWRITE_JPEG_QUALITY;
    param[1] = 60; /* default(95) 0-100 */


    while (video.read(frame) == true && keep_running == true) {

        /*Tic*/
        auto tic_send_video = Clock::now(); // First timestamp, before encoding

        /*Encoding*/
        cv::imencode(".jpg", frame, buf, param);                       /* Encode data from class Mat to JPG */
        auto* enc_msg = reinterpret_cast<unsigned char*>(buf.data()); /* Cast the JPG to char* */
        std::string encoded = base64_encode(enc_msg, buf.size());      /* Encode the data to base 64 */
        int size = START + encoded.size();

        if (size <= MAX_LEN) {
            encoded.insert(0, to_string(size));
            //printf("len message %d. First element %c\n", encoded.size(), encoded[0]);
            bytes = sendto(socket_desc, encoded.c_str(), size, 0, (struct sockaddr*)to_addr, sizeof(*to_addr));
            if (bytes == -1) {
                perror("sendto");
            }

            /*Clear*/
            buf.clear();
            enc_msg = NULL;
            encoded.clear();


            /*Toc*/
            auto toc_send_video = Clock::now(); //Second timestamp
            std::cout << "Elapsed time: " << duration_cast<milliseconds>(toc_send_video - tic_send_video).count() << std::endl; // Print difference in milliseconds

            /*Save to .csv file*/
            std::ofstream myFile1("Send_video_timestamp.csv", std::ios::app);
            myFile1 << duration_cast<milliseconds>(toc_send_video - tic_send_video).count() << endl;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    signal(SIGINT, handler); /* handles ctrl+C */
    config_pwm();
    int port_nr;
    struct sockaddr_in my_addr;
    struct sockaddr_in to_addr;
    int bytes;

    /*Clear the .csv files*/
    std::ofstream myFile1("rcvVideo_timestamp.csv");
    myFile1<<"";
    std::ofstream myFile2("sendCtrlMsg_timestamp.csv");
    myFile2<<"";

    /* check command line arguments */
    if (argc != 3) {
        fprintf(stderr, "usage: %s destination port\n", argv[0]);
        exit(1);
    }
    /* extract destination IP address */
    struct hostent* host = gethostbyname(argv[1]);

    if (host == NULL) {
        fprintf(stderr, "unknown host %s\n", argv[1]);
        exit(1);
    }

    in_addr_t ip_address = *((in_addr_t*)(host->h_addr));

    /* extract local port number */
    if (sscanf(argv[2], "%d", &port_nr) != 1) {
        fprintf(stderr, "invalid port %s\n", argv[2]);
        exit(1);
    }

    /* create UDP socket */
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc == -1) {
        perror("socket");
        exit(1);
    }
    /* bound to any local address on the specified port */
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port_nr);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bytes = bind(socket_desc, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bytes == -1) {
        perror("bind");
        exit(1);
    }
    /* allowing broadcast */
    int on = 1;
    bytes = setsockopt(socket_desc, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));
    if (bytes == -1) {
        perror("setsockopt");
        exit(1);
    }

    /* send message to the specified destination/port */
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port_nr);
    to_addr.sin_addr.s_addr = ip_address;

    pthread_t send_thread, receive_thread;
    pthread_create(&receive_thread, NULL, receive_ctrl_msg, &to_addr);
    pthread_create(&send_thread, NULL, send_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(socket_desc);
    return 0;
}



