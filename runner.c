#include "runner.h"
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int parse_pipeline(int argc, char **argv, struct pipeline *p)
{
    if (argv == NULL || p == NULL)
        return -1;

    int sep = -1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0) {
            sep = i;
            break;
        }
    }
    if (sep <= 1 || sep >= argc - 1)
        return -1;

    argv[sep] = NULL;
    p->left.argv = &argv[1];
    p->right.argv = &argv[sep + 1];
    return 0;
}

static pid_t spawn(char **argv, int in_fd, int out_fd, int pipe_r, int pipe_w)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        if (in_fd != STDIN_FILENO && dup2(in_fd, STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(126);
        }
        if (out_fd != STDOUT_FILENO && dup2(out_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(126);
        }
        close(pipe_r);
        close(pipe_w);
        execvp(argv[0], argv);
        perror(argv[0]);
        _exit(127);
    }
    return pid;
}

int run_pipeline(const struct pipeline *p)
{
    if (p == NULL)
        return -1;

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t left = spawn(p->left.argv, STDIN_FILENO, fd[1], fd[0], fd[1]);
    if (left == -1) {
        close(fd[0]);
        close(fd[1]);
        return -1;
    }
    pid_t right = spawn(p->right.argv, fd[0], STDOUT_FILENO, fd[0], fd[1]);

    close(fd[0]);
    close(fd[1]);

    if (right == -1) {
        waitpid(left, NULL, 0);
        return -1;
    }

    int status = 0;
    int failed = 0;
    if (waitpid(left, NULL, 0) == -1) {
        perror("waitpid");
        failed = 1;
    }
    if (waitpid(right, &status, 0) == -1) {
        perror("waitpid");
        failed = 1;
    }
    if (failed)
        return -1;

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return 1;
}
