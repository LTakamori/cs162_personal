#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

char *env_path[] = {"/usr/bin/", "/home/xabi/.local/bin/", "/usr/local/sbin/", "/usr/local/bin/", "/usr/sbin/", "/usr/bin:/sbin/",
                    "/bin:/usr/games/", "/usr/local/games/", "/snap/bin/"};

int cmd_exit(struct tokens *tokens);

int cmd_help(struct tokens *tokens);

int cmd_cd(struct tokens *tokens);

int cmd_pwd(struct tokens *tokens);

void exe_program(struct tokens *tokens);

void path_resol(char *path);

bool file_exist(char* file_path);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
    cmd_fun_t *fun;
    char *cmd;
    char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
        {cmd_help, "?",    "show this help menu"},
        {cmd_exit, "exit", "exit the command shell"},
        {cmd_cd,   "cd",   "changes the current working directory to that directory."},
        {cmd_pwd,  "pwd",  "prints the current working directory to standard output"},
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
    for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
        printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
    return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
    exit(0);
}

int cmd_cd(unused struct tokens *tokens) {
//    printf("%zu\n", tokens_get_length(tokens));
    if (tokens_get_length(tokens) != 2) {
        printf("bash: cd: too many/less arguments\n");
        return 1;
    }
    if (chdir(tokens_get_token(tokens, 1)) == 0) {
//        printf("bash: cd: too many/less arguments\n");
        return 0;
    } else {
        printf("bash: cd: %s: No such file or directory\n", tokens_get_token(tokens, 0));
        return 1;
    }
}

int cmd_pwd(struct tokens *tokens) {
    char buffer[4096];
    if (getcwd(buffer, 4096)) {
        printf("%s\n", buffer);
        return 0;
    } else {
        return 1;
    }
}

bool file_exist(char* file_path) {
    FILE *file;
    file = fopen(file_path, "r");
    if (file != NULL){
        fclose(file);
        return 1;
    }
    return 0;
}


void path_resol(char *path) {
    char temp[4096];
    if (path[0] == '.' || path[0] == '/') {
        return;
    } else {
        for (int i = 0; i < (sizeof(env_path) / sizeof(env_path[0])); i++) {
            strcpy(temp, env_path[i]);
            strcpy(temp + strlen(env_path[i]), path);
            if (file_exist(temp)) {
                strcpy(path, temp);
                return;
            }
            temp[0] = '\0';
        }
    }
}

void exe_program(struct tokens *tokens) {
    int argc = tokens_get_length(tokens);
    if (argc == 0) return;
    char *argv[argc + 1];
    for (int i = 0; i < argc; i++) {
        argv[i] = tokens_get_token(tokens, i);
    }
    path_resol(argv[0]);
    argv[argc] = NULL;
    int pid = fork();
    if (pid == 0) {
        execv(argv[0], argv);
        exit(0);
    } else if (pid > 0) {
        int child_stat;
        wait(&child_stat);
//        printf("Child status %d\n", child_stat);
        if (child_stat != 0) {
            printf("Some error happened when running the program %s\n", tokens_get_token(tokens, 0));
        }
    } else {
        printf("Failed to fork a child\n");
    }
}


/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
    for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
            return i;
    return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
    /* Our shell is connected to standard input. */
    shell_terminal = STDIN_FILENO;

    /* Check if we are running interactively */
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
         * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
         * foreground, we'll receive a SIGCONT. */
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);

        /* Saves the shell's process id */
        shell_pgid = getpid();

        /* Take control of the terminal */
        tcsetpgrp(shell_terminal, shell_pgid);

        /* Save the current termios to a variable, so it can be restored later. */
        tcgetattr(shell_terminal, &shell_tmodes);
    }
}

int main(unused int argc, unused char *argv[]) {
    init_shell();

    static char line[4096];
    int line_num = 0;

    /* Please only print shell prompts when standard input is not a tty */
    if (shell_is_interactive)
        fprintf(stdout, "%d: ", line_num);

    while (fgets(line, 4096, stdin)) {
        /* Split our line into words. */
        struct tokens *tokens = tokenize(line);

        /* Find which built-in function to run. */
        int fundex = lookup(tokens_get_token(tokens, 0));

        if (fundex >= 0) {
            cmd_table[fundex].fun(tokens);
        } else {
            /* TODO: REPLACE this to run commands as programs. */
            // fprintf(stdout, "This shell doesn't know how to run programs.\n");
            exe_program(tokens);
        }

        if (shell_is_interactive)
            /* Please only print shell prompts when standard input is not a tty */
            fprintf(stdout, "%d: ", ++line_num);

        /* Clean up memory */
        tokens_destroy(tokens);
    }

    return 0;
}
