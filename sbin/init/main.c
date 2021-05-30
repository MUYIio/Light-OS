#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #define _HAS_LOGIN
// #define _HAS_NETSERV
#define _HAS_CONSOLE
//#define CONSOLE_DEV "/dev/tty0"
#define CONSOLE_DEV "/dev/con0"

int main(int argc, char *argv[])
{
    /* 打开tty，用来进行基础地输入输出 */
    int tty0 = open(CONSOLE_DEV, O_RDONLY);
    if (tty0 < 0) {
        return -1;
    }
    int tty1 = open(CONSOLE_DEV, O_WRONLY);
    if (tty1 < 0) {
        close(tty0);
        return -1;
    }
    int tty2 = dup(tty1);
    if (tty2 != 2) {
        close(tty0);
        close(tty1);
        return -1;
    }
    int i;
    for (i = 0; i < argc; i++) {
        if (argv[i]) {
            printf("initd: argv %d=%s\n", i, argv[i]);
        }
    }
    
    /* 创建一个子进程 */
    int pid = fork();
    if (pid < 0) {
        printf("initd: fork process error! stop service.\n");
        return -1;
    }
    if (pid > 0) {
        /* 父进程就相当于init进程，功能很简单，就只等待子进程退出 */
        while (1) {
            int status = 0;
            int _pid;
            _pid = waitpid(-1, &status, 0);    /* wait any child exit */
            if (_pid > 1) {
                printf("initd: process %d exit with status %d.\n", _pid, status);
            }
        }
    }
    #ifdef _HAS_NETSERV
    pid = fork();
    if (pid < 0) {
        printf("initd: fork process error! stop service.\n");
        return -1;
    } else if (pid == 0) { /* 子进程执行服务 */
        exit(execve("/sbin/netserv", NULL, NULL));
    }
    #endif
    
    #ifdef __XLIBC__
    environ = NULL;
    setpgrp();
    tcsetpgrp(STDIN_FILENO, getpgrp());
    #endif
    
    #ifdef _HAS_LOGIN
    char *_argv[3] = {"-s", "/bin/sh", NULL};
    exit(execv("/sbin/login", _argv));
    #else

    exit(execve("/bin/sh", NULL, NULL));
    #endif
    return 0;
}

