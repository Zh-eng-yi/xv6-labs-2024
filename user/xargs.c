#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // Check if command is provided
    if (argc < 2) {
        printf("Usage: xargs <command> [initial-args...]\n");
        return 1;
    }
    // Cannot exceed MAXARG + 2 (1 for command, 1 for null terminator)
    if (argc > MAXARG + 2) {
        printf("Error: Too many arguments, maximum is %d.\n", MAXARG);
        return 1;
    }

    char *command = argv[1];
    char *args[MAXARG+2];

    // Copy initial arguments
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1];
    }
    args[argc - 1] = '\0';

    char buf[512];
    buf[511] = '\0';
    char *arg = buf;
    char *p = buf;
    int arg_count = argc - 2;
    int ws = 1;
    while (1) {
        // Read from stdin
        // If EOF, exec and exit
        int n = read(0, p, 1);
        if (n == 0) {
            if (p != buf) {
                *p = '\0';
                arg_count++;
                if (arg_count > MAXARG) {
                    printf("Error: Too many arguments, maximum is %d.\n", MAXARG);
                    return 1;
                }
                args[arg_count] = arg; // Store the start of the next argument
                if (fork() == 0) {
                    args[arg_count+1] = '\0';
                    exec(command, args);
                    printf("Error: exec failed for command %s\n", command);
                    exit(1);
                } else {
                    wait(0);
                }
            }
            break;
        }
        // If whitespace
        if (*p == ' ') {
            if (ws) {
                continue;
            }
            ws = 1;
            *p = '\0';
            p++;
            arg_count++;
            if (arg_count > MAXARG) {
                printf("Error: Too many arguments, maximum is %d.\n", MAXARG);
                return 1;
            }
            args[arg_count] = arg; // Store the start of the next argument
            arg = p; // Move to the next argument start
        }
        // If '\n', exec and continue
        else if (*p == '\n') {
            if (p == buf) {
                continue;
            }
            *p = '\0';
            arg_count++;
            if (arg_count > MAXARG) {
                printf("Error: Too many arguments, maximum is %d.\n", MAXARG);
                return 1;
            }
            args[arg_count] = arg; // Store the start of the next argument
            if (fork() == 0) {
                args[arg_count+1] = '\0';
                exec(command, args);
                printf("Error: exec failed for command %s\n", command);
                exit(1); 
            } else {
                wait(0);
                args[argc - 1] = '\0';
                arg_count = argc - 2;
                p = buf;
                arg = buf;
            }
        }
        // If any other character, continue reading
        else {
            ws = 0;
            p++;
        }
    }

}