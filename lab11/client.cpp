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

#define TIMEOUT_TIME 3      // s
#define SERVER_IP "127.0.0.1"

void sleep_ms(unsigned ms);
std::string get_cur_time();
/*including min and including max*/
int rnd(int min, int max);

void do_do(int server_fd);

/*
./client {port}
*/
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Syntax error. Expected: \"> ./client {port}\"" << std::endl;
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
            int sock;
            struct sockaddr_in addr;

            std::cout << "Creating socket... " << std::flush;
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock < 0)
            {
                perror("socket");
                exit(EXIT_FAILURE);
            }
            std::cout << "OK! " << std::endl;

            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = inet_addr(SERVER_IP);
            int start_time = time(NULL);
            int dur = 0;
            std::cout << "Connecting to: " << inet_ntoa(addr.sin_addr) << ":" << htons(addr.sin_port) << "... " << std::flush;
            do
            {
                sleep_ms(rnd(2, 350));
                if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
                {
                    std::cout << "OK! " << std::endl;
                    do_do(sock);
                    break;
                }
                else
                    dur = time(NULL) - start_time;
            }while(dur < TIMEOUT_TIME);
            if(dur >= TIMEOUT_TIME)
                std::cout << "\nTimeout. Exiting... " << std::endl;

            std::cout << "Clossing socket... " << std::flush;
            close(sock);
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

void do_do(int server_fd)
{
    std::cout << "do_do" << std::endl;
    unsigned long a_n = rnd(3, 30);
    int *a = (int*)malloc(sizeof(int)*a_n);
    for(unsigned li = 0; li < a_n; ++li) a[li] = rnd(0, 999);


    unsigned char buffer[8];

    union Long64 msg_size;
    msg_size.l = a_n;
    for(unsigned li = 0; li < 8; ++li)
        buffer[li] = msg_size.c[li];
    
    send(server_fd, buffer, 8, 0);

    send(server_fd, a, a_n*sizeof(int), 0);

    std::cout << "Sended: " << "[ " << std::flush;
        for(unsigned long li = 0; li < a_n; ++li)
            std::cout << a[li] << " " << std::flush;
                std::cout << "] " << std::endl;


    int readed = read(server_fd, a, a_n*sizeof(int));
    if(readed != a_n*sizeof(int))
    {
        std::cout << "Cannot read " << (a_n*sizeof(int)) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Getted: " << "[ " << std::flush;
        for(unsigned long li = 0; li < a_n; ++li)
            std::cout << a[li] << " " << std::flush;
                std::cout << "] " << std::endl;

    free(a);
}

