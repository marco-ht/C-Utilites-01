/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpierant <mpierant@student.42roma.it>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 22:37:04 by mpierant          #+#    #+#             */
/*   Updated: 2025/11/30 22:37:05 by mpierant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>

int    ft_popen(const char *file, char *const argv[], char type)
{
    pid_t   pid;
    int fd[2];

    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);
    if (pipe(fd) == -1)
        return (-1);
    pid = fork();
    if (pid == -1)
    {
        close(fd[0]);
        close(fd[1]);
        return (-1);
    }
    if (pid == 0)
    {
        if (type == 'r')
        {
            close(fd[0]);
            if (dup2(fd[1], STDOUT_FILENO) == -1)
            {
                close(fd[1]);
                exit(1);
            }
            close(fd[1]);
        }
        else
        {
            close(fd[1]);
            if (dup2(fd[0], STDIN_FILENO) == -1)
            {
                close(fd[0]);
                exit(1);
            }
            close(fd[0]);
        }
        execvp(file, argv);
        exit(1);
    }
    if (type == 'r')
    {
        close(fd[1]);
        return (fd[0]);
    }
    else
    {
        close(fd[0]);
        return (fd[1]);
    }
}

#include <sys/wait.h>
int main(void)
{
    int fd;
    char    c;

    fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
    if (fd == -1)
        return (1);
    while (read(fd, &c, 1))
        write(1, &c, 1);
    close(fd);
    fd =ft_popen("grep", (char *const[]){"grep", "ciao", NULL}, 'w');
    if (fd == -1)
        return (1);
    write(fd, "ciao ddd\nmiao\n", 14);
    close(fd);
    while (wait(NULL) != -1);
}