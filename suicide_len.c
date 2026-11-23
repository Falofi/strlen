#include <stddef.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf g_buf;

static void segfault_handler(int sig)
{
    (void)sig;
    longjmp(g_buf, 1);
}

size_t suicide_len(const char *str)
{
    size_t i;
    struct sigaction sa;
    struct sigaction old_sa;

    sa.sa_handler = segfault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, &old_sa);
    i = 0;
    if (setjmp(g_buf) == 0)
    {
        while (str[i])
            i++;
    }
    sigaction(SIGSEGV, &old_sa, NULL);
    return (i);
}
