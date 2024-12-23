/**
 * @file shell.c
 *
 * @brief a simple POSIX/Unix shell 
 *
 * @author Divya Tiwari
 * Contact: divya.tiwari@colorado.edu
 *
 */

#include <stdio.h>
#include <stdio_ext.h>
#include <memory.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); fprintf(stderr, "Error: %s: %d\n", msg, en); exit(EXIT_SUCCESS); } while (0)

#define handle_error_en_no_exit(en, msg) \
        do { errno = en; fprintf(stderr, "Error: %s: %d\n", msg, en);} while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

/**
 * @brief  Checks the type of quote the character array/string has, if none returns the '\0' character.
 * @note   
 * @param  arr: character array to check character in. 
 * @retval '\'' or '"' or '\0'  
 */
char check_quote(char* arr){
    if(strchr(arr, '\'') != NULL){
        return '\'';
    } 
    else if (strchr(arr, '"') != NULL){
        return '"';
    }
    return '\0';
}

/**
 * @brief  Concatenate the strings split by spaces. Concatenates based on similar quotes and raises error when quotes mismatch.
 * @note   
 * @param  are[]: character pointer array of command split by spaces.
 * @param  i: starting position of the quoted string in the array
 * @param  final_quote_position: final position of the quoted string in the array.
 * @retval number of positions combined, this will be used in loop where concatenate loop is called for all sub-strings stripped by strtok.
 */
int concatenate_quotes(char* are[], int i, int final_quote_position){
    int reductions = 0;
    while((i != final_quote_position)){
        char* current = are[i];
        char* next    = are[i+1];
        // Find the end of the current string
        while(*current != '\0') current++;

        // Replace '\0' with space and shift the__fpurge next string
        *current = ' ';
        current++;

        // Copy the next string in place
        while(*next != '\0'){
            if(next == 0x0){
                // Shift all pointers to the left
                for(int j = i + 1; are[j] != NULL; j++){
                    are[j] = are[j + 1];
                }
                break;
            } 
            *current = *next;
            current++;
            next++;
        }

        *current = '\0'; //NULL Terminate the new string

        // Shift all pointers to the left
        for(int j = i + 1; are[j] != NULL; j++){
            are[j] = are[j + 1];
        }
        final_quote_position--; reductions++;
    }
    return reductions;
}

/**
 * @brief  Concatenates the quoted arguments passed by the user and tokenized by space character. 
 * @note   
 * @param  cmd[]: character pointer array of command after tokenizing based on spaces in characters.
 * @param  length: length of character pointer array
 * @retval returns 0 if command is correctly parsed and concatenated based on quotes. returns -1 if command has incorrect quotes.
 */
int quote_concat_ll(char* cmd[], const int length){
    int i = 0, count = 0;
    bool lock = false;
    int start_idx[50]; memset(start_idx, -1, sizeof(start_idx));
    int end_idx[50];   memset(end_idx, -1, sizeof(end_idx));
    char val = '0';

    while(cmd[i] != NULL){
        if((strchr(cmd[i], '\'') != NULL || strchr(cmd[i], '"') != NULL) && !lock){
            // Check if this string has complete quotes 
            bool lock_single = false, lock_double = false;
            for(int ptr = 0; ptr < strlen(cmd[i]); ptr++){
                if(cmd[i][ptr] == '\''){
                    lock_single = lock_single ? false : (true && !lock_double);
                }
                else if(cmd[i][ptr] == '"'){
                    lock_double = lock_double ? false : (true && !lock_single);
                }
            }
            // Handling cases when quote is incomplete but it can be found in next sub-strings
            if(lock_single || lock_double){
                lock = true;
                start_idx[count] = i;
                val = check_quote(cmd[i]);
            } 
            else{
                i++;
                continue; // Handling case when quote is complete in this substring, we do not consider it for next concatenation
            }
        }
        else if(check_quote(cmd[i]) == val && lock){
            end_idx[count] = i;
            lock = false;
            count++;
        }
        i++;
    }

    if(lock){
        handle_error_en_no_exit(0, "Quotes not closed properly");
        return -1;
    }

    int reductions = 0;
    for(int ptr = 0; ptr < count; ptr++){
       int pos =  ptr > 0 ? start_idx[ptr-1] + 1 : start_idx[ptr];
       if(start_idx[ptr] == -1 || end_idx[ptr] == -1) break;
       reductions = concatenate_quotes(cmd, start_idx[ptr] - reductions, end_idx[ptr] - reductions);
    }

    return 0;
}

/**
 * @brief  Tokenizes the command based on spaces, concatenates them based on quotes, omits the command based on mismatched quotes.
 * @note   
 * @param  cmd: pointer to character array that stores user input taken from fgets.
 * @param  command_split: pointer to character pointer array (thus pointer to pointer) where all the arguments and commands will be stored.
 * @retval returns 0 if command is correctly parsed and concatenated based on quotes. returns -1 if command has incorrect quotes. returns -2 if number of arguments for the command is >= 100, -3 if command is empty after tokenizing with space.
 */
int command_parser(char* cmd, char** command_split){
    if(strlen(cmd) <= 0) return -1;

    char* split_cmd = strtok(cmd, " "); // Command split on the basis of spaces
    int words = 0;
    while (split_cmd != NULL) {
        command_split[words++] = split_cmd;
        split_cmd = strtok(NULL, " ");
    }
    if (command_split[0] == NULL) return -3;
    command_split[words] = NULL;
    int status = quote_concat_ll(command_split, words);
    int count = 0; while(command_split[count] != 0x0) { command_split[count++];};
    if (count > 99) return -2; // Case when number of arguments is > 100 
    if (status == -1) return -1; // Case when quotes are incomplete 
    return 0; 
}

/**
 * @brief  Removes the quotes from starting and ending of the arguments since execvp doesn't require them to run the specific commands.
 * @note   
 * @param  args[]: array of character pointers that store commands and arguments. 
 * @retval None
 */
void strip_quotes(char* args[]) {
    int i = 0;
    while(args[i] != 0x0){
        size_t len = strlen(args[i]);
        if(len >= 2 && 
           ((args[i][0] == '\'' && args[i][len - 1] == '\'') ||
            (args[i][0] == '"' && args[i][len - 1] == '"'))) {
            // Shift the string left by one and null terminate appropriately
            memmove(&args[i][0], &args[i][1], len - 2);
            args[i][len - 2] = '\0';
        }
        i++;
    }
}

/**
 * @brief  Function that takes care of calling OS commands and child process management.
 * @note   This function is written to respect the priority of commands based on process management, specific function call and generalised execvp call.
 * @param  args[]: array of character pointers with command and arguments tokenized.
 * @retval None
 */
void invoke_cmd(char* args[]){
    int status;
    pid_t pid;
    
    // Strips the quotes from all arguments
    strip_quotes(args);

    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell.\n");
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(args[0], "cd") == 0){
        if(args[1] == NULL) return;
        size_t len = strlen(args[1]);
        if(len < 1){handle_error_en_no_exit(errno, "cd failed invalid argument"); return;}
        if (chdir(args[1]) != 0){
            handle_error_en_no_exit(errno, "cd failed"); return;
        }
    }
    else{
        pid = fork();

        if (pid < 0) {
            // Fork failed
            handle_error_en_no_exit(errno, "fork failed"); return;
        }
        else if (pid == 0){
            //Child PID
            // Execute the command
            int resp = execvp(args[0], args);
            if (resp == -1) {
                handle_error_en(errno, "execvp failed");
            }
            else if(resp != 0){
                handle_error_en(errno, "command exited with code");
            }
        }
        else{
            // Parent process
            if (waitpid(pid, &status, 0) == -1) {
                handle_error_en_no_exit(errno, "waitpid failed");
            }
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stdout,"Error: Command exited with code %d\n", WEXITSTATUS(status));
            }
        }
    }
}

/**
 * @brief  Driver function for the shell, prints '$' and let's user enter commands of upto 1000 chars, or processes the first 1000 characters.
 * @note   Processes the eof, ctrl-c signal and no input cases. Removes '/n' at the end with '/0'
 * @retval None
 */
int call_cmd(){

    while(1){
        char cmd[1000] = {'\0'};
        char* cmd_parsed[1000];
        printf("$ ");
        if(fgets(cmd, sizeof(cmd), stdin) == NULL){
            if (feof(stdin)) {
                printf("\nEOF encountered.\n");
                break;
            } else if (ferror(stdin)) {
                handle_error_en_no_exit(errno, "Error taking input please enter again. ");
                continue;
            } else {
                printf("No input.\n");
                continue;
            }
        }

        size_t len = strlen(cmd);
        if(cmd[len -1] != '\n'){handle_error_en_no_exit(errno, " Number of input characters more than 1000"); __fpurge(stdin); continue;}
        if (len > 0 && cmd[len - 1] == '\n') {
            cmd[len - 1] = '\0';
        }

        int res = command_parser(cmd, cmd_parsed);
        if(res == -1) continue;
        else if (res == -2) {handle_error_en_no_exit(errno, " Number of arguments more than 100"); __fpurge(stdin); continue;}
        else if (res == -3) {handle_error_en_no_exit(errno, " Command entered with spaces only. Please input a valid command."); __fpurge(stdin); continue;}
        invoke_cmd(cmd_parsed);
    }
    return 0;
}

void signal_callback_handler(int signum){
    handle_error_en(errno, "SIGINT caught");
}

/**
 * @brief  Main function that calls the driver function.
 * @note   
 * @retval 
 */
int main(){
    signal(SIGINT, signal_callback_handler);
    int result = call_cmd();
    return 0;
}