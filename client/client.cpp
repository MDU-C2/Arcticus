#include "client.h"

using namespace cv;
using namespace std;

#define MAX_LEN 65535
#define MAX_NR 10
#define JOY_SLEEP 0.008      //prio 1 lägst
#define VIDEO_SLEEP 0.002 //prio 2 högst
#define PRE_ALLOCATION_SIZE 10000*1024*1024 /* Öka om du får page faults 100MB pagefault free buffer */

int socket_desc;
struct sockaddr_in global_to_addr;

static volatile int keep_running = true;




__thread void* stack_start;
__thread long stack_max_size = 0L;

void check_stack_size() {
  // address of 'nowhere' approximates end of stack
  char nowhere;
  void* stack_end = (void*)&nowhere;
  // may want to double check stack grows downward on your platform
  long stack_size = (long)stack_start - (long)stack_end;
  // update max_stack_size for this thread
  if (stack_size > stack_max_size)
    stack_max_size = stack_size;
}

void handler (int arg) {
    keep_running = false;
    struct ctrl_msg control_signal = {};
    int bytes = sendto(socket_desc, (struct ctrl_msg*)&control_signal, sizeof(control_signal), 0, (struct sockaddr*)&global_to_addr, sizeof(global_to_addr));
    if (bytes == -1) {
        perror("sendto");
        exit(1);
    }
    exit(1);
}
int lin_map (float value, float x_0, float y_0, float x_1, float y_1) {
    int y;
    float k = (y_1 - y_0)/(x_1 - x_0);
    float m = y_0 - k*x_0;
    y = k*value + m;
    if (y > 1024) {
        y = 1024;
    }
    return y;
}
static void setprio(int prio, int sched)
{
    struct sched_param param;
    // Set realtime priority for this thread
    param.sched_priority = prio;
    if (sched_setscheduler(0, sched, &param) < 0)
        perror("sched_setscheduler");
}
void configure_malloc_behavior (void) {
    /* Now lock all current and future pages 
        from preventing of being paged */
    if (mlockall(MCL_CURRENT | MCL_FUTURE))
        perror("mlockall failed:");

    /* malloc trimming and mmap will generate page faults. Therefore turn them off*/
    /* Turn off malloc trimming. malloc trimming will generate calls to sbrk*/
    mallopt(M_TRIM_THRESHOLD, -1);

    /* Turn off mmap usage. */
    mallopt(M_MMAP_MAX, 0);
}
void show_new_pagefault_count(const char* logtext, 
                const char* allowed_maj,
                const char* allowed_min)
{
    static int last_majflt = 0, last_minflt = 0;
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);

    printf("%-30.30s: Pagefaults, Major:%ld (Allowed %s), " \
            "Minor:%ld (Allowed %s)\n", logtext,
            usage.ru_majflt - last_majflt, allowed_maj,
            usage.ru_minflt - last_minflt, allowed_min);

    last_majflt = usage.ru_majflt; 
    last_minflt = usage.ru_minflt;
}
static void reserve_process_memory(int size)
{
    int i;
    char *buffer = (char*)malloc(size);

    /* Touch each page in this piece of memory to get it mapped into RAM */
    for (i = 0; i < size; i += sysconf(_SC_PAGESIZE)) {
        /* Each write to this buffer will generate a pagefault.
            Once the pagefault is handled a page will be locked in
            memory and never given back to the system. */
        buffer[i] = 0;
    }

    /* buffer will now be released. As Glibc is configured such that it 
        never gives back memory to the kernel, the memory allocated above is
        locked for this process. All malloc() and new() calls come from
        the memory pool reserved and locked above. Issuing free() and
        delete() does NOT make this locking undone. So, with this locking
        mechanism we can build C++ applications that will never run into
        a major/minor pagefault, even with swapping enabled. */
    free(buffer);
}
void* send_ctrl_msg (void* arg) {
    char nowhere;
    stack_start = (void*)&nowhere;
    setprio(sched_get_priority_max(SCHED_RR)-1, SCHED_RR);
    show_new_pagefault_count("Caused by creating send-thread", ">=0", ">=0");
    struct sockaddr_in* to_addr = (struct sockaddr_in*)arg;
    int bytes;
    struct ctrl_msg control_signal = {};
    int back[] = { 0, 1};
    int forward[] = { 1, 0};
    int scaling = 1;
    struct rusage usage;

    /* window */
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Joystick Use", sf::Style::Default);
    sf::Event e;

    sf::Joystick::Identification id = sf::Joystick::getIdentification(0);
    std::cout << "\nVendor ID: " << id.vendorId << "\nProduct ID: " << id.productId << std::endl;
    sf::String controller("Joystick Use: " + id.name);
    window.setTitle(controller);

    window.setVisible(false);
    /* query joystick for settings if it's plugged in */
    if (sf::Joystick::isConnected(0)) {
        /* check how many buttons joystick number 0 has */
        unsigned int button_count = sf::Joystick::getButtonCount(0);

        /* check if joystick number 0 has a Z axis */
        bool haz_z = sf::Joystick::hasAxis(0, sf::Joystick::Z);

        std::cout << "Button count: " << button_count << std::endl;
        std::cout << "Has a z-axis: " << haz_z << std::endl;

    }

    /* for movement */
    sf::Vector2f speed = sf::Vector2f(0.f,0.f);

    while (window.pollEvent(e) || keep_running) {
       // std::cout << "X axis: " << speed.x << std::endl;
       // std::cout << "Y axis: " << speed.y << std::endl;
        //tic
        if (speed.y > 0){ /* drive forward */
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
            scaling = 0.1;
        }
        int abs_vel = sqrt((speed.x*speed.x) + (speed.y*speed.y));
        int abs_mapped = lin_map(abs_vel, 0, 200, 100, 1024);
        if (keep_running == true){
            if (speed.x > 0) {
                control_signal.pwm_motor1 = scaling*((100 - speed.x)/100)*abs_mapped;
                control_signal.pwm_motor2 = scaling*abs_mapped;
            } else {
                control_signal.pwm_motor2 = scaling*((100 + speed.x)/100)*abs_mapped;
                control_signal.pwm_motor1 = scaling*abs_mapped;
            }
        } else {
            control_signal.pwm_motor1 = 0;
            control_signal.pwm_motor1 = 0;
            bytes = sendto(socket_desc, (struct ctrl_msg*)&control_signal, sizeof(control_signal), 0, (struct sockaddr*)to_addr, sizeof(*to_addr));
            if (bytes == -1) {
                perror("sendto");
                exit(1);
            }
            exit(1);
        }
        std::cout << "pwm1: " << control_signal.pwm_motor1 << std::endl;
        std::cout << "pwm2: " << control_signal.pwm_motor2 << std::endl;

        speed = sf::Vector2f(sf::Joystick::getAxisPosition(0, sf::Joystick::X), sf::Joystick::getAxisPosition(0, sf::Joystick::Y));
        //toc
        bytes = sendto(socket_desc, (struct ctrl_msg*)&control_signal, sizeof(control_signal), 0, (struct sockaddr*)to_addr, sizeof(*to_addr));
        if (bytes == -1) {
            perror("sendto");
            exit(1);
        }
        if (cv::waitKey(25) >= 0) {
            control_signal.pwm_motor1 = 0;
            control_signal.pwm_motor2 = 0;
        }
        sleep(JOY_SLEEP);
        break; //ta bort sen
       // getrusage(RUSAGE_SELF, &usage);
       // printf("Major-pagefaults:%ld, Minor Pagefaults:%ld\n", usage.ru_majflt, usage.ru_minflt);
    }
    show_new_pagefault_count("Caused by using send-thread stack", "0", "0");
    //check_stack_size();
    //printf("stack size %ld\n", stack_max_size);
    return NULL;
}
void* receive_video (void* arg) {
    setprio(sched_get_priority_max(SCHED_RR), SCHED_RR);
    show_new_pagefault_count("Caused by creating recv-thread", ">=0", ">=0");
    struct sockaddr_in* from_addr = (struct sockaddr_in*)arg;
    std::string encoded;
    
    while (keep_running) {
        socklen_t len = sizeof(from_addr);

        char str[MAX_LEN];
        char str_nr[MAX_NR];
        int index_stop;

        int bytes = recvfrom(socket_desc, str, MAX_LEN, 0, (struct sockaddr*)&from_addr, &len);
        //tic + antingen försumma eller 0.1ms
        if (bytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        
        for (int i = 0; i < MAX_NR; i++) {
            if (str[i] == '/') {
                index_stop = i;
                break;
            }
        }
        strncpy(str_nr, str, index_stop);
        str_nr[index_stop] = '\0';
        /* Convert to std::string and remove the number in front */
        encoded = "";
        for (int i = index_stop; i < atoi(str_nr); i++) {
            encoded = encoded + str[i];
        }
        string dec_jpg = base64_decode(encoded); /* Decode the data to base 64 */
        std::vector<uchar> data(dec_jpg.begin(), dec_jpg.end()); /* Cast the data to JPG from base 64 */
        cv::Mat img = cv::imdecode(cv::Mat(data), 1); /* Decode the JPG data to class Mat */

        cv::imshow("Video feed", img);
        //toc
        sleep(VIDEO_SLEEP);
        break; //ta bort sen
    }
    return NULL;
}
int main(int argc, char** argv) {
    signal(SIGINT, handler); /* handles ctrl+C signal */
    int bytes;
    int port_nr;
    struct sockaddr_in to_addr;
    struct sockaddr_in my_addr;

    show_new_pagefault_count("Initial count", ">=0", ">=0");
    /* Memory locking. Lock all current and future pages from preventing of being paged */
    configure_malloc_behavior();
    show_new_pagefault_count("mlockall() generated", ">=0", ">=0");
    //reserve_process_memory(PRE_ALLOCATION_SIZE);
       

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

    /* extract destination port number */
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
    /* allowing broadcast (optional) */
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
    global_to_addr = to_addr;

    /* create rt-threads */
    pthread_t recv_thread, send_thread;
    pthread_attr_t recv_attr, send_attr;

    if (pthread_attr_init(&send_attr))
        printf("error send_attr init\n");
    if (pthread_attr_setstacksize(&send_attr, 83886080))
   		printf("error stack size send-thread\n");

    if (pthread_attr_init(&recv_attr))
        printf("error recv_attr init\n");
    if (pthread_attr_setstacksize(&recv_attr, 83886080))
   		printf("error stack size recv-thread\n");



    pthread_create(&send_thread, NULL, send_ctrl_msg, &to_addr);
    pthread_create(&recv_thread, NULL, receive_video, &to_addr);
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(socket_desc);
    return 0;
}