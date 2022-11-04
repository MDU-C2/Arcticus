#include "client.h"
/* To run program ./exe address of server port*/
int main (int argc, char **argv) {   
    int ok;
    int port_nr;
    struct sockaddr_in to_addr;

    struct ctrl_msg msg = {
        .pwm_motor1 = 100,
        .pwm_motor2 = 255,
        .switch_signal_0 = 0,
        .switch_signal_1 = 1,
        .switch_signal_2 = 0,
        .switch_signal_3 = 1
    };
     
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
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        perror("socket");
        exit(1);
    }
    /* allowing broadcast (optional) */
    int on = 1;
    ok = setsockopt(udp_socket, SOL_SOCKET,SO_BROADCAST, &on, sizeof(int));
    if( ok == -1) {
        perror("setsockopt");
        exit(1);
    }

    /* send message to the specified destination/port */
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port_nr);
    to_addr.sin_addr.s_addr = ip_address;

    ok = sendto(udp_socket, (struct ctrl_msg*)&msg, 1024 + sizeof(msg), 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
    if ( ok == -1) {
        perror("sendto");
        exit(1);
    }
                 
    close(udp_socket);
     
return 0;
}