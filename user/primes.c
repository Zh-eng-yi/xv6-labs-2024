#include "kernel/types.h"
#include "user/user.h"

// void primes(int p) __attribute__((noreturn));
void primes(int p);

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(p[1]);
        primes(p[0]);
        exit(0);
    } else {
        close(p[0]);
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
}

void primes(int p) {
    close(0);
    dup(p);
    close(p);
    int n1;
    if (read(0, &n1, sizeof(n1)) <= 0) {
        return;
    }
    printf("prime %d\n", n1);
    int p2[2];
    pipe(p2);
    if (fork() == 0) {
        close(p2[1]);
        primes(p2[0]);
        exit(0);
    } else {
        close(p2[0]);
        int n2;
        while (read(0, &n2, sizeof(n2)) > 0) {
            if (n2 % n1 != 0) {
                write(p2[1], &n2, sizeof(n2));
            }
        }
        close(p2[1]);
        wait(0);
        exit(0);
    }
}
