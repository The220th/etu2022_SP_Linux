#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sstream>
#include <string>

void print_to_file(const char* who, int fd);


int main(int argc, char* argv[])
{
    int buff;
    if(argc != 4)
    {
        std::cout << "Syntax error. Expected: \"> ./main {parent_delay} {fork_delay} {vfork_delay}\"" << std::endl;
        return EXIT_FAILURE;
    }

    unsigned parent_delay = (unsigned)atoi(argv[1]);
    unsigned fork_delay = (unsigned)atoi(argv[2]);
    unsigned vfork_delay = (unsigned)atoi(argv[3]);

    std::string fileName;
    int fd;

    std::cout << "Enter file name: " << std::flush;
    getline(std::cin, fileName);

    fd = open(fileName.c_str(), O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    //fd = 1 is stdout, fd = 0 is stdin, fd = 2 is stderr
    if(fd < 0)
    {
        perror("Cannot open file");
        return errno;
    }

    int clone_dif = fork();
    if(clone_dif < 0)
    {
        perror("Cannot fork");
        return errno;
    }
    else if(clone_dif == 0)
    {
        sleep(fork_delay);
        print_to_file("fork child", fd);
        return 0;
    }

    int child_pid_fork = clone_dif;

    clone_dif = vfork();
    if(clone_dif < 0)
    {
        perror("Cannot vfork");
        return errno;
    }
    else if(clone_dif == 0)
    {
        // Ничего не трогаем в vfork, кроме exec* и _exit, иначе поведение непредсказуемо
        buff = execl("vfork_exec", "vfork_exec", argv[3], fileName.c_str(), NULL);
        if(buff < 0) // Если вернулись из execl, то уже очень плохо
        {
            perror("Cannot execl");
            _exit(errno);
        }
    }

    sleep(parent_delay);
    int child_exit_status = 0;
    print_to_file("Parent", fd);
    int wait_res = waitpid(child_pid_fork, &child_exit_status, 0);
    if(wait_res < 0)
    {
        perror("Cannot wait fork");
        return errno;
    }
    if(WIFEXITED(child_exit_status))
    {
        std::cout << "Child proccess " << child_pid_fork << " ends successfully. " << std::endl;
    }

    if(close(fd) < 0)
    {
        perror("Cannot close file");
        return errno;
    }
    return 0;
}

void print_to_file(const char* who, int fd)
{
    int pid = getpid();
    int ppid = getppid();
    int sid = getsid(pid);
    int pgid = getpgid(pid);
    unsigned uid = getuid();
    unsigned euid = geteuid();
    unsigned gid = getgid();
    unsigned egid = getegid();

    std::stringstream out;

    out << who << " says: ";
    out << "pid = " << pid << ", ";
    out << "ppid = " << ppid << ", ";
    out << "sid = " << sid << ", ";
    out << "pgid = " << pgid << ", ";
    out << "uid = " << uid << ", euid = " << euid << ", ";
    out << "gid = " << gid << ", egid = " << gid << ". " << std::endl; 

    std::string out_str = out.str();
    write(fd, out_str.c_str(), out_str.length());
}