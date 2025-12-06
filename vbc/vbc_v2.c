/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc_v2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpierant <mpierant@student.42roma.it>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 22:51:57 by mpierant          #+#    #+#             */
/*   Updated: 2025/12/06 18:17:18 by mpierant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void    error(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
    exit(1);
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
    if (*s)
        error(*s);
    printf("%d\n", res);
}

int expr()
{
    int val = term();
    while (*s == '+')
    {
        s++;
        val += term();
    }
    return (val);
}

int term()
{
    int val = factor();
    while (*s == '*')
    {
        s++;
        val *= factor();
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
        if (*s != ')')
            error(*s);
        s++;
        return (val);
    }
    error(*s);
    return (-1);
}