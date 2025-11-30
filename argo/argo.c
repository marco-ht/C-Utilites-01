/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpierant <mpierant@student.42roma.it>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 22:30:01 by mpierant          #+#    #+#             */
/*   Updated: 2025/11/30 22:30:03 by mpierant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "argo.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int argo(json *dst, FILE *stream);
static int parse_value(json *dst, FILE *stream);
static int parse_int(json *dst, FILE *stream);
static int parse_str(json *dst, FILE *stream);
static int parse_map(json *dst, FILE *stream);

static void free_items(pair *items, size_t size);

int argo(json *dst, FILE *stream)
{
    if (parse_value(dst, stream) == -1)
        return (-1);
    if (peek(stream) != EOF)
        return (unexpected(stream), free_json(*dst), -1);
    return (1);
}

static int  parse_value(json *dst, FILE *stream)
{
    int c = peek(stream);

    if (c == '"')
        return (parse_str(dst, stream));
    else if (c == '{')
        return (parse_map(dst, stream));
    else if (isdigit(c) || c == '-')
        return (parse_int(dst, stream));
    else
        return (unexpected(stream), -1);
}

static int  parse_int(json *dst, FILE *stream)
{
    int num;

    if (fscanf(stream, "%d", &num) == -1)
        return (unexpected(stream), -1);
    dst->type = INTEGER;
    dst->integer = num;
    return (1);
}

static int  parse_str(json *dst, FILE *stream)
{
    if (!expect(stream, '"'))
        return (-1);

    size_t  capacity = 32;
    size_t  len = 0;
    char    *buffer = malloc(capacity);
    if (!buffer)
        return (-1);
    while (1)
    {
        int c = getc(stream);
        if (c == EOF)
            return (unexpected(stream), free(buffer), -1);
        if (c == '"')
        {
            buffer[len] = '\0';
            dst->type = STRING;
            dst->string = buffer;
            return (1);
        }
        if (c == '\\')
        {
            int escaped = getc(stream);
            if (escaped == '"' || escaped == '\\')
                c = escaped;
            else
            {
                ungetc(escaped, stream);
                return (unexpected(stream), free(buffer), -1);
            }
        }
        if (len + 1 >= capacity)
        {
            capacity *= 2;
            char    *tmp = realloc(buffer, capacity);
            if (!tmp)
                return (free(buffer), -1);
            buffer = tmp;
        }
        buffer[len++] = c;
    }
}

static void free_items(pair *items, size_t size)
{
    size_t  i;

    i = 0;
    while (i < size)
    {
        free(items[i].key);
        free_json(items[i].value);
        i++;
    }
    free(items);
}

static int  parse_map(json *dst, FILE *stream)
{
    if (!expect(stream, '{'))
        return (-1);

    pair    *items = NULL;
    size_t  size = 0;
    json    key;

    if (accept(stream, '}'))
    {
        dst->type = MAP;
        dst->map.size = 0;
        dst->map.data = NULL;
        return (1);
    }

    while (1)
    {
        pair    *tmp = realloc(items, sizeof(pair) * (size + 1));
        if (!tmp)
            return (free_items(items, size), -1);
        items = tmp;

        if (parse_str(&key, stream) == -1)
            return (free_items(items, size), -1);
        if (!expect(stream, ':'))
            return (free(key.string), free_items(items, size), -1);
        if (parse_value(&items[size].value, stream) == -1)
            return (free(key.string), free_items(items, size), -1);
        items[size].key = key.string;
        size++;
        if (accept(stream, ','))
            continue;
        else if (accept(stream, '}'))
            break;
        else
            return (unexpected(stream), free_items(items, size), -1);
    }
    dst->type = MAP;
    dst->map.size = size;
    dst->map.data = items;
    return (1);
}