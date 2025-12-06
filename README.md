# C Utilities Collection

This repository contains a collection of advanced C utilities developed to demonstrate systems programming skills. These programs cover a range of topics including process management, inter-process communication, signal handling, JSON parsing, and recursive descent parsing techniques.

**"Advanced systems programming utilities in C"**

## Table of Contents

- [Overview](#overview)
- [Utilities](#utilities)
  - [ft_popen](#ft_popen)
  - [picoshell](#picoshell)
  - [sandbox](#sandbox)
  - [argo](#argo)
  - [vbc](#vbc)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
- [Compilation](#compilation)
- [Acknowledgments](#acknowledgments)
- [Disclaimer](#disclaimer)
- [License](#license)

## Overview

This collection demonstrates proficiency in:
- **Process creation and management** - using `fork()`, `execvp()`, `wait()`
- **Inter-process communication** - pipes and file descriptors
- **Signal handling** - `sigaction()`, `alarm()`, `kill()`
- **File I/O** - stream processing with `FILE*`
- **Recursive descent parsing** - building parsers for structured input
- **Memory management** - dynamic allocation and avoiding memory leaks

All utilities are written in C and follow best practices for systems programming, including proper error handling and resource cleanup.

## Utilities

### ft_popen

**Files:** `ft_popen.c`  
**Allowed functions:** `pipe`, `fork`, `dup2`, `execvp`, `close`, `exit`

A custom implementation of the `popen()` system call. This function launches an executable with specified arguments and returns a file descriptor connected to either the input or output of the command.

**Function signature:**
```c
int ft_popen(const char *file, char *const argv[], char type);
```

**Parameters:**
- `file` - the executable to launch
- `argv` - arguments array (usable with `execvp`)
- `type` - `'r'` for reading from command output, `'w'` for writing to command input

**Returns:** A file descriptor on success, -1 on error

**Example:**
```c
int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
// Read the output of ls from fd
char c;
while (read(fd, &c, 1))
    write(1, &c, 1);
close(fd);
```

**Key Learning:** Process creation, pipe manipulation, and file descriptor redirection.

### picoshell

**Files:** `picoshell.c`  
**Allowed functions:** `close`, `fork`, `wait`, `exit`, `execvp`, `dup2`, `pipe`

A pipeline executor that mimics shell behavior. Takes a null-terminated array of commands and connects the output of each command to the input of the next, just like the shell pipe operator (`|`).

**Function signature:**
```c
int picoshell(char **cmds[]);
```

**Parameters:**
- `cmds` - null-terminated array of command arrays (each command is ready for `execvp`)

**Returns:** 0 on success, 1 on error or if any command fails

**Example:**
```c
char *cmd1[] = {"cat", "file.txt", NULL};
char *cmd2[] = {"grep", "pattern", NULL};
char *cmd3[] = {"wc", "-l", NULL};
char **cmds[] = {cmd1, cmd2, cmd3, NULL};
int result = picoshell(cmds);
// Executes: cat file.txt | grep pattern | wc -l
```

**Key Learning:** Managing multiple pipes, proper construction/destruction order, handling multiple child processes.

### sandbox

**Files:** `sandbox.c`  
**Allowed functions:** `fork`, `waitpid`, `exit`, `alarm`, `sigaction`, `kill`, `printf`, `strsignal`, `errno`

A function tester that executes a function in an isolated environment with a timeout. Determines if a function is "nice" (exits normally with code 0) or "bad" (crashes, times out, or exits with non-zero status).

**Function signature:**
```c
int sandbox(void (*f)(void), unsigned int timeout, bool verbose);
```

**Parameters:**
- `f` - function pointer to test
- `timeout` - maximum execution time in seconds
- `verbose` - if true, prints diagnostic messages

**Returns:** 1 if function is nice, 0 if bad, -1 on error

**Verbose output messages:**
- `"Nice function!\n"` - function completed successfully
- `"Bad function: exited with code <n>\n"` - non-zero exit
- `"Bad function: <signal>\n"` - terminated by signal (segfault, abort, etc.)
- `"Bad function: timed out after <n> seconds\n"` - exceeded timeout

**Example:**
```c
void good_function(void) {
    // Does some work and returns normally
}

void bad_function(void) {
    int *p = NULL;
    *p = 42;  // Segmentation fault
}

sandbox(good_function, 5, true);  // Returns 1, prints "Nice function!"
sandbox(bad_function, 5, true);   // Returns 0, prints "Bad function: Segmentation fault"
```

**Key Learning:** Signal handling, process isolation, timeouts with `alarm()`, comprehensive error detection.

### argo

**Files:** `argo.c`, `argo.h`, `given.c`  
**Allowed functions:** `malloc`, `calloc`, `realloc`, `free`, `printf`, `getc`, `ungetc`, `isdigit`, `fscanf`, `write`

A recursive descent JSON parser that reads a file character-by-character and builds an Abstract Syntax Tree (AST). Supports three JSON types: integers, strings, and maps (objects).

**Function signature:**
```c
int argo(json *dst, FILE *stream);
```

**Parameters:**
- `dst` - pointer to json structure to store parsed result
- `stream` - FILE pointer to read from

**Returns:** 1 on success, -1 on error

**Supported JSON types:**
```c
typedef struct json {
    enum { MAP, INTEGER, STRING } type;
    union {
        struct { struct pair *data; size_t size; } map;
        int integer;
        char *string;
    };
} json;
```

**Features:**
- Recursive parsing of nested maps
- String escaping support (`\\` and `\"` only)
- Whitespace is treated as error (not ignored like standard JSON)
- Proper error messages: `"Unexpected token 'x'"` or `"Unexpected end of input"`

**Example:**
```c
FILE *fp = fopen("test.json", "r");
json result;
if (argo(&result, fp) == 1) {
    serialize(result);  // Print the parsed JSON
    free_json(result);  // Clean up
}
fclose(fp);
```

**Key Learning:** Recursive descent parsing, AST construction, dynamic memory management, stream processing.

### vbc

**Files:** `vbc.c`  
**Allowed functions:** `malloc`, `calloc`, `realloc`, `free`, `printf`, `isdigit`, `write`

A mathematical expression evaluator using recursive descent parsing. Evaluates expressions with addition, multiplication, and parentheses, respecting operator precedence.

**Program behavior:**
```
./vbc '<expression>'
```

**Supported operations:**
- Addition (`+`)
- Multiplication (`*`)
- Parentheses (`()`)
- Single-digit numbers (0-9)

**Operator precedence:**
- Multiplication has higher precedence than addition
- Parentheses override default precedence

**Examples:**
```sh
$ ./vbc '1'
1
$ ./vbc '2+3'
5
$ ./vbc '3*4+5'
17
$ ./vbc '3+4*5'
23
$ ./vbc '(3+4)*5'
35
$ ./vbc '1+'
Unexpected end of input
$ ./vbc '1+2)'
Unexpected token ')'
```

**Key Learning:** Recursive descent parsing, expression evaluation, operator precedence, grammar implementation.

## Project Structure

```
C-Utilites-01/
├── ft_popen/
│    └── ft_popen.c
├── picoshell/
│    └── picoshell.c
├── sandbox/
│    └── sandbox.c
├── argo/
│    ├── argo.c
│    ├── argo.h
│    ├── utilities.c
│    └── test.json
├── vbc/
│    ├── vbc_v2.c
│    └── vbc.c
└── README.md
```

## Installation

1. **Clone the Repository:**

   ```sh
   git clone https://github.com/marco-ht/utilities.git
   cd C-Utilites-01
   ```

2. **Compile Individual Utilities:**

   Each utility can be compiled separately:

   ```sh
   cd ft_popen
   gcc -Wall -Wextra -Werror ft_popen.c -o ft_popen
   cd ../picoshell
   gcc -Wall -Wextra -Werror picoshell.c -o picoshell
   cd ../sandbox
   gcc -Wall -Wextra -Werror sandbox.c -o sandbox
   cd ../argo
   gcc -Wall -Wextra -Werror argo.c utilities.c -o argo
   cd ../vbc
   gcc -Wall -Wextra -Werror vbc.c -o vbc
   OR
   gcc -Wall -Wextra -Werror vbc_v2.c -o vbc
   ```

## Usage

### ft_popen

```c
int fd = ft_popen("ls", (char *const[]){"ls", "-l", NULL}, 'r');
if (fd == -1)
    return (1);
char buffer[1024];
ssize_t n = read(fd, buffer, sizeof(buffer));
write(1, buffer, n);
close(fd);
wait(NULL);  // Don't forget to wait for child process
```

### picoshell

```c
char *cmd1[] = {"cat", "file.txt", NULL};
char *cmd2[] = {"grep", "search", NULL};
char **cmds[] = {cmd1, cmd2, NULL};
int result = picoshell(cmds);
```

### sandbox

```c
void test_function(void) {
    // Your code here
}

int result = sandbox(test_function, 5, true);
// result: 1 = nice, 0 = bad, -1 = error
```

### argo

```sh
./argo test.json
```

### vbc

```sh
./vbc '(2+3)*4'
```

## Compilation

All utilities should be compiled with:
```sh
gcc -Wall -Wextra -Werror <source_files> -o <output>
```

For sandbox, you may need to link against additional libraries on some systems:
```sh
gcc -Wall -Wextra -Werror sandbox.c -o sandbox
```

## Acknowledgments

- Thanks to the systems programming community for documentation and best practices.
- Special thanks to resources on UNIX process management, signal handling, and parsing techniques.

## Disclaimer

**IMPORTANT**:
This repository is published for demonstration purposes to showcase advanced systems programming skills in C. The code demonstrates proficiency in process management, IPC, signal handling, parsing, and memory management.

**Academic Integrity Notice**:
These utilities are provided as examples of systems programming techniques. If you're working on similar assignments, use this code as a learning reference only. Copying code for academic submissions is prohibited and violates academic integrity policies.

## License

This repository is licensed under the Creative Commons - NonCommercial-NoDerivatives (CC BY-NC-ND 4.0) license. You are free to share this repository as long as you:

- Provide appropriate credit.
- Do not use it for commercial purposes.
- Do not modify, transform, or build upon the material.

For further details, please refer to the CC BY-NC-ND 4.0 license documentation.

Developed with dedication and attention to systems programming best practices.
