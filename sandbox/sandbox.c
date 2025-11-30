/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpierant <mpierant@student.42roma.it>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 22:37:19 by mpierant          #+#    #+#             */
/*   Updated: 2025/11/30 22:50:26 by mpierant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

static volatile sig_atomic_t    g_timed_out = 0;    //Atomic for safe handler-program communication
static volatile pid_t   g_child_pid = -1;

static void handler(int sig)
{
    (void)sig;
    g_timed_out = 1;
    if (g_child_pid > 0)
        kill(g_child_pid, SIGKILL);
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    pid_t   pid;
    int status;
    struct sigaction    old_sa;
    struct sigaction    sa;
    int saved_errno;

    g_timed_out = 0;
    g_child_pid = -1;

    pid = fork();
    if (pid == -1)
        return (-1);
    if (pid == 0)
    {
        f();
        exit(0);
    }

    g_child_pid = pid;  //We access to it only before installing handler (no data race)

    sa.sa_handler = &handler;
    if (sigemptyset(&sa.sa_mask) == -1)
        return (-1);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, &old_sa) == -1)
        return (-1);

    alarm(timeout);

    while (1)
    {
        if (waitpid(pid, &status, 0) == -1)
        {
            if (errno == EINTR)
                continue;
            saved_errno = errno;
            alarm(0);
            sigaction(SIGALRM, &old_sa, NULL);
            errno = saved_errno;
            return (-1);
        }
        else
            break;
    }

    alarm(0);
    sigaction(SIGALRM, &old_sa, NULL);

    if (g_timed_out)
    {
        if (verbose)
            printf("Bad function: timed out after %d seconds\n", timeout);
        return (0);
    }
    else if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == 0)
        {
            if (verbose)
                printf("Nice function!\n");
            return (1);
        }
        else
        {
            if (verbose)
                printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
            return (0);
        }
    }
    else if (WIFSIGNALED(status))
    {
        if (verbose)
            printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
        return (0);
    }
    else
        return (-1);
}

/* ===================== */
/* VERY BAD FUNCTIONS    */
/* ===================== */

void nice_function(void)
{
    int i = 0;
    while (i < 100000000)
        i++;
}

void exit_zero(void)
{
    exit(0);
}

void exit_42(void)
{
    exit(42);
}

void segfault(void)
{
    int *p = NULL;
    *p = 10;
}

void do_abort(void)
{
    abort();
}

void div_zero(void)
{
    int x = 1;
    int y = 0;
    x = x / y;
    (void)x;
}

void infinite_loop(void)
{
    while (1)
        ;
}

void ignored_signal(void)
{
    signal(SIGTERM, SIG_IGN);
    while (1)
        ;
}

void same_time_access(void)
{
    sleep(1);
}

/* ===================== */
/* MAIN FOR TESTING      */
/* ===================== */

int main(void)
{
    printf("=== TEST sandbox() EDGE CASES ===\n\n");

    printf("Nice function:\n");
    sandbox(nice_function, 1, true);

    printf("\nExit(0):\n");
    sandbox(exit_zero, 1, true);

    printf("\nExit(42):\n");
    sandbox(exit_42, 1, true);

    printf("\nSegfault:\n");
    sandbox(segfault, 1, true);

    printf("\nabort():\n");
    sandbox(do_abort, 1, true);

    printf("\nDivision by zero:\n");
    sandbox(div_zero, 1, true);

    printf("\nIgnored signal:\n");
    sandbox(ignored_signal, 1, true);

    printf("\nInfinite loop (timeout):\n");
    sandbox(infinite_loop, 1, true);

    printf("\nSame time access to atomic g_timed_out:\n");
    sandbox(same_time_access, 1, true);

    printf("\n=== END OF TEST ===\n");

    return 0;
}