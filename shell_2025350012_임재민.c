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

int command(char* line){
    char* args[MAX_ARGS];
    wordsep(line, args);
	if (args[0] == NULL) {
			return 1;
		}

		if (strcmp(args[0], "exit") == 0) {
			exit(0);
		}

		if (strcmp(args[0], "cd") == 0) {

			if (args[1] != NULL){
		 if (strcmp(args[1], "~") == 0) {  
               	 args[1] = home_dir;  
           		 }
		 if(chdir(args[1]) != 0){
                      perror("cd");
			 return 1;
                 }
		return 0;
		}
			else{
			fprintf(stderr, "cd:missing operand\n");
			return 1;
		}
		}

	if(strcmp(args[0], "pwd") == 0){
	char cwd[PATH_MAX];
	if(getcwd(cwd, sizeof(cwd)) != NULL){
		printf("%s\n", cwd);
	 fflush(stdout);
	}	
	else{
		perror("pwd");
		return 1;
	}
	return 0;
	}
pid_t pid = fork();

if(pid < 0){
	perror("Fork failded");	
	return 1;
    }
else if(pid == 0) {
       if(execvp(args[0], args) < 0){
	perror("Execution failed");
	
	}
	_exit(1);
	}
	else{
	int status;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status)){
		return WEXITSTATUS(status);
	}
	}
	return 1;
      }

int  pipeline_execute(char* line){
	char* commands[MAX_ARGS];
	 int num_commands = 0;

 	 commands[num_commands] = strtok(line, "|");
    	while (commands[num_commands] != NULL) {
        	num_commands++;
       		 commands[num_commands] = strtok(NULL, "|");
   	 }

	int i, in_fd = 0, status = 0;
	
	for(i = 0; i < num_commands; i++){
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1) {
   	 perror("Pipe creation failed");
   	 return 1;
	}


	if(fork() == 0){
	dup2(in_fd, 0);

	if(i < num_commands - 1){
	dup2(pipe_fd[1], 1);
	}

	close(pipe_fd[0]);
	close(pipe_fd[1]); 
	char* args[MAX_ARGS];
	wordsep(commands[i], args);

	if(execvp(args[0], args) < 0){
	perror("Execution failed");
	_exit(1);
	}
	}
	else{
	wait(&status);
	close(pipe_fd[1]);
	if(in_fd != 0){
		close(in_fd);
	}
	in_fd = pipe_fd[0];
	}	
	}
	while (wait(&status) > 0) {     
	 if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            return WEXITSTATUS(status);          }
    } else {
                return 1;      
	}
	}
	return 0;
}



void multiple_commands(char* line) {
    char* m_command;
    int prev_success = 1;


    
    m_command = strtok(line, ";");

    while (m_command != NULL) {
       
        while (*m_command == ' ' || *m_command == '\t') {
            m_command++;
        }

        
        char* and_split = strstr(m_command, "&&");
        char* or_split = strstr(m_command, "||");

        if (and_split) {
            *and_split = '\0';  
            prev_success = command(m_command);

            if (prev_success == 0) {
                prev_success = command(and_split + 2);  
            }
        } else if (or_split) {
            *or_split = '\0';  
            prev_success = command(m_command);

            if (prev_success != 0) {
                prev_success = command(or_split + 2);  
            }
        } else {
        	if(strstr(m_command, "|")){
			prev_success = pipeline_execute(m_command);
		}
		else
			prev_success = command(m_command);
		}     
         m_command = strtok(NULL, ";");
	
	}
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
		multiple_commands(line);
	}
	return 0;
}

