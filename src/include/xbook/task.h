#ifndef _XBOOK_TASK_H
#define _XBOOK_TASK_H

#include <arch/page.h>
#include <arch/cpu.h>
#include <arch/fpu.h>
#include <arch/task.h>
#include <arch/interrupt.h>
#include <sys/proc.h>
#include <sys/time.h>
#include <types.h>
#include "list.h"
#include "vmm.h"
#include "timer.h"
#include "spinlock.h"
#include "fsal.h"
#include "exception.h"
#include "alarm.h"
#include "portcomm.h"
#include "msgpool.h"
#include "signal.h"
#include "uid.h"
#ifdef CONFIG_PTHREAD
#include "pthread.h"
#endif

// #define TASK_TRAPFRAME_ON_KSTACK

typedef enum {
    TASK_READY = 0,         /* 进程处于就绪状态 */
    TASK_RUNNING,           /* 进程正在运行中 */
    TASK_BLOCKED,           /* 进程由于某种原因被阻塞 */
    TASK_WAITING,           /* 进程处于等待子进程状态 */
    TASK_STOPPED,           /* 进程处于停止运行状态 */
    TASK_HANGING,           /* 进程处于挂起，等待父进程来回收  */
    TASK_ZOMBIE,            /* 进程处于僵尸状态，父进程没有等待它 */
} task_state_t;

#define MAX_TASK_NAMELEN 32
#define TASK_STACK_MAGIC 0X19980325
#define MAX_TASK_STACK_ARG_NR 16
#define TASK_KERN_STACK_SIZE    (8192 * 3)

#define TASK_TIMESLICE_MIN  1
#define TASK_TIMESLICE_MAX  100
#define TASK_TIMESLICE_BASE  1

typedef void (*exit_hook_t)(void *);

enum task_flags {
    THREAD_FLAG_DETACH              = (1 << 0),     /* 线程分离标志，表示自己释放资源 */
    THREAD_FLAG_JOINED              = (1 << 1),     /* 线程被其它线程等待中 */
    THREAD_FLAG_JOINING             = (1 << 2),     /* 线程正在等待其它线程 */
    THREAD_FLAG_CANCEL_DISABLE      = (1 << 3),     /* 线程不能被取消 */
    THREAD_FLAG_CANCEL_ASYCHRONOUS  = (1 << 4),     /* 线程收到取消信号时立即退出 */
    THREAD_FLAG_CANCELED            = (1 << 5),     /* 线程已经标记上取消点 */
    THREAD_FLAG_WAITLIST            = (1 << 6),     /* 在等待链表中 */
    THREAD_FLAG_KERNEL              = (1 << 7),     /* 内核中的线程 */
    TASK_FLAG_NO_NEW_PRIVS          = (1 << 8),     /* 进程没有部分操作权限，为了安全考虑，例如调用execve操作 */
};

/* 本地通信端口表 */
#define LPC_PORT_NR 8
typedef struct {
    void *ports[LPC_PORT_NR];
} lpc_port_table_t;

#define LPC_PORT_BAD(port)  ((port) < 0 || (port) >= LPC_PORT_NR)

typedef struct {
    unsigned char *kstack;              /* kernel stack, must be first member */
    task_state_t state;
    spinlock_t lock;                    /* 操作task成员时需要进行上锁 */
    cpuid_t cpuid;
    pid_t pid;                          /* 进程自己的ID（process id） */
    pid_t parent_pid;                   /* 父进程的PID */
    pid_t tgid;                         /* 线程组ID：线程属于哪个进程，和pid一样，就说明是主线程，不然就是子线程（thread group id） */
    pid_t pgid;                         /* 进程组ID：一组进程拥有同样的进程组ID （process group id）*/
    uid_t uid;                          /* 用户ID：表明是在哪个用户上执行这个任务 （user id）*/
    gid_t gid;                          /* 用户组ID：表明用户属于哪个用户组 */
    uid_t euid;                         /* 有效的用户ID：和uid一样*/
    gid_t egid;                         /* 有效的用户组ID：和gid一样 */
    uid_t suid;                         /* 保存的用户ID：用户ID的备份 */
    gid_t sgid;                         /* 保存的用户组ID：用户组ID的备份 */
    gid_t groups[NGROUPS_MAX];          /* 附加用户组ID，也就是当前用户在哪些组里面 */
    pid_t sid;                          /* 会话ID，多个进程组组成一个会话。（SessionID） */
    unsigned long flags;                
    char priority;             /* 任务的动态优先级 */
    char static_priority;      /* 任务的静态优先级 */
    unsigned long ticks;                /* 运行的ticks，当前剩余的timeslice */
    unsigned long timeslice;            /* 时间片，可以动态调整 */
    unsigned long elapsed_ticks;        /* 任务执行总共占用的时间片数 */
    unsigned long syscall_ticks;        /* 执行系统调用总共占用的时间片数 */
    clock_t syscall_ticks_delta;  /* 执行单个系统调用占用的时间片数 */
    int exit_status;                    
    task_func_t *kthread_entry;
    void *kthread_arg;       // 线程执行时传入参数
    task_context_t context;             /* 任务的上下文 */
    trap_frame_t *trapframe;
    char name[MAX_TASK_NAMELEN];        
    struct vmm *vmm;                    
    list_t list;                        /* 处于所在队列的链表，就绪队列，阻塞队列等 */
    list_t global_list;                 /* 全局任务队列，用来查找所有存在的任务 */
    fpu_t fpu;
    long errcode;                       /* 错误码：用户多线程时用来标记每一个线程的错误码 */
    file_man_t *fileman;    
    exception_manager_t exception_manager;         
    timer_t sleep_timer;               
    alarm_t alarm;
    port_comm_t *port_comm;   
    lpc_port_table_t port_table;  
    #ifdef CONFIG_PTHREAD
    pthread_desc_t *pthread;            /* 用户线程管理，多个线程共同占有，只有一个主线程的时候为NULL */
    #endif
    exit_hook_t exit_hook;  /* 退出调用的钩子函数 */
    void *exit_hook_arg;
    struct tms times;

    /* 信号相关 */
    uint8_t signal_left;     /* 有信号未处理 */
    uint8_t signal_catched;     /* 有一个信号被捕捉，并处理了 */
    
    signal_t signals;        /* 进程对应的信号 */
    sigset_t signal_blocked;  /* 信号阻塞 */
    sigset_t signal_pending;     /* 信号未决 */
    spinlock_t signal_mask_lock;  /* 信号屏蔽锁 */

    mode_t umask;       /* umask for open/mkdir and etc. */
    int parent_death_signal;    /* 父进程死亡后，给子进程发送信号 */
    unsigned int stack_magic;
} task_t;

extern list_t task_global_list;
extern volatile int task_init_done;
extern char *kernel_stack_buttom;

#define TASK_GET_TRAP_FRAME(task) \
        ((trap_frame_t *) (((unsigned char *) (task) + \
        TASK_KERN_STACK_SIZE) - sizeof(trap_frame_t)))

#define TASK_IN_SAME_THREAD_GROUP(a, b) \
        ((a)->tgid == (b)->tgid)

#define TASK_IS_KERNEL_THREAD(task) \
        ((task)->flags & THREAD_FLAG_KERNEL)

#ifdef CONFIG_PTHREAD
/* 判断是用户态进程或者是用户态单线程 */
#define TASK_IS_SINGAL_THREAD(task) \
        ((((task)->pthread && \
        (atomic_get(&(task)->pthread->thread_count) <= 1))  || \
        (task)->pthread == NULL) && !TASK_IS_KERNEL_THREAD(task))

#define TASK_CHECK_THREAD_CANCELATION_POTINT(task) \
    do { \
        if (!((task)->flags & THREAD_FLAG_CANCEL_DISABLE) && \
            (task)->flags & THREAD_FLAG_CANCELED) { \
            pthread_exit((void *) THREAD_FLAG_CANCELED); \
        } \
    } while (0)

#else
#define TASK_IS_SINGAL_THREAD(task) \
        1

#define TASK_CHECK_THREAD_CANCELATION_POTINT(task) \
    do { \
        if (!((task)->flags & THREAD_FLAG_CANCEL_DISABLE) && \
            (task)->flags & THREAD_FLAG_CANCELED) { \
        } \
    } while (0)
#endif

#define TASK_WAS_STOPPED(task) ((task)->state == TASK_STOPPED)

#define TASK_NOT_READY(task) ((task)->state == TASK_BLOCKED || \
        (task)->state == TASK_WAITING || \
        (task)->state == TASK_STOPPED)

#define TASK_ENTER_WAITLIST(task) (task)->flags |= THREAD_FLAG_WAITLIST
#define TASK_LEAVE_WAITLIST(task) (task)->flags &= ~THREAD_FLAG_WAITLIST
#define TASK_IN_WAITLIST(task) ((task)->flags & THREAD_FLAG_WAITLIST)

#define TASK_NEED_STATE(__task, __state) while ((__task)->state != __state)

#define foreach_task(task) list_for_each_owner((task), &task_global_list, global_list)

void tasks_init();

void task_init(task_t *task, char *name, uint8_t prio_level);
void task_free(task_t *task);
void task_dump(task_t *task);

task_t *kern_thread_start(char *name, uint8_t prio_level, task_func_t *func, void *arg);
void kern_thread_exit(int status);

task_t *task_find_by_pid(pid_t pid);
void task_add_to_global_list(task_t *task);
void task_activate_when_sched(task_t *task);

void task_block(task_state_t state);
void task_unblock(task_t *task);
void task_yield();

void task_set_timeslice(task_t *task, uint32_t timeslice);

#define task_sleep() task_block(TASK_BLOCKED) 
static inline void task_wakeup(task_t *task)
{
    if (TASK_NOT_READY(task)) {
        /* NOTICE: if in a waitlist, must del it. */
        if (TASK_IN_WAITLIST(task)) {   
            list_del_init(&task->list);
        }
        task_unblock(task);
    }
}

static inline void task_detach(task_t *task)
{
    if (TASK_NOT_READY(task)) {
        /* NOTICE: if in a waitlist, must del it. */
        if (TASK_IN_WAITLIST(task)) {   
            list_del_init(&task->list);
        }
    }
}

pid_t task_take_pid();
void task_rollback_pid();
void tasks_print();
void task_start_user();
unsigned long task_sleep_by_ticks(clock_t ticks);
int task_count_children(task_t *parent);
int task_do_cancel(task_t *task);
pid_t task_get_pid(task_t *task);
int task_set_cwd(task_t *task, const char *path);
char *task_get_cwd();

int task_is_child(pid_t pid, pid_t child_pid);

#define sys_sched_yield     task_yield
pid_t sys_get_pid();
pid_t sys_get_ppid();
pid_t sys_get_tid();
pid_t sys_get_pgid(pid_t pid);
int sys_set_pgid(pid_t pid, pid_t pgid);
pid_t sys_set_tid_address(int *tidptr);
pid_t sys_getsid(pid_t pid);
pid_t sys_setsid();

int sys_getver(char *buf, int len);
int sys_tstate(tstate_t *ts, unsigned int *idx);
unsigned long sys_unid(int id);

static inline void task_exit_hook(task_t *task)
{
    if (task->exit_hook) {
        task->exit_hook(task->exit_hook_arg);
        task->exit_hook = NULL;
    }
}


void task_stack_build(task_t *task, task_func_t *function, void *arg);
int task_stack_build_when_forking(task_t *child);

unsigned long syscall_dispatch(trap_frame_t *frame);

#endif   /* _XBOOK_TASK_H */
