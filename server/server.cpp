#include "server.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "base64.h"
#include "opencv2/highgui.hpp"
#include <vector>
#include <string>
#include <list>

using namespace cv;
using namespace std;
#define START 4
#define MAX_LEN 65535
int socket_desc;
void *receive_ctrl_msg (void *arg) {
    struct sockaddr_in *to_addr = (struct sockaddr_in *)arg;
    struct ctrl_msg *msg = (struct ctrl_msg *)malloc(sizeof(struct ctrl_msg));
    while (1) {
        /* receive message */
        socklen_t len = sizeof(to_addr);
        int bytes = recvfrom(socket_desc, msg, sizeof(*msg), 0, (struct sockaddr *)&to_addr, &len);
        if (bytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        /* display message */
        printf("pwm_motor1 %d\n", msg->pwm_motor1);
    }
    free(msg);
}
void *send_video (void *arg) {
    struct sockaddr_in *to_addr = (struct sockaddr_in *)arg;
    int bytes;

    /*Create a video capturing object*/
    cv::VideoCapture video(0);

    /*Check if camera is opened*/
    if (video.isOpened() == false) {
        exit(1);
    }
    
    /*Create a class to save the frame to*/
    cv::Mat frame;
    
    while (video.read(frame) == true) {
        //frame = Mat::zeros(480, 640, CV_8U);
        /*Encoding, frame-> jpg -> base 64*/
        std::vector<uchar> buf;
        std::vector<int> param(2);
        param[0] = cv::IMWRITE_JPEG_QUALITY;
        param[1] = 20; /* default(95) 0-100 */

        cv::imencode(".jpg", frame, buf, param);                       /* Encode data from class Mat to JPG */
        auto *enc_msg = reinterpret_cast<unsigned char *>(buf.data()); /* Cast the JPG to char* */
        std::string encoded = base64_encode(enc_msg, buf.size());      /* Encode the data to base 64 */
        int size = START + encoded.size();
        if (size <= MAX_LEN) {
            encoded.insert(0,to_string(size));
            //printf("len message %d. First element %c\n", encoded.size(), encoded[0]);
            bytes = sendto(socket_desc, encoded.c_str(), size, 0, (struct sockaddr *)to_addr, sizeof(*to_addr));
            if (bytes == -1) {
                perror("sendto");
            }
            buf.clear();
            enc_msg = NULL;
            encoded.clear();
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int port_nr;
    struct sockaddr_in my_addr;
    struct sockaddr_in to_addr;
    int bytes;

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

    /* extract local port number */
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
    /* allowing broadcast */
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

    pthread_t send_thread, receive_thread;
    pthread_create(&receive_thread, NULL, receive_ctrl_msg, &to_addr);
    pthread_create(&send_thread, NULL, send_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(socket_desc);
    return 0;
}