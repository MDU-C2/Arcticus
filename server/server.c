#include "server.h"
/* To run program ./exe port*/
int socket_desc;
void *receive_ctrl_msg (void *arg) {
    struct sockaddr_in *to_addr = (struct sockaddr_in*)arg;
    struct ctrl_msg *msg = malloc(sizeof(struct ctrl_msg));
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
    struct sockaddr_in *to_addr = (struct sockaddr_in*)arg;
    int bytes;
    struct ctrl_msg msg = {
        .pwm_motor1 = 1,
        .pwm_motor2 = 0,
        .switch_signal_0 = 0,
        .switch_signal_1 = 1,
        .switch_signal_2 = 0,
        .switch_signal_3 = 1
    };
    while (1) {
        bytes = sendto(socket_desc, (struct ctrl_msg*)&msg, sizeof(msg), 0, (struct sockaddr *)to_addr, sizeof(*to_addr));
        if (bytes == -1) {
            perror("sendto");
            exit(1);
        }
    }
}
int main (int argc, char **argv) {
    int port_nr;
    struct sockaddr_in my_addr;
    struct sockaddr_in to_addr;
    int bytes;
     
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

    /* extract local port number */
    if(sscanf(argv[2], "%d", &port_nr) != 1) {
      fprintf(stderr, "invalid port %s\n", argv[2]);
      exit(1);
    }
 
    /* create UDP socket */
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0); 
    if(socket_desc == -1) {
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

    pthread_t send_thread,receive_thread;
    pthread_create(&receive_thread, NULL, receive_ctrl_msg, &to_addr);
    pthread_create(&send_thread, NULL, send_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

 
    close(socket_desc);
    return 0;
}