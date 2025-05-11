#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#define MAX_ARGS 64
#define MAX_LINE 256

void wordsep(char* line, char** args) {
        int i = 0;
        args[i] = strtok(line, " \t\n");
        while (args[i] != NULL && i <= MAX_ARGS - 2) {
                i++;
                args[i] = strtok(NULL, " \n\t");
        }
        args[i] = NULL;
}

int main(void) {
        char line[MAX_LINE];
        char* args[MAX_ARGS];
        while (1) {
                printf("kai2006>");
                fflush(stdout);

                if (fgets(line, sizeof(line), stdin) == NULL) {
                        break;
                }
                wordsep(line, args);

                if (args[0] == NULL) {
                        continue;
                }                if (strcmp(args[0], "exit") == 0) {
                        break;
                }

                if (strcmp(args[0], "cd") == 0) {

                        if (args[1] != NULL){
                             if(chdir(args[1]) != 0)
                                  perror("cd");
                          }
                        else
                                fprintf(stderr, "cd:missing operand\n");
                        continue;
                }
pid_t pid = fork();

if(pid < 0){
        perror("Fork failded"); 
    }
else if(pid == 0) {
       if(execvp(args[0], args) < 0){
        perror("Execution failed");
        }
        _exit(1);
        }
        else{
        wait(NULL);
        }
      }
return 0;
}
