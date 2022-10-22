#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

#include <iostream>


#define BUFF_LEN 256*1024
#define IN_FINE "./in.txt"
#define OUT_FILE1 "./out1.txt"
#define OUT_FILE2 "./out2.txt"


// ===== CHILD ====

int main_child(int fd_file_w, int fd_pipe_r, bool first_second_ch);

void process_handler(int sig);

int globalvar_parent_exit_flag;
bool globalvar_another_ch_ready;

// ===== CHILD ====

void q_handler(int sig);
/*
argv:
0 - programm name
1 - fd file out
2 - fd pipe in
*/
int main(int argc, char* argv[])
{
    int fd_file_w = atoi(argv[1]); 
    int fd_pipe_r = atoi(argv[2]);
    const bool first_second_ch = true;

    bool end_flag = 0;
    globalvar_parent_exit_flag = 0;
    __sighandler_t oldHandler;
    if(first_second_ch == false)
    {
        oldHandler = signal(SIGUSR2, process_handler);
    }
    else
    {
        oldHandler = signal(SIGUSR1, process_handler);
    }
    if(oldHandler == SIG_ERR)
    {
        perror("Signal SIGPIPE: ");
        _exit(EXIT_FAILURE);
    }
    oldHandler = signal(SIGQUIT, q_handler);
    if(oldHandler == SIG_ERR)
    {
        perror("Signal SIGQUIT: ");
        return -1;
    }

    globalvar_another_ch_ready = !first_second_ch;

    int read_ret;
    unsigned char c;

    //if(first_second_ch == true)
    //    kill(0, SIGUSR2);
    while(   !(globalvar_parent_exit_flag == 1 && end_flag == 1)   )
    {
        if(globalvar_another_ch_ready == false)
        {
            //pause();
            //sleep(1);
            //write(1, ("w2-" + std::to_string(globalvar_parent_exit_flag)).c_str(), 4); 
            continue;
        }
        else
        {
            do
            {
                read_ret = read(fd_pipe_r, &c, 1);
                if(read_ret <= 0)
                {
                    //sleep(1);
                    //std::cout << first_second_ch << " " << globalvar_parent_exit_flag << ": " << std::flush;
                    //perror("read pipe");
                }
                if((read_ret <= 0) && globalvar_parent_exit_flag == 1)
                {
                    end_flag = 1;
                    break;
                }
            }while(read_ret <= 0);

            if(read_ret > 0)
            {
                write(fd_file_w, &c, 1);
                write(1, &c, 1);
            }
            globalvar_another_ch_ready = false;
        }

        if(first_second_ch == false)
        {
            kill(0, SIGUSR1);
        }
        else
        {
            kill(0, SIGUSR2);
        }
    }

    return 0;
}

void q_handler(int sig)
{
    globalvar_parent_exit_flag = 1;
}

void process_handler(int sig)
{
    globalvar_another_ch_ready = true;
}