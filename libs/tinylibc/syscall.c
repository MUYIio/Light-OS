#include <stddef.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/reboot.h>
#include <sys/utsname.h>
#include <sys/syscall.h>

int open(const char *path, int flags)
{
    return syscall(SYS_openat, AT_FDCWD, path, flags, O_RDWR);
}

int openat(int dirfd,const char *path, int flags)
{
    return syscall(SYS_openat, dirfd, path, flags, 0600);
}

int close(int fd)
{
    return syscall(SYS_close, fd);
}

ssize_t read(int fd, void *buf, size_t len)
{
    return syscall(SYS_read, fd, buf, len);
}

ssize_t write(int fd, const void *buf, size_t len)
{
    return syscall(SYS_write, fd, buf, len);
}

pid_t getpid(void)
{
    return syscall(SYS_getpid);
}

pid_t getppid(void)
{
    return syscall(SYS_getppid);
}

int sched_yield(void)
{
    return syscall(SYS_sched_yield);
}

pid_t fork(void)
{
    return syscall(SYS_clone, SIGCHLD, 0);
}

pid_t clone(int (*fn)(void *arg), void *arg, void *stack, size_t stack_size, unsigned long flags)
{
    if (stack)
	stack += stack_size;

    return __clone(fn, stack, flags, NULL, NULL, NULL);
    //return syscall(SYS_clone, fn, stack, flags, NULL, NULL, NULL);
}
void exit(int code)
{
    syscall(SYS_exit, code);
}

int waitpid(int pid, int *code, int options)
{
    return syscall(SYS_wait4, pid, code, options, 0);
}

int exec(char *name)
{
    return syscall(SYS_execve, name);
}

int execve(const char *name, char *const argv[], char *const argp[])
{
    return syscall(SYS_execve, name, argv, argp);
}

clock_t times(struct tms *buf)
{
	return syscall(SYS_times, buf);
}

int64 get_time()
{
    TimeVal time;
    int err = sys_get_time(&time, 0);
    if (err == 0)
    {
        return ((time.sec & 0xffff) * 1000 + time.usec / 1000);
    }
    else
    {
        return -1;
    }
}

int sys_get_time(TimeVal *ts, int tz)
{
    return syscall(SYS_gettimeofday, ts, tz);
}

int time(unsigned long *tloc)
{
    return syscall(SYS_time, tloc);
}

int sleep(unsigned long long time)
{
    TimeVal tv = {.sec = time, .usec = 0};
    if (syscall(SYS_nanosleep, &tv, &tv)) return tv.sec;
    return 0;
}

int set_priority(int prio)
{
    return syscall(SYS_setpriority, prio);
}

void *mmap(void *start, size_t len, int prot, int flags, int fd, off_t off)
{
    return syscall(SYS_mmap, start, len, prot, flags, fd, off);
}

int munmap(void *start, size_t len)
{
    return syscall(SYS_munmap, start, len);
}

int wait(int *code)
{
    return waitpid((int)-1, code, 0);
}

int spawn(char *file)
{
    return syscall(SYS_spawn, file);
}

int mailread(void *buf, int len)
{
    return syscall(SYS_mailread, buf, len);
}

int mailwrite(int pid, void *buf, int len)
{
    return syscall(SYS_mailwrite, pid, buf, len);
}

int fstat(int fd, struct kstat *st)
{
    return syscall(SYS_fstat, fd, st);
}

int sys_linkat(int olddirfd, char *oldpath, int newdirfd, char *newpath, unsigned int flags)
{
    return syscall(SYS_linkat, olddirfd, oldpath, newdirfd, newpath, flags);
}

int sys_unlinkat(int dirfd, char *path, unsigned int flags)
{
    return syscall(SYS_unlinkat, dirfd, path, flags);
}

int link(char *old_path, char *new_path)
{
    return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int unlink(char *path)
{
    return sys_unlinkat(AT_FDCWD, path, 0);
}

int uname(struct utsname *buf)
{
    return syscall(SYS_uname, buf);
}

int brk(void *addr)
{
    return syscall(SYS_brk, addr);
}

char *getcwd(char *buf, size_t size){
    return syscall(SYS_getcwd, buf, size);
}

int chdir(const char *path){
    return syscall(SYS_chdir, path);
}

int mkdir(const char *path, mode_t mode){
    return syscall(SYS_mkdirat, AT_FDCWD, path, mode);
}

int getdents(int fd, struct linux_dirent64 *dirp64, unsigned long len){
    //return syscall(SYS_getdents64, fd, dirp64, len);
    return syscall(SYS_getdents64, fd, dirp64, len);
}

int pipe(int fd[2]){
    return syscall(SYS_pipe2, fd, 0);
}

int dup(int fd){
    return syscall(SYS_dup, fd);
}

int dup2(int old, int new){
    return syscall(SYS_dup3, old, new, 0);
}

int mount(const char *special, const char *dir, const char *fstype, unsigned long flags, const void *data)
{
    return syscall(SYS_mount, special, dir, fstype, flags, data);
}

int umount(const char *special)
{
    return syscall(SYS_umount2, special, 0);
}

unsigned int alarm(unsigned int seconds)
{
    return syscall(SYS_alarm, seconds);
}

int getitimer(int which, struct itimerval *curr_value)
{
    return syscall(SYS_getitimer, which, curr_value);
}

int setitimer(int which, const struct itimerval *restrict new_value,
    struct itimerval *restrict old_value)
{
    return syscall(SYS_setitimer, which, new_value, old_value);
}

int sched_setaffinity(pid_t tid, size_t size, const cpu_set_t *set)
{
    return syscall(SYS_sched_setaffinity, tid, size, set);
}

int sched_getaffinity(pid_t tid, size_t size, cpu_set_t *set)
{
    return syscall(SYS_sched_getaffinity, tid, size, set);
}

int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    return syscall(SYS_clock_gettime, clockid, tp);
}

int clock_settime(clockid_t clockid, const struct timespec *tp)
{
    return syscall(SYS_clock_settime, clockid, tp);
}

int sched_get_priority_max(int policy)
{
    return syscall(SYS_sched_get_priority_max, policy);
}

int sched_get_priority_min(int policy)
{
    return syscall(SYS_sched_get_priority_min, policy);
}

int removexattr(const char *path, const char *name)
{
	return syscall(SYS_removexattr, path, name);
}

int lremovexattr(const char *path, const char *name)
{
	return syscall(SYS_lremovexattr, path, name);
}

int fremovexattr(int fd, const char *name)
{
	return syscall(SYS_fremovexattr, fd, name);
}

int setxattr(const char *path, const char *name, const void *value, size_t size, int flags)
{
	return syscall(SYS_setxattr, path, name, value, size, flags);
}

int lsetxattr(const char *path, const char *name, const void *value, size_t size, int flags)
{
	return syscall(SYS_lsetxattr, path, name, value, size, flags);
}

int fsetxattr(int filedes, const char *name, const void *value, size_t size, int flags)
{
	return syscall(SYS_fsetxattr, filedes, name, value, size, flags);
}

ssize_t getxattr(const char *path, const char *name, void *value, size_t size)
{
	return syscall(SYS_getxattr, path, name, value, size);
}

ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size)
{
	return syscall(SYS_lgetxattr, path, name, value, size);
}

ssize_t fgetxattr(int filedes, const char *name, void *value, size_t size)
{
	return syscall(SYS_fgetxattr, filedes, name, value, size);
}

ssize_t listxattr(const char *path, char *list, size_t size)
{
	return syscall(SYS_listxattr, path, list, size);
}

ssize_t llistxattr(const char *path, char *list, size_t size)
{
	return syscall(SYS_llistxattr, path, list, size);
}

ssize_t flistxattr(int filedes, char *list, size_t size)
{
	return syscall(SYS_flistxattr, filedes, list, size);
}

int getpriority(int which, id_t who)
{
	return syscall(SYS_getpriority, which, who);
}

int setpriority(int which, id_t who, int prio)
{
	return syscall(SYS_setpriority, which, who, prio);
}

int reboot(int type)
{
	return syscall(SYS_reboot, 0xfee1dead, 672274793, type);
}

uid_t getuid(void)
{
	return syscall(SYS_getuid);
}

uid_t geteuid(void)
{
	return syscall(SYS_geteuid);
}

gid_t getgid(void)
{
	return syscall(SYS_getgid);
}

gid_t getegid(void)
{
	return syscall(SYS_getegid);
}

int setuid(uid_t uid)
{
	return syscall(SYS_setuid, uid);
}

int setgid(gid_t gid)
{
	return syscall(SYS_setgid, gid);
}

int seteuid(uid_t euid)
{
	return syscall(SYS_setresuid, -1, euid, -1);
}

int setegid(gid_t egid)
{
	return syscall(SYS_setresgid, -1, egid, -1);
}

int getgroups(int count, gid_t list[])
{
	return syscall(SYS_getgroups, count, list);
}

int setgroups(size_t count, const gid_t list[])
{
	return syscall(SYS_setgroups, count, list);
}

int setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	return syscall(SYS_setresuid, ruid, euid, suid);
}

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid)
{
	return syscall(SYS_getresuid, ruid, euid, suid);
}

int setresgid(uid_t rgid, uid_t egid, uid_t sgid)
{
	return syscall(SYS_setresgid, rgid, egid, sgid);
}

int getresgid(uid_t *rgid, uid_t *egid, uid_t *sgid)
{
	return syscall(SYS_getresgid, rgid, egid, sgid);
}

int setpgid(pid_t pid, pid_t pgid)
{
	return syscall(SYS_setpgid, pid, pgid);
}

pid_t getpgid(pid_t pid)
{
	return syscall(SYS_getpgid, pid);
}

pid_t getpgrp(void)
{
	return syscall(SYS_getpgid, 0);
}

pid_t gettid(void)
{
	return syscall(SYS_gettid);
}

pid_t setsid(void)
{
	return syscall(SYS_setsid);
}

pid_t getsid(pid_t pid)
{
	return syscall(SYS_getsid, pid);
}