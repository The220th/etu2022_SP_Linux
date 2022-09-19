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
    if(argc != 3)
    {
        std::cout << "Syntax error. Expected: \"> programm {delay} {filename}\"" << std::endl;
        return EXIT_FAILURE;
    }
    unsigned vfork_delay = (unsigned)atoi(argv[1]);
    const char* fileName = argv[2];

    int fd = open(fileName, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0)
    {
        perror("Cannot open file");
        return errno;
    }

    sleep(vfork_delay);

    print_to_file("vfork child", fd);

    close(fd);

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