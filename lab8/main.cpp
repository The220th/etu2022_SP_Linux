#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

#include <iostream>
#include <chrono>


#define DELAY_ENABLED 1
#define IN_FINE "./in.txt"
#define Q_MAIN_OWNER 0



typedef struct _msg_
{
    long whom; // msg_type
    int who;
    long long time_stamp;
    bool ra; //false-request, true-answer
} msg;

const unsigned MSG_LEN = sizeof(long) + sizeof(int) + sizeof(long long) + sizeof(bool);



long long get_cur_time();
void print_msg(const msg &msg_to_print);

int create_connect(unsigned my_num, int queue_key);
unsigned whom_send(unsigned my_num, unsigned next_num_1_or_2);
void send_requests_to_read(unsigned my_num, int queue_id, long long cur_time);
void read_file(int fd);
void send_allow_msg_from_current(msg &cur_msg, int queue_id);

/*
> ./main {prognum_num} {queue_key}
*/
int main(int argc, char* argv[])
{
    srandom(time(NULL));
    if(argc != 3)
    {
        std::cout << "Syntax error. Expected: \"> ./main {prognum_num} {queue_key}\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    const unsigned my_num = (unsigned)atoi(argv[1]);
    const int queue_key = atoi(argv[2]);
    if(!(my_num == 0 || my_num == 1 || my_num == 2))
    {
        std::cout << "{prognum_num} must be 0, 1 or 2. " << std::endl;
        exit(EXIT_FAILURE);
    }
    if(queue_key <= 0)
    {
        std::cout << "{queue_key} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }

    int fd = open(IN_FINE, O_RDONLY, S_IRUSR);
    if(fd < 0)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    const int msqid = create_connect(my_num, queue_key);

    /*
    Отправить запрос двум другим.
    Пока всем не ответил
        Ожидать сообщения:
            if(msg.ra == false)
            {
                Если файл прочитан, то:
                    отвечаем положительно, читать можно
                Если файл не прочитан, то:
                    Если программа послала запрос раньше нас, то ей даём разрешение
                    Если мы раньше послали, то откладываем запрос в долгий ящик (   msgsnd(queue_local, ...)   ). Ответим потом
            }
            if(msg.ra == true)
            {
                одна из програм разрешила читать, помечаем это.
                когда получены оба разрешения, то читать.
            }

    */
   long long send_req_time = get_cur_time();
   std::cout << "Current timestamp: " << send_req_time << std::endl;
   send_requests_to_read(my_num, msqid, send_req_time);
   std::cout << "Read request messages for the other two programs have been sent. " << std::endl;

    int queue_local = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    int serviced = 0;
    int recieved_local = 0;
    int recieved_all = 0;
    int allowed = 0;
    while(   !(serviced == 2 && recieved_all == 4)   )
    {
        std::cout << "Recieving msg..." << std::endl;
        msg rec_msg;
        unsigned buff = msgrcv(msqid, &rec_msg, MSG_LEN, my_num+1, 0);
        if(buff != MSG_LEN)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        ++recieved_all;
        std::cout << "Msg recieved: " << std::endl;
        print_msg(rec_msg);

        if(rec_msg.ra == true)
        {
            std::cout << "Read allowed from: " << (rec_msg.who-1) << std::endl;
            
            ++allowed;
            if(allowed == 2)
            {
                std::cout << "Time = " << get_cur_time() << ". Begin file reading: " << std::endl;
                sleep(3);
                std::cout << "\"\"\"" << std::endl;
                read_file(fd);
                std::cout << std::endl << "\"\"\"" << std::endl;
                std::cout << "Readed! " << std::endl;

                while(recieved_local > 0)
                {
                    msg l_msg;
                    msgrcv(queue_local, &l_msg, MSG_LEN, 0, 0);
                    --recieved_local;
                    send_allow_msg_from_current(l_msg, msqid);
                    ++serviced;
                }
                std::cout << "Allowed all another to read too. " << std::endl;
            }
        }
        else
        {
            std::cout << "Read request from: " << (rec_msg.who-1) << std::endl;
            if(allowed >= 2)
            {
                std::cout << "I am already read file, so I allow them read too. " << std::endl;

                send_allow_msg_from_current(rec_msg, msqid);
                ++serviced;
            }
            else
            {
                if(send_req_time < rec_msg.time_stamp)
                {
                    std::cout << "My timestamp lower, then him. So allow then read later. " << std::endl;

                    msgsnd(queue_local, &rec_msg, MSG_LEN, 0);
                    ++recieved_local;
                }
                else
                {
                    std::cout << "My timestamp larger, then him. So I allow them read too. " << std::endl;
                    send_allow_msg_from_current(rec_msg, msqid);
                    ++serviced;
                }
            }
        }
        #if DELAY_ENABLED == 1
        sleep(random() % 3);
        #endif // DELAY_ENABLED == 1
        std::cout << "Recieved: " << recieved_all << ", serviced: " << serviced << std::endl << std::endl << std::endl;
    }

    close(fd);

    if(my_num == Q_MAIN_OWNER)
    {
        std::cout << "Press enter to close msg queue with id = " << msqid << std::endl;
        getchar();

        msgctl(msqid, IPC_RMID, NULL);
    }

    std::cout << "============================== DONE! ==============================" << std::endl;
    return 0;
}


int create_connect(unsigned my_num, int queue_key)
{
    int buff;
    if(my_num == Q_MAIN_OWNER)
    {
        buff = msgget(queue_key, 0666 | IPC_CREAT | IPC_EXCL);
        if(buff == -1)
        {
            std::cout << "Cannot create msg queue. queue with key = " << queue_key << " already exist, choose another one" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "A queue with key =  " << queue_key << " has been created. Its id = " << buff << ". " << std::endl;
    }
    else
    {
        do
        {
            buff = msgget(queue_key, IPC_EXCL);
            if(buff == -1)
            {
                std::cout << "Queue with key = " << queue_key << " doesn't exist yet. Waiting..." << std::endl;
                #if DELAY_ENABLED == 1
                sleep(random() % 5);
                #endif // DELAY_ENABLED == 1
            }
        }while(buff == -1);
        buff = msgget(queue_key, IPC_CREAT);
        std::cout << "There was an attachment to the queue with key = " << queue_key << ". Its id = " << buff << ". " << std::endl;
    }
    return buff;
}

void send_requests_to_read(unsigned my_num, int queue_id, long long cur_time)
{
    msg _msg;
    _msg.whom = whom_send(my_num, 1)+1;
    _msg.who = my_num+1;
    _msg.time_stamp = cur_time;
    _msg.ra = false;

    std::cout << "Sending msg: " << std::endl;
    print_msg(_msg);
    msgsnd(queue_id, &_msg, MSG_LEN, 0);
    std::cout << std::endl;

    // https://linux.die.net/man/2/msgsnd
    // The msgsnd() system call appends a COPY of the message pointed to by 
    // &_msg to the message queue...
    _msg.whom = whom_send(my_num, 2)+1;
    std::cout << "Sending msg: " << std::endl;
    print_msg(_msg);
    msgsnd(queue_id, &_msg, MSG_LEN, 0);
    std::cout << std::endl;
}


void send_allow_msg_from_current(msg &cur_msg, int queue_id)
{
    unsigned buff;
    buff = cur_msg.who;
    cur_msg.who = cur_msg.whom;
    cur_msg.whom = buff;
    cur_msg.time_stamp = 0;
    cur_msg.ra = true;
    msgsnd(queue_id, &cur_msg, MSG_LEN, 0);
}

long long get_cur_time()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
}

void print_msg(const msg &msg_to_print)
{
    std::cout << "to: " << msg_to_print.whom-1 << std::endl;
    std::cout << "from: " << msg_to_print.who-1 << std::endl;
    std::cout << "time: " << msg_to_print.time_stamp << std::endl;
    std::cout << "ra: " << (msg_to_print.ra?"true":"false") << std::endl;
}

unsigned whom_send(unsigned my_num, unsigned next_num_1_or_2)
{
    unsigned ret_num = -1;
    if(my_num == 0)
    {
        if(next_num_1_or_2 == 1)
            ret_num = 1;
        else if (next_num_1_or_2 == 2)
            ret_num = 2;
        else
        {
            std::cout << "Failed successfully (whom_send). my_num = " << my_num << ", next_num_1_or_2 = " << next_num_1_or_2 << ". " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if(my_num == 1)
    {
        if(next_num_1_or_2 == 1)
            ret_num = 2;
        else if (next_num_1_or_2 == 2)
            ret_num = 0;
        else
        {
            std::cout << "Failed successfully (whom_send). my_num = " << my_num << ", next_num_1_or_2 = " << next_num_1_or_2 << ". " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if(my_num == 2)
    {
        if(next_num_1_or_2 == 1)
            ret_num = 0;
        else if (next_num_1_or_2 == 2)
            ret_num = 1;
        else
        {
            std::cout << "Failed successfully (whom_send). my_num = " << my_num << ", next_num_1_or_2 = " << next_num_1_or_2 << ". " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cout << "Failed successfully (whom_send). my_num = " << my_num << ", next_num_1_or_2 = " << next_num_1_or_2 << ". " << std::endl;
        exit(EXIT_FAILURE);
    }
    return ret_num;
}

void read_file(int fd)
{
    unsigned char c;
    while(read(fd, &c, 1) != 0)
    {
        write(1, &c, 1);
    }
}