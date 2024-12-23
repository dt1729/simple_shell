
# Simple POSIX/Unix Shell

A lightweight and straightforward POSIX/Unix-compatible shell implemented in C, designed to execute standard Unix commands, handle user input with proper quote management, and manage basic shell functionalities such as changing directories, executing programs and exiting the shell.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Compilation](#compilation)
- [Usage](#usage)
- [Built-in Commands](#built-in-commands)
- [Examples](#examples)
- [Error Handling](#error-handling)
- [Author](#author)
- [License](#license)

## Features

- **Command Execution**: Execute standard Unix commands using `execvp`.
- **Command Execution with absolute paths**: Executes standard Unix commands even when they're given from their absolute paths.
- **Built-in Commands**: Supports `cd` for changing directories and `exit` to terminate the shell.
- **Quote Handling**: Properly parses and concatenates arguments enclosed in single (`'`) or double (`"`) quotes, ensuring commands with spaces or special characters are handled correctly.
- **Error Management**: Provides informative error messages for issues such as failed command execution, improper usage, or signal interruptions.
- **Signal Handling**: Catches `SIGINT` (Ctrl+C) to allow graceful termination of the shell without crashing the shell.
- **Input Parsing**: Efficiently tokenizes user input, handles quoted strings, and limits the number of arguments to prevent excessive memory usage.

## Prerequisites

- **Operating System**: Linux OS (Tested on Ubuntu 20.04)
- **Compiler**: gcc version 9.4.0 
- **Libraries**: Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, etc.)

## Compilation

To compile the shell, ensure you have GCC installed on your system. Use the following command in your terminal:

```bash
gcc -o simple_shell shell.c
```

**Explanation of Flags:**

- `-o simple_shell`: Specifies the output executable name as `simple_shell`.
- `shell.c`: The source code file. Replace this with the actual filename if it's different.

## Usage

After successful compilation, run the shell using:

```bash
./simple_shell
```

You will be greeted with a prompt `$ ` where you can enter your commands.

### Exiting the Shell

To exit the shell, type:

```bash
exit
```
alternatively, you can do ctrl-c and exit the shell using interrupt. Ctrl-D(EOF) can also be used to exit the shell.

### **Examples of usage:**

  ```bash
  $ cd /usr/local
  ```

### `exit`

Terminate the shell session.

**Usage:**

```bash
exit
```

- **Example:**

  ```bash
  $ exit
  Exiting shell.
  ```

## Examples

1. **Listing Files:**

   ```bash
   $ ls -l
   total 12
   -rwxr-xr-x 1 user user 12345 Dec 1 12:00 simple_shell
   ```

2. **Making directories with nested quotes:**

    ```bash
    $ mkdir '/tmp/"hello world"'
    $ cd '/tmp/"hello world"'
    $ pwd 
    /tmp/"hello world"
    ```

3. **Changing Directory and Printing Working Directory:**

   ```bash
   $ cd /tmp
   $ pwd
   /tmp
   ```

4. **Echoing a String with Quotes:**

   ```bash
   $ echo "Hello, World!"
   Hello, World!
   ```

5. **Handling Command with Mismatched Quotes:**

   ```bash
   $ echo "This will fail
   Error: Quotes not closed properly: 0
   ```

## Error Handling

- **Invalid Commands:** Displays an error message if a command is not found or fails to execute.

  ```bash
  $ invalidcommand
    execvp failed: No such file or directory: 2
  ```

- **Mismatched Quotes:** Alerts the user if quotes are not properly closed.

  ```bash
  $ echo "Unclosed string
    Error: Quotes not closed properly: 0
  ```

- **Too Many Arguments:** Limits the number of arguments to 100 and notifies the user if exceeded.

  ```bash
  $ command arg1 arg2 ... arg101
  Error: Number of arguments more than 100
  ```

- **Too many characters:** Limits the number of arguments to 1000 and notifies the user if exceeded.
  ```bash
  $ echo 1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
    Error:  Number of input characters more than 1000: 0
  $
  ```


- **Signal Interruptions:** Handles `SIGINT` (Ctrl+C) and exits the shell gracefully.

  ```bash
  $ ^CSIGINT caught: Success
    Error: SIGINT caught: 0
  ```

- **Process exit code:** Shows the exit code with which the child process being executed, exited with.

  ```bash
  $ /bin/sh -c 'exit 7'
    Error: Command exited with code 7
  ```

## Author

**Divya Tiwari**  
Contact: [divya.tiwari@colorado.edu](mailto:divya.tiwari@colorado.edu)
