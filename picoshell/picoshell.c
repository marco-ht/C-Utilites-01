/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpierant <mpierant@student.42roma.it>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 22:37:16 by mpierant          #+#    #+#             */
/*   Updated: 2025/11/30 22:44:53 by mpierant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int    picoshell(char **cmds[])
{
    int i;
    int j;
    int n;
    int status;
    int flag;

    if (!cmds)
        return (1);
    n = 0;
    while (cmds[n])
        n++;
    if (n == 0)
        return (1);

    pid_t   pid[n];
    int fd[n - 1][2];

    //Creation of n - 1 pipes
    i = 0;
    while (i < n - 1)
    {
        if (pipe(fd[i]) == -1)
        {
            j = 0;
            while (j < i)
            {
                close(fd[j][0]);
                close(fd[j][1]);
                j++;
            }
            return (1);
        }
        i++;
    }

    //Fork (child process) for each of the n commands
    i = 0;
    while (i < n)
    {
        pid[i] = fork();
        if (pid[i] == -1)
        {
            j = 0;
            while (j < n - 1)
            {
                close(fd[j][0]);
                close(fd[j][1]);
                j++;
            }
            return (1);
        }
        if (pid[i] == 0)
        {
            if (i > 0)
            {
                if (dup2(fd[i - 1][0], STDIN_FILENO) == -1)
                {
                    j = 0;
                    while (j < n - 1)
                    {
                        close(fd[j][0]);
                        close(fd[j][1]);
                        j++;
                    }
                    exit(1);
                }
            }
            if (i < n - 1)
            {
                if (dup2(fd[i][1], STDOUT_FILENO) == -1)
                {
                    j = 0;
                    while (j < n - 1)
                    {
                        close(fd[j][0]);
                        close(fd[j][1]);
                        j++;
                    }
                    exit(1);
                }
            }

            j = 0;
            while (j < n - 1)
            {
                close(fd[j][0]);
                close(fd[j][1]);
                j++;
            }

            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        i++;
    }

    j = 0;
    while (j < n - 1)
    {
        close(fd[j][0]);
        close(fd[j][1]);
        j++;
    }

    flag = 0;
    while (wait(&status) != -1)
    {
        if (!WIFEXITED(status))
            flag = 1;
        if (WIFEXITED(status) && WEXITSTATUS(status))
            flag = 1;
    }
    return (flag);
}

#include <stdio.h>
int main(void)
{
    char *cmd1[] = {"cat", "picoshell.c", NULL};
    char *cmd2[] = {"grep", "include", NULL};
    char **cmds[] = {cmd1, cmd2, NULL};
    int res = picoshell(cmds);
    printf("res = %d\n", res);
}