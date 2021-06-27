#include "test.h"

int test_uid(int argc, char *argv[])
{
    printf("%s: %d\n", $(getuid), getuid());
    printf("%s: %d\n", $(geteuid), geteuid());
    printf("%s: %d\n", $(getgid), getgid());
    printf("%s: %d\n", $(getegid), getegid());
    printf("%s: %d\n", $(setuid), setuid(getuid()));
    printf("%s: %d\n", $(seteuid), seteuid(geteuid()));
    printf("%s: %d\n", $(setgid), setgid(getgid()));
    printf("%s: %d\n", $(setegid), setegid(getegid()));
    
    printf("%s: %d\n", $(setuid), setuid(10));
    printf("%s: %d\n", $(seteuid), seteuid(10));
    printf("%s: %d\n", $(setgid), setgid(20));
    printf("%s: %d\n", $(setegid), setegid(20));

    printf("%s: %d\n", $(getuid), getuid());
    printf("%s: %d\n", $(geteuid), geteuid());
    printf("%s: %d\n", $(getgid), getgid());
    printf("%s: %d\n", $(getegid), getegid());

    printf("%s: %d\n", $(getgroups), getgroups(0, NULL));
    printf("%s: %d\n", $(setgroups), setgroups(0, NULL));
    printf("%s: %d\n", $(getgroups), getgroups(0, NULL));

    gid_t list[3] = {1, 2, 3};
    gid_t list2[3];
    int x;
    printf("%s: %d\n", $(setgroups), setgroups(3, list));
    printf("%s: %d\n", $(getgroups), x = getgroups(0, NULL));
    printf("%s: %d\n", $(getgroups), x = getgroups(x, list2));
    int i; 
    for (i = 0; i < x; i++)
        printf("%d=>%d\n", i, list2[i]);

    uid_t ruid, euid, suid;
    gid_t rgid, egid, sgid;
    printf("%s: %d\n", $(setresuid), setresuid(10,20,30));
    printf("%s: %d\n", $(getresuid), getresuid(&ruid, &euid, &suid));
    printf("%d, %d, %d\n", ruid, euid, suid);
    
    printf("%s: %d\n", $(setresgid), setresgid(10,20,30));
    printf("%s: %d\n", $(getresgid), getresgid(&rgid, &egid, &sgid));    
    printf("%d, %d, %d\n", rgid, egid, sgid);

    return 0;
}