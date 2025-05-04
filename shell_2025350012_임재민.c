#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#define MAX_ARGS 64

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
	char line[256];
	char* args[MAX_ARGS];
	int i;
	while (fgets(line, sizeof(line), stdin) != NULL) {
		wordsep(line, args);
		for (i = 0; args[i] != NULL; i++) {
			printf("%s\n", args[i]);
		}
	}
	return 0;
}
