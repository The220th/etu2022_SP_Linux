#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sstream>

void print_to_file(const char* who, int fd);


int main()
{
    std::string fileName;
    int fd;

    std::cout << "Enter file name: " << std::flush;
    getline(std::cin, fileName);

    //fd = open(fileName.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    fd = 1;
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
        for(int i = 0; i < 10; ++i)
            print_to_file("fork", fd);
        return 0;
    }
    else
    {
        sleep(2);
        int child_pid = clone_dif;
        int child_exit_status = 0;
        for(int i = 0; i < 10; ++i)
            print_to_file("original", fd);
        int wait_res = waitpid(child_pid, &child_exit_status, 0);
        if(wait_res < 0)
        {
            perror("Cannot wait fork");
            return errno;
        }
        if(WIFEXITED(child_exit_status))
        {
            std::cout << "Child proccess " << child_pid << " ends successfully. " << std::endl;
        }
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

    out << who << "says: ";
    out << "pid = " << pid << ", ";
    out << "ppid = " << ppid << ", ";
    out << "sid = " << sid << ", ";
    out << "pgid = " << pgid << ", ";
    out << "uid = " << uid << ", euid = " << euid << ", ";
    out << "gid = " << gid << ", egid = " << gid << ". " << std::endl; 

    std::string out_str = out.str();
    write(fd, out_str.c_str(), out_str.length());
}