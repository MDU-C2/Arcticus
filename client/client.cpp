#include "client.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "base64.h"
#include "opencv2/highgui.hpp"
#include <vector>
#include <string.h>

using namespace cv;
using namespace std;

#define MAX_LEN 65535
#define MAX_NR 10

int socket_desc;
void *send_ctrl_msg (void *arg) {
    struct sockaddr_in *to_addr = (struct sockaddr_in*)arg;
    int bytes;
    struct ctrl_msg msg = {};
    msg.pwm_motor1 = 255;
    while (1) {
        bytes = sendto(socket_desc, (struct ctrl_msg*)&msg, sizeof(msg), 0, (struct sockaddr *)to_addr, sizeof(*to_addr));
        if (bytes == -1) {
            perror("sendto");
            exit(1);
        } 
    }
}
void *receive_video (void *arg) {
    struct sockaddr_in *from_addr = (struct sockaddr_in*)arg;
    std::string encoded;   

    while (1) {
        socklen_t len = sizeof(from_addr);

        char str[MAX_LEN];
        char str_nr[MAX_NR];
        int index_stop;
        
        int bytes = recvfrom(socket_desc, str, MAX_LEN, 0,(struct sockaddr *)&from_addr, &len);
        if (bytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        //cout << str << endl;
        for (int i = 0; i < MAX_NR; i++) {
            if (str[i] == '/') {
                index_stop = i;
                break;
            }
        }
        //printf("stop index %d\n",index_stop);
        strncpy(str_nr, str, index_stop);
        str_nr[index_stop] ='\0';
      //  printf("string %s\n",str_nr);
        //printf("int nr %d\n", atoi(str_nr));
        /* Convert to std::string and remove the number in front*/
        encoded = "";
        for (int i = index_stop; i < atoi(str_nr); i++) {
            encoded= encoded + str[i];
        }
        //cout << encoded << endl;
		string dec_jpg = base64_decode(encoded); /* Decode the data to base 64 */
		std::vector<uchar> data(dec_jpg.begin(), dec_jpg.end()); /* Cast the data to JPG from base 64 */
		cv::Mat img = cv::imdecode(cv::Mat(data), 1); /* Decode the JPG data to class Mat */

        cv::imshow("Video feed", img);
        if (cv::waitKey(25) >= 0) {
            break;
        }
        
    }
    return NULL;
}
int main (int argc, char **argv) {   
    int bytes;
    int port_nr;
    struct sockaddr_in to_addr;
    struct sockaddr_in my_addr;

    /* check command line arguments */
    if (argc != 3) { 
        fprintf(stderr, "usage: %s destination port\n", argv[0]);
        exit(1); 
    }
 
    /* extract destination IP address */
    struct hostent *host = gethostbyname(argv[1]);
    
    if (host == NULL) {
        fprintf(stderr, "unknown host %s\n",argv[1]);
        exit(1);
    }
     
    in_addr_t ip_address = *((in_addr_t *)(host->h_addr));
 
    /* extract destination port number */
    if(sscanf(argv[2], "%d",&port_nr) != 1) {
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

    bytes = bind(socket_desc, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (bytes == -1) {
      perror("bind");
      exit(1);
    }
    /* allowing broadcast (optional) */
    int on = 1;
    bytes = setsockopt(socket_desc, SOL_SOCKET,SO_BROADCAST, &on, sizeof(int));
    if( bytes == -1) {
        perror("setsockopt");
        exit(1);
    }

    /* send message to the specified destination/port */
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port_nr);
    to_addr.sin_addr.s_addr = ip_address;

    pthread_t receive_thread, send_thread;
    pthread_create(&send_thread, NULL, send_ctrl_msg, &to_addr);
    pthread_create(&receive_thread, NULL, receive_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(socket_desc); 
    return 0;
}