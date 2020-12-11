#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit lim;
    getrlimit(RLIMIT_STACK, &lim);
    long stack = lim.rlim_cur;
    getrlimit(RLIMIT_NPROC, &lim);
    long proc = lim.rlim_cur;
    getrlimit(RLIMIT_NOFILE, &lim);
    long fd = lim.rlim_cur;
    printf("stack size: %ld\n", stack);
    printf("process limit: %ld\n", proc);
    printf("max file descriptors: %ld\n", fd);
    return 0;
}
