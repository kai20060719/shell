
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>

#define MAX_ARGS 64
#define MAX_LINE 256
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

char* home_dir;

void wordsep(char* line, char** args) {
	int i = 0;
	args[i] = strtok(line, " \t\n");
	while (args[i] != NULL && i <= MAX_ARGS - 2) {
		i++;
		args[i] = strtok(NULL, " \n\t");
	}
	args[i] = NULL;
}

int  shell_prompt(){
	char cwd[PATH_MAX];
	char hostname[HOST_NAME_MAX];
	char* username = getlogin();
	
	home_dir = getenv("HOME");

	if(gethostname(hostname, sizeof(hostname)) != 0){
	perror("gethostname");
	return -1;
	}
	
	if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd");
        return -1;
        }

	if(strncmp(cwd, home_dir, strlen(home_dir)) == 0){
		cwd[0] = '~';
		memmove(cwd + 1, cwd + strlen(home_dir), strlen(cwd) - strlen(home_dir) + 1);
	}
	printf("[" COLOR_RED "%s" COLOR_RESET "@" COLOR_GREEN "%s" COLOR_RESET ":" COLOR_BLUE "%s" COLOR_RESET "]$ ",  username, hostname, cwd);
	fflush(stdout);
	
	return 0;
}

int main(void) {
	char line[MAX_LINE];
	char* args[MAX_ARGS];
	while (1) {
		if(shell_prompt() < 0){
		fprintf(stderr, "Prompt failed.\n");
		break;
		}

		if (fgets(line, sizeof(line), stdin) == NULL) {
			break;
		}
		wordsep(line, args);

		if (args[0] == NULL) {
			continue;
		}

		if (strcmp(args[0], "exit") == 0) {
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

	if(strcmp(args[0], "pwd") == 0){
	char cwd[PATH_MAX];
	if(getcwd(cwd, sizeof(cwd)) != NULL){
		printf("%s\n", cwd);
	}	
	else{
		perror("pwd");
	}
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

