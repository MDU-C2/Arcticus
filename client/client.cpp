#include "client.h"
#include <X11/Xlib.h>
using namespace cv;
using namespace std;
using namespace std::chrono;
using Clock = std::chrono::steady_clock;

#define MAX_LEN 65535
#define MAX_NR 10


#define SEND_SLEEP 0.015 //15ms
#define RECV_SLEEP 0.015 //15ms
#define SEND_PRIO 79//low 98
#define RECV_PRIO 80//high 99

int socket_desc;
struct sockaddr_in global_to_addr;

/*Parameters for CPU time clock*/
clock_t start_joystick_clk, end_joystick_clk, start_video_clk, end_video_clk;
double sendCtrlCPU, rcvVideoCPU;

static void setprio(int prio, int sched) {
    struct sched_param param;
    // Set realtime priority for this thread 
    param.sched_priority = prio;
    if (sched_setscheduler(0, sched, &param) < 0)
        perror("sched_setscheduler");
}

static volatile int keep_running = true;
void handler(int arg)
{
    keep_running = false;
    struct ctrl_msg control_signal = {};
    int bytes = sendto(socket_desc, (struct ctrl_msg *)&control_signal, sizeof(control_signal), 0, (struct sockaddr *)&global_to_addr, sizeof(global_to_addr));
    if (bytes == -1)
    {
        perror("sendto");
        exit(1);
    }
    printf("pwm1: %d pwm2: %d\n", control_signal.pwm_motor1, control_signal.pwm_motor2);
    abort();
}
int lin_map(float value, float x_0, float y_0, float x_1, float y_1)
{
    int y;
    float k = (y_1 - y_0) / (x_1 - x_0);
    float m = y_0 - k * x_0;
    y = k * value + m;
    if (y > 1024)
    {
        y = 1024;
    }
    return y;
}
void *send_ctrl_msg(void *arg) {
    setprio(SEND_PRIO, SCHED_RR);
    struct sockaddr_in *to_addr = (struct sockaddr_in *)arg;
    int bytes;
    struct ctrl_msg control_signal = {};
    int back[] = {0, 1};
    int forward[] = {1, 0};
    int scaling = 1;

    /* for movement */
    sf::Vector2f speed = sf::Vector2f(0.f, 0.f);

    while (keep_running) {
        sf::Joystick::update();
        /*Tic*/
        auto tic_send_ctrl_msg = Clock::now(); // First timestamp, before sending
        start_joystick_clk = clock(); // First timestamp for sending ctrl msg in CPU time

        // std::cout << "X axis: " << speed.x << std::endl;
        // std::cout << "Y axis: " << speed.y << std::endl;

        if (speed.y > 0) { /* drive forward */
            control_signal.switch_signal_0 = forward[0];
            control_signal.switch_signal_1 = forward[1];
            control_signal.switch_signal_2 = forward[0];
            control_signal.switch_signal_3 = forward[1];
            scaling = 1;
        } else { /* drive backwards */
            control_signal.switch_signal_0 = back[0];
            control_signal.switch_signal_1 = back[1];
            control_signal.switch_signal_2 = back[0];
            control_signal.switch_signal_3 = back[1];
            scaling = 1;
        }
        int abs_vel = sqrt((speed.x * speed.x) + (speed.y * speed.y));
        int abs_mapped = lin_map(abs_vel, 0, 200, 100, 1024);
        if (keep_running == true) {
            if (speed.x > 0) {
                control_signal.pwm_motor1 = scaling * ((100 - speed.x) / 100) * abs_mapped;
                control_signal.pwm_motor2 = scaling * abs_mapped;
            } else {
                control_signal.pwm_motor2 = scaling * ((100 + speed.x) / 100) * abs_mapped;
                control_signal.pwm_motor1 = scaling * abs_mapped;
            }
        } else {
            control_signal.pwm_motor1 = 0;
            control_signal.pwm_motor1 = 0;
            bytes = sendto(socket_desc, (struct ctrl_msg *)&control_signal, sizeof(control_signal), 0, (struct sockaddr *)to_addr, sizeof(*to_addr));
            if (bytes == -1) {
                perror("sendto");
                exit(1);
            }
            exit(1);
        }
        //  std::cout << "pwm1: " << control_signal.pwm_motor1 << std::endl;
        //  std::cout << "pwm2: " << control_signal.pwm_motor2 << std::endl;

        speed = sf::Vector2f(sf::Joystick::getAxisPosition(0, sf::Joystick::X), sf::Joystick::getAxisPosition(0, sf::Joystick::Y));
        bytes = sendto(socket_desc, (struct ctrl_msg *)&control_signal, sizeof(control_signal), 0, (struct sockaddr *)to_addr, sizeof(*to_addr));

        if (bytes == -1) {
            perror("sendto");
            exit(1);
        }
        if (cv::waitKey(1) >= 0) {
            control_signal.pwm_motor1 = 0;
            control_signal.pwm_motor2 = 0;
        }
        /*Toc*/
        auto toc_send_ctrl_msg = Clock::now(); // Second timestamp, after sending ctrl message
        end_joystick_clk = clock();            // Second timestamp, after sending ctlr message in CPU time 
        sendCtrlCPU = (double) (end_joystick_clk - start_joystick_clk) / CLOCKS_PER_SEC;
        //std::cout << "Total CPU time for joystick: " << sendCtrlCPU << std::endl;
        // std::cout << "Elapsed time sending ctrl message: " << duration_cast<microseconds>(toc_send_ctrl_msg - tic_send_ctrl_msg).count() << std::endl; // Print difference in milliseconds

        /*Save to .csv file*/
        std::ofstream myFile3("sendCtrlTime.csv", std::ios::app);
        myFile3 << duration_cast<microseconds>(toc_send_ctrl_msg - tic_send_ctrl_msg).count() << endl;
        std::ofstream myFile4("sendCtrlCPU.csv", std::ios::app );
        myFile4 << sendCtrlCPU << endl;

        sleep(SEND_SLEEP);
    }
    return NULL;
}
void *receive_video(void *arg)
{
    setprio(RECV_PRIO, SCHED_RR);
    struct sockaddr_in *from_addr = (struct sockaddr_in *)arg;
    std::string encoded;

    while (keep_running)
    {
        socklen_t len = sizeof(from_addr);

        char str[MAX_LEN];
        char str_nr[MAX_NR];
        int index_stop;

        /*Receive video message*/
        int bytes = recvfrom(socket_desc, str, MAX_LEN, 0, (struct sockaddr *)&from_addr, &len);
        if (bytes == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        /*Tic*/
        start_video_clk = clock(); // First timestamp, before receiving video in CPU time
        auto tic_rcv_video = Clock::now(); // First timestamp, before receiving video

        /*Ugly fix to what?*/
        for (int i = 0; i < MAX_NR; i++)
        {
            if (str[i] == '/')
            {
                index_stop = i;
                break;
            }
        }

        /*Ugly fix to what?*/
        strncpy(str_nr, str, index_stop);
        str_nr[index_stop] = '\0';
        /* Convert to std::string and remove the number in front */
        encoded = "";
        for (int i = index_stop; i < atoi(str_nr); i++)
        {
            encoded = encoded + str[i];
        }

        /*Decode part*/
        string dec_jpg = base64_decode(encoded);                 /* Decode the data to base 64 */
        std::vector<uchar> data(dec_jpg.begin(), dec_jpg.end()); /* Cast the data to JPG from base 64 */
        cv::Mat img = cv::imdecode(cv::Mat(data), 1);            /* Decode the JPG data to class Mat */

        /*Display the video frames*/
        
        cv::imshow("Video feed", img);

        /*Toc*/
        end_video_clk = clock();           // Second timestamp, after receieving video in CPU time
        auto toc_rcv_video = Clock::now(); // Second timestamp, after recieving video


        rcvVideoCPU = (double) (end_video_clk - start_video_clk) / CLOCKS_PER_SEC;
        
        //std::cout << "Total CPU time for video: " << rcvVideoCPU << std::endl;
       // std::cout << "Elapsed time receiving video: " << duration_cast<milliseconds>(toc_rcv_video - tic_rcv_video).count() << std::endl; // Print difference in milliseconds

        /*Save to .csv file*/
        std::ofstream myFile2("rcvVideoTime.csv", std::ios::app);
        myFile2 << duration_cast<microseconds>(toc_rcv_video - tic_rcv_video).count() << endl;
        std::ofstream myFile5("rcvVideoCPU.csv", std::ios::app );
        myFile5 << rcvVideoCPU << endl;
        sleep(RECV_SLEEP);
    }
    return NULL;
}
int main(int argc, char **argv)
{
    XInitThreads();
    signal(SIGINT, handler); /* handles ctrl+C signal */
    int bytes;
    int port_nr;
    struct sockaddr_in to_addr;
    struct sockaddr_in my_addr;

    /*Clear the .csv files*/
    std::ofstream myFile2("rcvVideoTime.csv");
    myFile2 << "";
    std::ofstream myFile3("sendCtrlTime.csv");
    myFile3 << "";
    std::ofstream myFile4("sendCtrlCPU.csv");
    myFile4 << "";
    std::ofstream myFile5("rcvVideoCPU.csv");
    myFile5 << "";

    /* check command line arguments */
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s destination port\n", argv[0]);
        exit(1);
    }

    /* extract destination IP address */
    struct hostent *host = gethostbyname(argv[1]);

    if (host == NULL)
    {
        fprintf(stderr, "unknown host %s\n", argv[1]);
        exit(1);
    }

    in_addr_t ip_address = *((in_addr_t *)(host->h_addr));

    /* extract destination port number */
    if (sscanf(argv[2], "%d", &port_nr) != 1)
    {
        fprintf(stderr, "invalid port %s\n", argv[2]);
        exit(1);
    }
    /* create UDP socket */
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc == -1)
    {
        perror("socket");
        exit(1);
    }
    /* bound to any local address on the specified port */
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port_nr);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bytes = bind(socket_desc, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (bytes == -1)
    {
        perror("bind");
        exit(1);
    }
    /* allowing broadcast (optional) */
    int on = 1;
    bytes = setsockopt(socket_desc, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));
    if (bytes == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    /* send message to the specified destination/port */
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port_nr);
    to_addr.sin_addr.s_addr = ip_address;
    global_to_addr = to_addr;

    pthread_t receive_thread, send_thread;
    pthread_attr_t recv_attr, send_attr;
    
    if (pthread_attr_init(&send_attr)) {
        printf("error send_attr init\n");
    }
    if (pthread_attr_init(&recv_attr)) {
        printf("error recv_attr init\n");
    }
      

    pthread_create(&send_thread, &send_attr, send_ctrl_msg, &to_addr);
    pthread_create(&receive_thread, &recv_attr, receive_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);
 
    close(socket_desc);
    return 0;
}