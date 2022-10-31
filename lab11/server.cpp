#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream>

#define TIMEOUT_TIME 3000
#define LISTEN_NUM 5
#define MAXEVENTS 150

void sleep_ms(unsigned ms);
std::string get_cur_time();
/*including min and including max*/
int rnd(int min, int max);

void client_handler(int client_fd, unsigned handler_id);

void sort(int* a, unsigned long n);

/*
./server {port}
*/
int main(int argc, char* argv[])
{
    unsigned gi = 1;

    if(argc != 2)
    {
        std::cout << "Syntax error. Expected: \"> ./server {port}\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int port = atoi(argv[1]);
    if(port <= 1024)
    {
        std::cout << "Choose another {port}. " << std::endl;
        exit(EXIT_FAILURE);
    }

    srandom(time(NULL));
    int ret_buff;
    
    // =============== SOCKET ===================
        int server_fd, new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);

        

        std::cout << "Creating socket... " << std::flush;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        std::cout << "OK! " << std::endl;

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        std::cout << "Binding socket: PORT=" << port << "... " << std::flush;
        if (bind(server_fd, (struct sockaddr*)&address,
                sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        std::cout << "OK! " << std::endl;

        std::cout << "Starting listen... " << std::flush;
        if (listen(server_fd, LISTEN_NUM) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        std::cout << "OK! " << std::endl;

        // =============== EPOLL ===================
                struct epoll_event ev, *trig_events;
                trig_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*MAXEVENTS);
                int epfd = epoll_create(MAXEVENTS);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = server_fd;
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) < 0)
                {
                    fprintf(stderr, "epoll set insertion error: fd=%d", server_fd);
                    perror("epoll_ctl");
                    exit(EXIT_FAILURE);
                }

                bool STOP_RUNNING = false;
                do
                {
                    std::cout << "Waiting for events..." << std::endl;
                    int nfds = epoll_wait(epfd, trig_events, MAXEVENTS, TIMEOUT_TIME);
                    std::cout << "Waited for " << nfds << " events..." << std::endl;
                    if(nfds == -1)
                    {
                        perror("epoll_wait");
                        exit(EXIT_FAILURE);
                    }
                    else if(nfds > 0)
                    {
                        for(int li = 0; li < nfds; ++li)
                        {
                            if(trig_events[li].data.fd == server_fd)
                            {
                                std::cout << "A client want to connect. " << std::endl;
                                int client = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                                if(client < 0)
                                {
                                    perror("Client accept");
                                    exit(EXIT_FAILURE);
                                }
                                std::cout << "New connection: " << inet_ntoa(address.sin_addr) << ":" << htons(address.sin_port) << std::endl;
                                
                                ret_buff = fork();
                                if(ret_buff < 0)
                                {
                                    perror("Cannot fork");
                                    exit(EXIT_FAILURE);
                                }
                                else if(ret_buff == 0)
                                {
                                    client_handler(client, gi);
                                    exit(EXIT_SUCCESS);
                                }
                                ++gi;
                            }
                            else
                            {
                                std::cout << "Something gone wrong! " << std::endl;
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    else if(nfds == 0)
                    {
                        STOP_RUNNING = true;
                        std::cout << "HuKTO HE nPuWEJl=(. Shutting down... " << std::endl;
                    }
                }while(!STOP_RUNNING);
        // =============== EPOLL ===================

        std::cout << "Clossing server... " << std::flush;
        shutdown(server_fd, SHUT_RDWR);
        std::cout << "OK! " << std::endl;

        std::cout << "========== Done! ==========" << std::endl;
    // =============== SOCKET ===================

    return 0;
}



std::string get_cur_time()
{
    long long long_long_long_long_very_long_value_long_long = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
    return "[" + std::to_string(long_long_long_long_very_long_value_long_long) + "] ";
}

void sleep_ms(unsigned ms)
{
    // mdelay(write_delay_after); // активное ожидание, хз где: <linux/delay.h>? <ams/delay.h>? <sys/delay.h>? 

    // std::this_thread::sleep_for(std::chrono::milliseconds(x)); //   #include <chrono>    and    #include <thread>

    if(usleep(1000*ms) < 0)
        perror("usleep error");
}

int rnd(int min, int max)
{
    return random() % (max-min+1) + min;
}


union Long64
{
   unsigned long l;
   unsigned char c[8];
};

void client_handler(int client_fd, unsigned handler_id)
{
    std::ostringstream ss;

    unsigned char buffer[8];
    ssize_t readed;

    readed = read(client_fd, buffer, 8);
    if(readed != 8)
    {
        std::cout << "Cannot read 8" << std::endl;
        exit(EXIT_FAILURE);
    }

    union Long64 msg_size;
    for(unsigned li = 0; li < 8; ++li)
        msg_size.c[li] = buffer[li];
    unsigned long sz = msg_size.l;

    int *a = (int*)malloc(sizeof(int)*sz);

    readed = read(client_fd, a, sz*sizeof(int));
    if(readed != sz*sizeof(int))
    {
        std::cout << "Cannot read " << (sz*sizeof(int)) << std::endl;
        exit(EXIT_FAILURE);
    }

    ss << "Getted (id=" << handler_id << "): " << "[ " << std::flush;
        for(unsigned long li = 0; li < sz; ++li)
            ss << a[li] << " " << std::flush;
                    ss << "] " << std::endl;

    sort(a, sz);

    send(client_fd, a, sz*sizeof(int), 0);

    ss << "Sended (id=" << handler_id << "): " << "[ " << std::flush;
        for(unsigned long li = 0; li < sz; ++li)
            ss << a[li] << " " << std::flush;
                    ss << "] " << std::endl;

    std::cout << ss.str() << std::endl;

    free(a);
    close(client_fd);
}

void sort(int* a, unsigned long n)
{
    int buff;
    for(unsigned long i = 0; i < n-1; ++i)
        for(unsigned long j = i+1; j < n; ++j)
            if(a[i] > a[j])
            {
                buff = a[i];
                a[i] = a[j];
                a[j] = buff;
            }
}