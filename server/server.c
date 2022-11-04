#include "server.h"
/* To run program ./exe port*/
int main (int argc, char **argv) {
    int port_nr;
    struct sockaddr_in my_addr;
    struct sockaddr_in from_addr;
    int ok;

    struct ctrl_msg *msg = malloc(sizeof(struct ctrl_msg));
     
    /* check command line arguments */
    if (argc != 2) {
      fprintf(stderr, "usage: %s port\n", argv[0]);
      exit(1);
    }
 
    /* extract local port number */
    if(sscanf(argv[1], "%d", &port_nr) != 1) {
      fprintf(stderr, "invalid port %s\n", argv[1]);
      exit(1);
    }
 
    /* create UDP socket */
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0); 
    if(udp_socket == -1) {
      perror("socket");
      exit(1);
    }
    /* bound to any local address on the specified port */
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port_nr);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ok = bind(udp_socket, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (ok == -1) {
      perror("bind");
      exit(1);
    }
    /* allowing broadcast (optional) */
    int on = 1;
    ok = setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));
    if (ok == -1) {
      perror("setsockopt");
      exit(1);
    }
 
    while (1) {
          /* receive message */
          socklen_t len = sizeof(from_addr);
          int r = recvfrom(udp_socket, msg, sizeof(*msg), 0, (struct sockaddr *)&from_addr, &len);
          if (r == -1) {
            perror("recvfrom");
            exit(1);
          }
 
          /* display message and source address/port */
          printf("from %s:%d : pwm_motor1 %d\n", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port), msg->pwm_motor1);
      }
 
    close(udp_socket);
    free(msg);
    return 0;
}