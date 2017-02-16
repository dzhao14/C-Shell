#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void
exec_background(char * command1, char** args1, char* command2, char** args2) {
	int cpid;
	if ((cpid = fork())) {
		int cpid2;
		if ((cpid2 = fork())) {
			int status2;
			waitpid(cpid2, &status2, 0);
			if (WIFEXITED(status2)) {
			}
		}
		else {
			execvp(command2, args2);
			printf("second half failed\n");
			exit(1);
		}
	}
	else {
		execvp(command1, args1);
		printf("first half fails.\n");
		exit(1);
	}	
}

void
exec_pipe(char * command1, char** args1, char* command2, char** args2) {
	int cpid;
	//int pipe_fds[2];
	//t err = pipe(pipe_fds);
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) {
			int cpid2;
			if ((cpid2 = fork())) {
				int status2;
				waitpid(cpid2, &status2, 0);
				if (WIFEXITED(status2)) {
				}
			}
			else {
				int in = open("temp.txt", O_RDONLY);
				dup2(in, 0);
				close(in);
				//dup2(pipe_fds[0], 0);
				//close(pipe_fds[1]);
				execvp(command2, args2);
				printf("second half failed\n");
				exit(1);
			}
		}
	}
	else {
		int in = open(args1[1], O_RDONLY);
		dup2(in, 0);
		close(in);
		int out = open("temp.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
		dup2(out, 1);
		close(out);
		//dup2(pipe_fds[1], 1);
		//close(pipe_fds[0]);
		execvp(command1, args1);
		printf("first half fails.\n");
		exit(1);
	}
}

int
exec_and_or(char * command, char** args) {
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) {
			return 0; 
		}
	}
	else {
		execvp(command, args);
		exit(1);
	}	
}

void
exec_semicolon(char * command, char** args) {
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) { }
	}
	else {
		execvp(command, args);
		exit(1);
	}
}

void
parse_input(char* input)
{
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) {
		}
	}
	else {
		int pipe = 0;
		int background = 0;
		char* backargs1[100];
		char* backargs2[100];
		char* pipeargs1[100];
		char* pipeargs2[100];
		char* args[100];
		int ii = 0;
		char* arg = strtok(input, " \n");
		while (arg) {
			//printf("arg is: %s\n", arg);
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
				exec_semicolon(args[0], args);
				args[0] = strtok(NULL, " \n");
				ii = 1;
			}
			else if (strcmp(arg, "&&") == 0) {
				args[ii] = 0;
				if (exec_and_or(args[0], args) == 0 && strcmp(args[0], "false") != 0) {
					char * command = strtok(NULL, " \n");
					args[0] = command;
					ii = 1;
				}
				else {
					exit(1);
				}
			}
			else if (strcmp(arg, "||") == 0) {
				args[ii] = 0;
				if (exec_and_or(args[0], args) != 0 || strcmp(args[0], "false") == 0) {
					char * command = strtok(NULL, " \n");
					args[0] = command;
					ii = 1;
				}
				else {
					exit(1);
				}
			}
			else if (strcmp(arg, "|") == 0) {
				pipe = 1;
				int jj = 0;
				for (jj; args[jj]; jj++) {
					pipeargs1[jj] = args[jj];
				}
				pipeargs1[jj] = 0;
				ii = 0;
			}
			else if (strcmp(arg, "&") == 0) {
				background = 1;
				int jj = 0;
				for (jj; args[jj]; jj++) {
					backargs1[jj] = args[jj];
				}
				backargs1[jj] = 0;
				ii = 0;
			}
			else {
				args[ii] = arg;
				ii++;
			}
			arg = strtok(NULL, " \n");
		}
		if (pipe == 0 && background == 0) {
 			args[ii] = 0;
			execvp(args[0], args);
			exit(1);
		}
		else if (background = 1) {
			args[ii] = 0;
			int jj = 0;
			for (jj; args[jj]!=0; jj++) {
				backargs2[jj] = args[jj];
			}
			backargs2[jj] = 0;
			exec_background(backargs1[0], backargs1, backargs2[0], backargs2);
			exit(1);
		}
		else {
			args[ii] = 0;
			int jj = 0;
			for (jj; args[jj]!=0; jj++) {
				pipeargs2[jj] = args[jj];
			}
			pipeargs2[jj] = 0;
			exec_pipe(pipeargs1[0], pipeargs1, pipeargs2[0], pipeargs2);
			exit(1);
		}
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
			char cmd_cp[256];
    		if (!fgets(cmd, 256, stdin)) {
				//EOF
				printf("\n");
				break;
			}
			strcpy(cmd_cp, cmd);
			char* command = strtok(cmd, " \n");
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
    		parse_input(cmd_cp);
		}
	}
	else {
		FILE *fp = fopen(argv[1], "r");
		char buffer[256] = {0};
		while (fgets(buffer, 256, fp) != NULL) {
			char buffer_cp[256];
			strcpy(buffer_cp, buffer);
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
    			parse_input(buffer_cp);
			}
		}
		fclose(fp);
	}

    return 0;
}
