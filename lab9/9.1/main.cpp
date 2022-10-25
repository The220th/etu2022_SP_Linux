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
#include <sys/wait.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

#define SYNC_ON 1             // Если SYNC_ON == 0, то выключить Алгоритм Лампорта и писать, когда вздумается
#define TERMINAL_OUT_ON 1       // Если TERMINAL_OUT_ON == 0, то запись будет в файл, иначе в терминал
#define FILE_NAME "out.txt"
#define Q_MAIN_OWNER 0
#define ARRAY_LEN 6
            /*
                ARRAY_LEN = len(choosing) + len(number) = 3 + 3, where:
                0* choosing_0
                1* choosing_1
                2* choosing_2
                3* number_0
                4* number_1
                5* number_2
            */



void sleep_ms(unsigned ms);
int create_mount(unsigned prog_num, int shmem_key, int **shmem_p);
int max_num(const int* a, unsigned n);
/*return < 0, if 1 < 2; return == 0, if 1 == 2; return > 0 if 1 > 2*/
int compare_tuple2(int a1, int b1, int a2, int b2);
/*true if a < c or a == c and b < d*/
bool compare_less(int a, int b, int c, int d);
void write_to_file(std::fstream &fd, unsigned prog_num);
void write_to_file(int fd, unsigned prog_num);
std::string get_cur_time();


std::string print_arr(const int* a, unsigned n);



/*
> ./main {prognum_num} {shared_mem_key} {repeat_count} {delay_ms}
*/
int main(int argc, char* argv[])
{
    if(argc != 5)
    {
        std::cout << "Syntax error. Expected: \"> ./main {prognum_num} {shared_mem_key} {repeat_count} {delay_ms}\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    const unsigned prog_num = (unsigned)atoi(argv[1]);
    const int shmem_key = atoi(argv[2]);
    const int repeat_count = atoi(argv[3]);
    const unsigned delay = (unsigned)atoi(argv[4]);
    if(   !(prog_num == 0 || prog_num == 1 || prog_num == 2)   )
    {
        std::cout << "{prog_num} must be 0, 1 or 2. " << std::endl;
        exit(EXIT_FAILURE);
    }
    if(shmem_key <= 0)
    {
        std::cout << "{shared_mem_key} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }
    if(repeat_count <= 0)
    {
        std::cout << "{repeat_count} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }
    if(atoi(argv[4]) <= 0)
    {
        std::cout << "{delay_ms} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }

    srandom(time(NULL) + prog_num);

    //std::fstream file_out;
    //file_out.open(FILE_NAME, std::ios::out | std::ios::ate);
    int fd = open(FILE_NAME, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);

    int *shmem_p;
    const int shmem_id = create_mount(prog_num, shmem_key, &shmem_p);
    std::cout << get_cur_time() << "Getted addresses:" << std::endl;
    std::cout << "Shared memory (key = " << shmem_key << ", id = " << shmem_id << "): " << shmem_p << std::endl;
    sleep(1);

    const unsigned len = ARRAY_LEN / 2;
    int *choose_p = shmem_p+0;
    int *number_p = shmem_p+len;

        #if SYNC_ON != 0
        // ================    Алгоритм Лампорта begin  ==============================

        for(unsigned li = 0; li < repeat_count; ++li)
        {
            choose_p[prog_num] = 1;
            number_p[prog_num] = max_num(number_p, len)+1;
            choose_p[prog_num] = 0;

            for(unsigned j = 0; j < len; ++j)
                if(j != prog_num)
                {
                    while(choose_p[j] == 1);

                    while(  number_p[j] != 0
                                             && compare_tuple2(number_p[j], j, number_p[prog_num], prog_num) < 0 );
                                             //&& compare_less(number_p[j], j, number_p[prog_num], prog_num)   );
                }
            
            //   Критическая секция - начало
            #if TERMINAL_OUT_ON == 0
            std::cout << get_cur_time() << "prog" << prog_num << " enter in critical section - " << print_arr(number_p, len) << std::endl;
            #endif

                    //write_to_file(file_out, prog_num);
                    write_to_file(fd, prog_num);

            #if TERMINAL_OUT_ON == 0
            std::cout << get_cur_time() << "prog" << prog_num << " leave from critical section - " << print_arr(number_p, len) << std::endl;
            #endif
            //   Критическая секция - конец

            number_p[prog_num] = 0;
        }

        // ================    Алгоритм Лампорта end  ================================
        #else
            // ================    Наобум begin  ================================
            for(unsigned li = 0; li < repeat_count; ++li)
                //write_to_file(file_out, prog_num);
                write_to_file(fd, prog_num);
            // ================    Наобум end  ==================================
        #endif

    //file_out.close();
    if(close(fd) < 0)
        perror("Cannot close file");

    std::cout << get_cur_time() << "Unmounting shared memory with id = " << shmem_id << "... " << std::flush;
        if(shmdt(shmem_p) < 0)
        {
            perror("shmdt failed");
            exit(EXIT_FAILURE);
        }
    std::cout << "OK! " << std::endl;

    if(prog_num == Q_MAIN_OWNER)
    {
        std::cout << get_cur_time() << "Destroyed shared memory with id = " << shmem_id << "... " << std::flush;
            if(shmctl(shmem_id, IPC_RMID, NULL) < 0)
            {
                perror("shmdt failed");
                exit(EXIT_FAILURE);
            }
        std::cout << "OK! " << std::endl;
    }

    std::cout << "============================== prog" << prog_num << " DONE! ==============================" << std::endl;
    return 0;
}


int create_mount(unsigned prog_num, int shmem_key, int **shmem_p)
{
    const unsigned page_sz = 4*1024; // размер страницы 4 кБ
    int buff;
    int *buffp;
    int shmem_id;
    const unsigned needed_bytes = ARRAY_LEN*4;
    const unsigned shmem_len = (needed_bytes%page_sz==0?needed_bytes:(needed_bytes/page_sz)*page_sz+page_sz);
    
    std::cout << get_cur_time() << "Len of int array is " << ARRAY_LEN << ". So shared memory size will be " << shmem_len << " bytes. " << std::endl;
    

    if(prog_num == Q_MAIN_OWNER)
    {
        buff = shmget(shmem_key, shmem_len, 0666 | IPC_CREAT | IPC_EXCL);
        if(buff == -1)
        {
            std::cout << "Cannot create shared memory with key = " << shmem_key << ", because it is already exist, choose another one. " << std::endl;
            perror("shmget failed");
            exit(EXIT_FAILURE);
        }
        shmem_id = buff;
        std::cout << get_cur_time() << "A shared memory with key = " << shmem_key << " has been created. Its id = " << buff << ". Mounting... " << std::endl;
    }
    else
    {
        do
        {
            buff = shmget(shmem_key, shmem_len, IPC_EXCL);
            if(buff == -1)
            {
                std::cout << get_cur_time() << "Shared memory with key = " << shmem_key << " doesn't exist yet. Waiting..." << std::endl;
                sleep(1);
            }
        }while(buff == -1);
        shmem_id = buff;
        std::cout << get_cur_time() << "There was an attachment to the shared memory with key = " << shmem_key << ". Its id = " << buff << ". " << std::endl;
    }


    buffp = (int*)shmat(buff, NULL, 0);
    if(buffp == NULL)
    {
        std::cout << "Cannot mount=<" << std::endl;
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    *shmem_p = buffp;

    if(prog_num == Q_MAIN_OWNER)
        for(unsigned li = 0; li < ARRAY_LEN; ++li)
            buffp[li] = 0;

    return shmem_id;
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

int max_num(const int* a, unsigned n)
{
    int res = -1;
    for(unsigned li = 0; li < n; ++li)
        if(res < a[li])
            res = a[li];
    return res;
}

int compare_tuple2(int a1, int b1, int a2, int b2)
{
    if(a1 != a2)
        return a1-a2;
    else
        return b1-b2;
}

bool compare_less(int a, int b, int c, int d)
{
    return (a < c) || (a == c && b < d);
}

void write_to_file(std::fstream &fd, unsigned prog_num)
{
    unsigned long write_delay_after = random() % 1500+1;

    #if TERMINAL_OUT_ON == 1
    std::cout << get_cur_time() << "prog" << prog_num << " write: " << std::flush;
    #else
    fd << get_cur_time() << "prog" << prog_num << " write: " << std::flush;
    #endif

    unsigned N = random() % 15 + 1;
    for(unsigned i = 0; i < N; ++i)
    {
        #if TERMINAL_OUT_ON == 1
        std::cout << std::to_string(random() % 1000) + " " << std::flush;
        #else
        fd << std::to_string(random() % 1000) + " " << std::flush;
        #endif
        if(random() % 2 == 0)
            sleep_ms(random() % 150);
    }

    #if TERMINAL_OUT_ON == 1
    std::cout << std::endl;
    #else
    fd << std::endl;
    #endif
    
    sleep_ms(write_delay_after);
}

void write_to_file(int fd, unsigned prog_num)
{
    unsigned long write_delay_after = random() % 1500+1;

    #if TERMINAL_OUT_ON == 1
    int ffd = 1;
    #else
    int ffd = fd;
    #endif

    std::string buffS;

    buffS = get_cur_time() + "prog" + std::to_string(prog_num) + " write: ";
    write(ffd, buffS.c_str(), buffS.length());

    unsigned N = random() % 15 + 1;
    for(unsigned i = 0; i < N; ++i)
    {
        buffS = std::to_string(random() % 1000) + " ";
        write(ffd, buffS.c_str(), buffS.length());
        if(random() % 2 == 0)
            sleep_ms(random() % 150);
    }

    write(ffd, "\n", 1);
    
    sleep_ms(write_delay_after);
}

std::string print_arr(const int* a, unsigned n)
{
    std::string res = "[";

    for(unsigned li = 0; li < n; ++li)
    {
        res += std::to_string(a[li]);
        if(li != n-1)
        res += " ";
    }
    res += "]";

    return res;
}