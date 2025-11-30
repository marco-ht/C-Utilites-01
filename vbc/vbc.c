/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpierant <mpierant@student.42roma.it>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 22:51:57 by mpierant          #+#    #+#             */
/*   Updated: 2025/11/30 22:51:58 by mpierant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <ctype.h>

void    error(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

char    *s;

int expr();
int term();
int factor();

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    s = argv[1];
    int res = expr();
    if (res == -1)
        return (1);
    if (*s)
        return (error(*s), 1);
    printf("%d\n", res);
}

int expr()
{
    int val = term();
    if (val == -1)
        return (-1);
    while (*s == '+')
    {
        s++;
        int tmp = term();
        if (tmp == -1)
            return (-1);
        val += tmp;
    }
    return (val);
}

int term()
{
    int val = factor();
    if (val == -1)
        return (-1);
    while (*s == '*')
    {
        s++;
        int tmp = factor();
        if (tmp == -1)
            return (-1);
        val *= tmp;
    }
    return (val);
}

int factor()
{
    if (isdigit(*s))
        return (*s++ - '0');
    if (*s == '(')
    {
        s++;
        int val = expr();
        if (val == -1)
            return (-1);
        if (*s != ')')
            return (error(*s), -1);
        s++;
        return (val);
    }
    error(*s);
    return (-1);
}