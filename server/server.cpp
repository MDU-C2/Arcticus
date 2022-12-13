                                                                                  
#include "server.h"


using namespace std::chrono;
using Clock = std::chrono::steady_clock;

using namespace cv;
using namespace std;
#define START 4
#define MAX_LEN 65535

#define JOY_SLEEP 0.015 //15ms
#define VIDEO_SLEEP 0.015 //15ms
#define SEND_PRIO 80
#define RECV_PRIO 79

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

// include time?
clock_t start_command_clk, end_command_clk, start_video_clk, end_video_clk;

double rcvCtrlCPU, sendVideoCPU;


static void setprio(int prio, int sched) {
    struct sched_param param;
    // Set realtime priority for this thread 
    param.sched_priority = prio;
    if (sched_setscheduler(0, sched, &param) < 0)
        perror("sched_setscheduler");
}
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
int keep_running = true;
void handler(int arg) {
    pwmWrite(enA, 0);
    pwmWrite(enB, 0);
    keep_running = false;
    exit(1);
}

void* receive_ctrl_msg(void* arg) {
    setprio(RECV_PRIO, SCHED_RR);
    struct sockaddr_in* to_addr = (struct sockaddr_in*)arg;
    struct ctrl_msg* msg = (struct ctrl_msg*)malloc(sizeof(struct ctrl_msg));
    while (keep_running) {
        /* receive message */
        socklen_t len = sizeof(to_addr);
        int bytes = recvfrom(socket_desc, msg, sizeof(*msg), 0, (struct sockaddr*)&to_addr, &len);
        if (bytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        /*Tic*/
        auto tic_rcv_ctrl_msg = Clock::now(); //First timestamp, before receiving control message
        start_command_clk = clock();

 
       if (msg->pwm_motor1 == 0 && msg->pwm_motor2 == 0)
        {
//      keep_running=false;
            pwmWrite(enA, msg->pwm_motor1);
            pwmWrite(enB, msg->pwm_motor2);      
            free(msg);
            msg =NULL;
            abort();

        }

        /*Assign direction to the motors*/
        digitalWrite(inA1, msg->switch_signal_0); /* For motor1*/

        digitalWrite(inA2, msg->switch_signal_1);
        digitalWrite(inB1, msg->switch_signal_2); /* For motor2 */
        digitalWrite(inB2, msg->switch_signal_3);

        /*Assign PWM values to the motors*/
        if (keep_running == true)
        {
            pwmWrite(enA, msg->pwm_motor1); /* Motor 1 */
            pwmWrite(enB, msg->pwm_motor2); /* Motor 2 */
            //  printf("pwm 1: %d pwm2: %d\n", msg->pwm_motor1, msg->pwm_motor2);
        }

        else
        {
            break;
        }

 
        /*Toc*/
        auto toc_rcv_ctrl_msg = Clock::now(); //Second timestamp, after receiving control message
        end_command_clk = clock();
        rcvCtrlCPU = (double) (end_command_clk - start_command_clk) / CLOCKS_PER_SEC;
        //std::cout << "Total CPU time for command: " << rcvCtrlCPU << std::endl;
        //std::cout << "Elapsed time receiving ctrl msg: " << duration_cast<microseconds>(toc_rcv_ctrl_msg - tic_rcv_ctrl_msg).count() << std::endl; // Print difference in milliseconds

        /*Save to .csv file*/
        std::ofstream myFile2("rcvCtrlTime.csv", std::ios::app);
        myFile2 << duration_cast<microseconds>(toc_rcv_ctrl_msg - tic_rcv_ctrl_msg).count() << endl;
        std::ofstream myFile3("rcvCtrlCPU.csv", std::ios::app);
        myFile3 << rcvCtrlCPU << endl;
        sleep(JOY_SLEEP);
    }
    free(msg);
    return NULL;
}

void* send_video(void* arg) {
    setprio(SEND_PRIO, SCHED_RR);
    struct sockaddr_in* to_addr = (struct sockaddr_in*)arg;
    int bytes;

    /*Create a video capturing object*/
    cv::VideoCapture video(0);

    /*Change resolution*/
    video.set(CAP_PROP_FPS, 30);
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

       start_video_clk = clock();

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
            end_video_clk = clock();
            sendVideoCPU = (double) (end_command_clk - start_video_clk) / CLOCKS_PER_SEC;
           // std::cout << "Total CPU time for video: " << sendVideoCPU << std::endl;
           // std::cout << "Elapsed time: " << duration_cast<milliseconds>(toc_send_video - tic_send_video).count() << std::endl; // Print difference in milliseconds

            /*Save to .csv file*/
            std::ofstream myFile1("sendVideoTime.csv", std::ios::app);
            myFile1 << duration_cast<microseconds>(toc_send_video - tic_send_video).count() << endl;
            std::ofstream myFile4("sendVideoCPU.csv", std::ios::app);
            myFile4 << sendVideoCPU << endl;
            sleep(VIDEO_SLEEP);
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
    std::ofstream myFile1("sendVideoTime.csv");
    myFile1<<"";
    std::ofstream myFile2("rcvCtrlTime.csv");
    myFile2<<"";
    std::ofstream myFile3("rcvCtrlCPU.csv");
    myFile3<<"";
    std::ofstream myFile4("sendVideoCPU.csv");
    myFile4<<"";

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
    pthread_attr_t recv_attr, send_attr;
    
    if (pthread_attr_init(&send_attr)) {
        printf("error send_attr init\n");
    }
    if (pthread_attr_init(&recv_attr)) {
        printf("error recv_attr init\n");
    }

    pthread_create(&receive_thread, &recv_attr, receive_ctrl_msg, &to_addr); //kom ihåg att sätta attr sen på rt
    pthread_create(&send_thread, &send_attr, send_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(socket_desc);
    return 0;
}

