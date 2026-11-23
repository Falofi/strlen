#include <unistd.h>
#include <sys/wait.h>

int fork_len(const char *str)
{
    pid_t pid;
    int status;

    if (!*str)
        return (0);
    pid = fork();
    if (pid == 0)
        _exit(fork_len(str + 1));
    waitpid(pid, &status, 0);
    return (1 + WEXITSTATUS(status));
}
