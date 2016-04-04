#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define INPUT_SIZE 515
#define HIST_MAX 20

char prompt_message[7] = "mysh # ";
char error_message[30] = "An error has occurred\n";
char *str_history = "history";
char *str_exit = "exit";
int batch = 0;

typedef struct __list_history {
    unsigned int index;
    char *function;
} list_history;

void printPrompt() {
    if (!batch) {
        write(STDOUT_FILENO, prompt_message, strlen(prompt_message)); 
    }
}

void printError() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char const *argv[]) {
    char buffer[INPUT_SIZE];
    FILE *stream;
    char *tok = NULL;
    char *command = NULL;
    char *command2 = NULL; 
    char *redir = NULL;
    char *hist_redir = NULL;
    const char sp[2] = " ";
    list_history list[HIST_MAX];
    list_history hist_lookup;
    int numhist = 0;
    int hist_index = 0;
    char *str;
    int status;
    int length;
    int index = 0;
    int i = 0;
    int j;
    int fd_open;
    pid_t child, child_wait;

    if (argc > 2) {
        printError();
        exit(1);
    } else if (argc == 2) {
        batch = 1; /* default batch = 0 */
    }

    if (batch) {
        stream = fopen(argv[1], "r");
        if (stream == NULL) {
            printError();
            exit(1);
        }
    } else {
        stream = stdin;
    }

    do {
        if (!batch) {
            printPrompt();
        }

        str = fgets(buffer, INPUT_SIZE, stream);

        if (!str) {
            break;
        }

        if (batch) {
            if (strlen(buffer) <= 513) {
                write(STDOUT_FILENO, buffer, strlen(buffer));
            }
        }

        /* If too huge, cut it */
        if (strlen(buffer) > 513) {
            buffer[512] = '\n';
            buffer[513] = '\0';
            buffer[514] = '\0';
            write(STDOUT_FILENO, buffer, strlen(buffer));
            printError();
            continue;
        }

        /* Cutting the end of the buffer */
        if (strcmp( &buffer[strlen(buffer) - 1], "\n") == 0) {
            buffer[strlen(buffer) - 1] = '\0';
        }

        /* Look for redirection */
        redir = strchr(buffer, '>');
        if (redir != NULL) {
            length = (redir - buffer);
            redir ++;

            while (*redir == ' ') {
                redir ++;
            }

            if (strlen(redir) == 0 || strchr(redir, '>') != NULL || strchr(redir, ' ') != NULL) {
                printError();
                continue;
            }

            command = strndup(buffer, length);

            char *s = command + ((length - 1));

            while (s != command) {
                if (*s == ' ') {
                    *s = '\0';
                }
                else {
                    break;
                }
                s --;
            }

        } else {
            command = strdup(buffer);
        }

        command2 = strdup(command);

        tok = strtok(command, sp);

        /* Implementing the "!" method for history */
        if (*tok == '!') {

            char *histnum = strtok(NULL, " ");

            if (histnum) {
                i = atoi(histnum);
            } else if (strlen(tok) > 1) {
                tok ++;
                i = atoi(tok);
            }

            if (strtok(NULL, " ") != NULL || i <= 0 || i > numhist || i < numhist - HIST_MAX) {
                printError();
                continue;
            }

            if(i < HIST_MAX + 1) {
                index = i - 1;
            } else {
                index = i - 1 - HIST_MAX; 
            }

            hist_lookup = list[index];
            strcpy(buffer, hist_lookup.function);

            if (redir != NULL) hist_redir = strdup(redir);

            redir = strchr(buffer, '>');

            if (redir != NULL || hist_redir != NULL) {
                if (hist_redir != NULL && redir != NULL) {
                    printError();
                    continue;
                } else if (hist_redir != NULL && redir == NULL) {
                    redir = strdup(hist_redir);
                }

                if (hist_redir == NULL) {
                    length = redir - buffer;
                } else {
                    length = INPUT_SIZE;
                }

                if (hist_redir == NULL) {
                    redir ++;
                }

                while (*redir == ' ') {
                    redir ++;
                }

                if (strlen(redir) == 0 || strchr(redir, '>') != NULL || strchr(redir, ' ') != NULL) {
                    printError();
                    continue;
                }

                command = strndup(buffer, length);
                char *s = command + length - 1;

                while (s != command) {
                    if (*s == ' ') {
                        *s = '\0';
                    } else {
                        break;
                    }
                    s --;
                }
            } else {
                command = strdup(buffer);
            }
            
            command2 = strdup(command);
            tok = strtok(command, sp);
        }

        hist_lookup.index = numhist + 1;
        hist_lookup.function = strdup(buffer);
        list[hist_index] = hist_lookup;
        numhist++;
        hist_index++;

        /* Reindex for the circular array */
        if (hist_index == HIST_MAX) {
            hist_index = 0;
        }

        /* EXIT */
        if (strcmp(tok, str_exit) == 0) {
            if (strlen(command2) != 4) {
                printError();
            }
            exit(0);
        }

        /* Calling functions in /bin */
        child = fork();
        if (child == 0) { /* Child Process */

            int arg_num = 0;
            int i = 0;
            char *arg_ptr = command2;

            while (*arg_ptr == ' ') {
                arg_ptr ++;
            }

            char *end = command2 + (strlen(command2) - 1);

            while (*end == ' ') {
                *end = '\0';
                end --;
            }

            while (*arg_ptr) {
                char c = *arg_ptr;
                if (c == ' ') {
                    arg_num++;
                    while(*arg_ptr == ' ') {
                        arg_ptr ++;
                    }
                }
                arg_ptr ++;
            }

            char *arguments[arg_num + 2];

            for (i = 1; i < arg_num + 2; i++) {
                if (i <= arg_num) {
                    arguments[i] = strdup(strtok(NULL, " "));
                } else {
                    arguments[i] = NULL;
                }
            }

            arguments[0] = strdup(tok);

            /* Handle redirection */
            if (redir) {
                close(STDOUT_FILENO);
                fd_open = open(redir, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
                if (fd_open < 0) {
                    printError();
                    exit(1);
                }
            }

            /* HISTORY */
            if (strcmp(tok, str_history) == 0) {
                if (arg_num != 0) {
                    printError();
                    continue;
                }

                if (numhist > HIST_MAX) {
                    if (hist_index == HIST_MAX) {
                        j = 0;
                    } else {
                        j = hist_index;
                    }
                } else {
                    j = 0;
                }

                int low = (numhist < HIST_MAX) ? numhist : HIST_MAX;
                
                for (i = 0; i < low; i++) {
                    hist_lookup = list[j];
                    printf("%d %s\n", hist_lookup.index, hist_lookup.function);
                    j++;
                    if (j == HIST_MAX) {
                        j = 0;
                    }
                }

                exit(0);
            }

            execvp(tok, arguments);

            /* Fail if return */
            printError();
            exit(1);
        } else if (child > 0) {
            /* Wait till child is done */
            child_wait = wait(&status);
        } else {
            /* Error */
            printError();
            exit(1);
        }

    } while (str != NULL);

    return 0;
}