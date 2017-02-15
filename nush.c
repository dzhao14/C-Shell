#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void
parse_input(char* command, char* input)
{
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) {
		}
	}
	else {
		char* args[100];
		args[0] = command;
		int ii = 1;
		char* arg = strtok(input, " \n");
		while (arg) {
			if (strcmp(arg, ">") == 0) {
				char* file = strtok(NULL, " \n");
				int out = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
				dup2(out, 1);
				close(out);
			}
			else if (strcmp(arg, "<") == 0) {
				char* file = strtok(NULL, " \n");
				int in = open(file, O_RDONLY);
				dup2(in, 0);
				args[ii] = file;
				ii++;
				close(in);
			}
			else if (strcmp(arg, ";") == 0) {
				args[ii] = 0;
				execvp(command, args);
			}
			else {
				args[ii] = arg;
				ii++;
			}
			arg = strtok(NULL, " \n");
		}
 		args[ii] = 0;
	
		execvp(command, args);
	}
}

int
main(int argc, char* argv[])
{
    if (argc == 1) {
		while (1 == 1) {
        	printf("nush$ ");
        	fflush(stdout);
    		char cmd[256];
			cmd[0] = 0;
    		if (!fgets(cmd, 256, stdin)) {
				//EOF
				printf("\n");
				break;
			}
			char* command = strtok(cmd, " \n");
			char* input = strtok(NULL, "\n;");
			if (!command) {
				//do nothing because the user just hit enter
			}
			else if (strcmp(command, "exit") == 0) {
				break;
			}
			else if (strcmp(command, "cd") == 0) {
				chdir(input);
			}
    		parse_input(command, input);
		}
	}
	else {
		FILE *fp = fopen(argv[1], "r");
		char buffer[256] = {0};
		while (fgets(buffer, 256, fp) != NULL) {
			char* command = strtok(buffer, " \n");
			char* input = strtok(NULL, "\n");
			if (!command) {
				//do nothing because the user just hit enter
			}
			else if (strcmp(command, "exit") == 0) {
				break;
			}
			else if (strcmp(command, "cd") == 0) {
				chdir(input);
			}
			else {
    			parse_input(command, input);
			}
		}
		fclose(fp);
	}

    return 0;
}
