#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

void t()
{
    char b;
    int c[30];
    while (1) {
        
    }
}

/* 栈溢出测试 */
int fib(int n)
{
    char buf[4096] = {0};
    if (n < 2)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

file_test()
{
    



    while (1);
}


int main()
{
    #if 0
    int a = 0;
    char buf[32] = {0};
    int c = a + 10;
    t();
    while (1) {
        
    }
    #endif
    #if 1
    /* 打开tty，用来进行基础地输入输出 */
    int tty0 = open("/dev/con0", O_RDONLY);
    if (tty0 < 0) {
        while (1)
        {
            /* code */
        }
        
        return -1;
    }
    
    int tty1 = open("/dev/con0", O_WRONLY);
    if (tty1 < 0) {
        while (1)
        {
            /* code */
        }
        close(tty0);
        return -1;
    }
    int tty2 = dup(tty1);
    
    //printf("[INIT]: start.\n");
    char *str = "[INIT]: start.\n";
    write(tty1, str, strlen(str));
    

    file_test();





    /* exec测试 */
    char *argv[3] = {"/sbin/test1","arg1", 0};
    char *env[3] = {"env0", "env1",0};
    // execve("/sbin/init", argv, env);    
    #if 0
    pid_t child = create_process(argv, env, 0);
    printf("create %d\n", child);
    waitpid(child, NULL, 0);
    printf("Wait done\n");
        
    while (1)
    {
        /* code */
    }
    
    #endif
    #if 0
    pid_t pid1 = fork();
    if (pid1 > 0) {
        write(tty1, "I am parent\n", 12);
        int state;
        waitpid(pid1, &state, 0);
        write(tty1, "Wait done\n", 9);
        while (1)
        {
            /* code */
        }
    } else {
        write(tty1, "I am child\n", 11);
        execve("/sbin/test1", argv, env);    
        _exit(0);
    }
    
    while (1)
    {
        /* code */
    }
    #endif
    /*====内存测试===*/
    brk(NULL);
    char *mbuf = sbrk(1024);
    memset(mbuf, 0, 1024);

    char *v = (char *) 0x00001000;
    *v = 0x10;

    printf("hello, world!\n");
    int n = fib(10);
    printf("fib:%d\n", n);
    
    char *mbuf0 = malloc(32);
    char *mbuf1 = malloc(512);
    char *mbuf2 = malloc(4096*2);
    printf("malloc:%p %p %p\n", mbuf0, mbuf1, mbuf2);
    free(mbuf0);
    free(mbuf1);
    free(mbuf2);
    printf("test done!\n");

    // printf("hello, world!\n");
    pid_t pid = fork();
    if (pid > 0) {
        printf("I am parent, pid=%d child=%d\n", getpid(), pid);
        int state;
        waitpid(pid, &state, 0);
        write(tty1, "Wait done\n", 9);
    } else {
        printf("I am child, pid=%d parent=%d\n", getpid(), getppid());
        sleep(3);
        printf("sleep done\n");
    }
    pid = fork();
    if (pid > 0) {
        printf("I am parent, pid=%d child=%d\n", getpid(), pid);
        int state;
        waitpid(pid, &state, 0);
    } else {
        printf("I am child, pid=%d parent=%d\n", getpid(), getppid());
    }
    #endif
    _exit(0);
    while (1)
    {
        /* code */
    }
    
    return 0;
}