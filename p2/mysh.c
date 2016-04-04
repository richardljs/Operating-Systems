#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define INPUT_SIZE 514
#define HISTORY_SIZE 20

char *words[INPUT_SIZE];
char *buffer[INPUT_SIZE];
char *command[INPUT_SIZE];
char *history[HISTORY_SIZE];
char *preToken, *postToken;
int is_batch = 0, is_redirection = 0;
int history_count = 0, history_head = 0, history_tail = 0;

char prompt_message[7] = "mysh # ";
char error_message[30] = "An error has occurred\n";

void prompt() {
    if (!is_batch)
        write(STDOUT_FILENO, prompt_message, strlen(prompt_message));
}

void error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int split(char *line, char *words[]) {
    int count = 0;
    char *input = strdup(line);
    while (1) {
        while (isspace(*input))
            input++;
        if (*input == '\0')
            return count;
        words[count++] = input;
        while (!isspace(*input) && *input != '\0')
            input++;
        if (*input == '\0') return count;
        *input++ = '\0';
    }
}

void addHistory(char *args) {
    history_count++;
    // When buffer is full
    if (history_tail == history_head - 1 || (history_tail + history_head) == 19) {
        history[history_head] = strdup(args);
        history_tail = history_head;
        history_head = (history_head+1) % HISTORY_SIZE;
    }
    // When buffer is not full
    else if (history_tail > history_head) {
        history[history_tail] = strdup(args);
        history_tail++;
    }
}

void printHistory(void) {
    int i, j;
    if (history_count < 20)
        for (i = 0; i < history_tail + 1; i++)
            printf("%d %s\n", i, history[i]);
    else {
        for (i = history_head; i < HISTORY_SIZE; i++)
            printf("%d %s\n", (history_count - HISTORY_SIZE + i - history_head), history[i]);
        for (j = 0; j <= history_tail; j++)
            printf("%d %s\n", history_count - history_tail + j, history[j]);
    }
}

int main (int argc, char *argv[]) {
    // Variables
    FILE *inFile;
    char input[INPUT_SIZE];
    int outFile, outFile_buf, i, rc;
    int count = 0;
    
    // Interactive or batch mode
    if (argc == 1)
        inFile = stdin;
    else if (argc == 2) {
        inFile = fopen(argv[1], "r");
        if (inFile == NULL) {
            error();
            exit(1);
        }
        is_batch = 1;
    }
    else {
        error();
        exit(1);
    }
    
    prompt();
    
    // Handle input
    while (fgets(input, INPUT_SIZE, inFile)) {
        
        // Input too long
        if (strlen(input) > 513) {
            if (is_batch)
                write(STDOUT_FILENO, input, strlen(input));
            error();
            prompt();
            continue;
        }
        
        // Empty command line
        if (split(input, buffer) == 0) {
            prompt();
            continue;
        }
        
        is_redirection = 0;
        preToken = NULL;
        postToken = NULL;
        
        if (is_batch)
            write(STDOUT_FILENO, input, strlen(input));
        
        // Search for redirection
        preToken = strtok(strdup(input), ">");
        if (strlen(preToken) != strlen(input)) {
            is_redirection = 1;
            postToken = strtok(NULL, ">");
            // Too many redirections
            if (strtok(NULL, ">")) {
                error();
                prompt();
                continue;
            }
        }
        
        // Handle redirection
        if (!is_redirection) {
            count = split(input, words);
            for (i = 0; i < count; i++)
                command[i] = strdup(words[i]);
            command[i] = NULL;
        }
        else {
            count = split(preToken, words);
            i = split(postToken, &words[count]);
            count += i;
            // No output file
            if (i != 1) {
                error();
                prompt();
                continue;
            }

            outFile_buf = dup(1);
            outFile = open(words[count - 1], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
            if (outFile < 0) {
                error();
                prompt();
                continue;
            }
            else if (dup2(outFile, 1) < 0) {
                error();
                prompt();
                continue;
            }
            close(outFile);
            
            for (i = 0; i < count - 1; i++)
                command[i] = strdup(words[i]);
            command[i] = NULL;
        }
        
        if (is_redirection) {

        }
        
        // Exit
        if (!strcmp("exit", words[0])) {
            if (count != 1) {
                error();
                prompt();
                continue;
            }
            else
                exit(0);
        }
        
        rc = fork();
        if (rc == 0) {
            execvp(command[0], command);
            error();
        }
        else if (rc < 0) {
            error();
            continue;
        }
        else
            (void) wait(NULL);
        
        prompt();
    }
    
    return 0;
}