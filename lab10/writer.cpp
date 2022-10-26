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
#include <time.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

// Readers nums in negative, writers num is not negarive.
#define CREATOR_PROG_NUM 0
#define FILE_NAME "out.txt"

void sleep_ms(unsigned ms);
std::string get_cur_time();
/*including min and including max*/
int rnd(int min, int max);

int create_shsem(int prog_num, int sem_key);

void write_to_file(int prog_num, const char* file_name);

/*
Всего планируется создать простых 4 семафора:
    (1) mutex для доступа к файлу на запись
    (2) кол-во желающих записать писателей
    (3) кол-во желающих прочитать читателей
    (4) кол-во всего работающих читателей и писателей вместе

Алгоритм:
    Для писателей: 
        1. Инкрементирует (4). Ждёт рандомное время. 
        2. Пока есть, что записать:
            3. Инкрементирует (2). 
            4. Ждёт, пока (3) упадёт до нуля. 
            5. Пытается сквозь мьютекс (1) записать в файл очередную строку. 
            6. Декрементирует (2). 
            7. Ждёт рандомное время. Вернуться в пункт 2. 
        8. Декрементирует (4).
        // 9. Если писатель последний из всех читателей и писателей, то уничтожить все семафоры. 
        9. Если писатель создавал семафоры, то он должен дождаться пока все закончат и уничтожить все созданные семафоры. 
        10. Конец
    
    Для читателей: 
        1. Инкрементирует (4). Ждёт рандомное время. 
        2. Ждёт, пока (2) упадёт до нуля. Как только (2) упало до нуля, то сразу инкрементировать (3). 
                То есть получается 2 операции за раз одновременно: ждать (2) упадёт до нуля и инкрементировать (3). 
        3. Читает. 
        4. Декрементирует (3). 
        // 5. Если читатель последний из всех читателей и писателей, то уничтожить все семафоры. 
        5. Если читатель создавал семафоры, то он должен дождаться пока все закончат и уничтожить все созданные семафоры. 
        6. Конец. 

*/

/*
./writer {prog_num} {sem_key}
*/
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << "Syntax error. Expected: \"> ./writer {prognum_num} {sem_key}\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int prog_num = atoi(argv[1]);
    const int sem_key = atoi(argv[2]);
    if(sem_key <= 0)
    {
        std::cout << "{sem_key} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }

    srandom(time(NULL) + 13*prog_num);

    const int sem_id = create_shsem(prog_num, sem_key);
    struct sembuf sembuff_buff;

    sembuff_buff = {4, 1, 0};
    semop(sem_id, &sembuff_buff, 1);

    sleep_ms(rnd(1150, 1300));
    

    
        const unsigned times_count = rnd(5, 15);
        std::cout << "\nwriter " << prog_num << " want to write " << times_count << " lines. " << std::endl; 
        for(unsigned gi = 0; gi < times_count; ++gi)
        {
            sembuff_buff = {2, 1, 0};
            semop(sem_id, &sembuff_buff, 1);

            sembuff_buff = {3, 0, 0};
            semop(sem_id, &sembuff_buff, 1);

            sembuff_buff = {1, -1, 0};
            semop(sem_id, &sembuff_buff, 1);

            write_to_file(prog_num, FILE_NAME);

            sembuff_buff = {1, 1, 0};
            semop(sem_id, &sembuff_buff, 1);

            sembuff_buff = {2, -1, 0};
            semop(sem_id, &sembuff_buff, 1);

            sleep_ms(rnd(50, 300));
        }



    sembuff_buff = {4, -1, 0};
    semop(sem_id, &sembuff_buff, 1);

    if(prog_num == CREATOR_PROG_NUM)
    {
        sembuff_buff = {4, 0, 0};
        semop(sem_id, &sembuff_buff, 1);
        std::cout << "Destroying semaphores with key = " << sem_key << " and id = " << sem_id << "... " << std::flush; 
        if(shmctl(sem_id, IPC_RMID, 0) < 0)
        {
            perror("Destroy semaphores in shmctl failed");
        }
        std::cout << "OK! " << std::endl; 
    }
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



int create_shsem(int prog_num, int sem_key)
{
    int buff;
    int shsem_id;

    if(prog_num == CREATOR_PROG_NUM)
    {
        buff = semget(sem_key, 4, 0666 | IPC_CREAT | IPC_EXCL);
        if(buff == -1)
        {
            std::cout << "Cannot create semaphores with key = " << sem_key << ", because it is already exist, choose another one. " << std::endl;
            perror("semget failed");
            exit(EXIT_FAILURE);
        }
        shsem_id = buff;
        std::cout << get_cur_time() << "Semaphores with key = " << sem_key << " has been created. Its id = " << shsem_id << ". " << std::endl;
    }
    else
    {
        do
        {
            buff = semget(sem_key, 4, IPC_EXCL);
            if(buff == -1)
            {
                std::cout << get_cur_time() << "Semaphores with key = " << sem_key << " doesn't exist yet. Waiting..." << std::endl;
                sleep(1);
            }
        }while(buff == -1);
        shsem_id = buff;
        std::cout << get_cur_time() << "There was an attachment to the semaphores with key = " << sem_key << ". Its id = " << shsem_id << ". " << std::endl;
    }

    if(prog_num == CREATOR_PROG_NUM)
    {
        struct sembuf sembuff_buff = {1, 1, 0};
        semop(shsem_id, &sembuff_buff, 1);
    }

    return shsem_id;
}

void write_to_file(int prog_num, const char* file_name)
{
    std::cout << "\nwriter " << prog_num << " writting in file... " << std::flush;

    unsigned long write_delay_after = rnd(1, 100);
    int fd = open(file_name, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);

    std::string buffS;
    buffS = get_cur_time() + "writer " + std::to_string(prog_num) + " write: ";
    write(fd, buffS.c_str(), buffS.length());

    unsigned N = rnd(1, 15);
    for(unsigned i = 0; i < N; ++i)
    {
        buffS = std::to_string(rnd(0, 1000)) + " ";
        write(fd, buffS.c_str(), buffS.length());
        if(rnd(0, 1) == 0)
            sleep_ms(random() % 50);
    }

    write(fd, "\n", 1);
    
    sleep_ms(write_delay_after);

    if(close(fd) < 0)
        perror("Cannot close file");

    std::cout << "OK! " << std::endl;
}