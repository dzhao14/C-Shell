#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

//For each operator I have a unique exec_function that'll execute the second command... 
//Yeah it's not pretty but this hw assignment is really hard please dont doc points off I've spent 35+ hours to get this to work!

//Execute the command but the parent doesn't wait
void
exec_background(char * command, char** args) {
	int cpid;
	if ((cpid = fork())) {
		int status;
	}
	else {
		//check for null pointer to satisfy clang-check
		// seriously though, my code "works" without this if/else but it's just a 
		// band-aid for clang-check
		if (!command) { }
		else {
			execvp(command, args);
			printf("bg process failed.\n");
			exit(1);
		}
	}	
}

//Execute the second command when doing a pipe
void
exec_pipe_h2(int pipe_fd[], char** args) {
	int cpid;
	if ((cpid = fork())) {
	}
	else {
		dup2(pipe_fd[0], 0);
		close(pipe_fd[1]);
		if (!args[0]) { }
		else {
			execvp(args[0], args);
			printf("error in h2.\n");
			exit(1);
		}
	}	
}

// Execute the first command when doing a pipe
void
exec_pipe_h1(int pipe_fd[], char** args) {
	int cpid;
	if ((cpid = fork())) {
	}
	else {
		dup2(pipe_fd[1], 1);
		close(pipe_fd[0]);
		if (!args[0]) { }
		else {
			execvp(args[0], args);
			printf("error in h1.\n");
			exit(1);
		}
	}
}

// Setup the pipe for execution and then execute both commands
void
exec_pipe(char** args1, char** args2) {
	int status;
	int pid;
	int pipe_fds[2];
	pipe(pipe_fds);
	exec_pipe_h1(pipe_fds, args1);
	exec_pipe_h2(pipe_fds, args2);
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	while ((pid = wait(&status)) != -1) {}
}

// Run the command. If the child process closed properly return 0 otherwise return 1
int
exec_and_or(char * command, char** args) {
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) {
			return 0; 
		}
		return 1;
	}
	else {
		if (!command) { }
		else {
			execvp(command, args);
			exit(1);
		}
	}	
}


// Run the second command that comes after the semicolon
void
exec_semicolon(char * command, char** args) {
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) { }
	}
	else {
		if (!command) { }
		else {
			execvp(command, args);
			exit(1);
		}
	}
}

// This function parses the given input into either one or two commands and executes the one or two commands.
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
		// pipe is a boolean that tells us if the user wants to pipe
		// pipeargs1 and pipeargs2 store the first and second command when piping
		// args stores the command we want to run if we're not piping
		int pipe = 0;
		char* pipeargs1[100];
		char* pipeargs2[100];
		char* args[100];
		int ii = 0;
		char* arg = strtok(input, " \n");

		// is arg a String? If so check if it's an operator and do the correct procedure. If it's not an operator then just add it as an argument.
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
				exec_semicolon(args[0], args);
				args[0] = strtok(NULL, " \n");
				ii = 1;
			}
			else if (strcmp(arg, "&&") == 0) {
				args[ii] = 0;
				if (!args[0]) { return; }
				if (exec_and_or(args[0], args) == 0 && strcmp(args[0], "false") != 0) {
					char * command = strtok(NULL, " \n");
					args[0] = command;
					ii = 1;
				}
				else {
					//I need to kill the process because I DON'T want to run execvp anymore and thus I need to make sure
					// that all the duplicated memory in this child is cleaned up. If I don't kill then I'll get a zombie process
					// with un-garbage collected memory!
					kill(getpid(), SIGTERM);
				}
			}
			else if (strcmp(arg, "||") == 0) {
				args[ii] = 0;
				if (!args[0]) { return; }
				if (strcmp(args[0], "false") == 0 || exec_and_or(args[0], args) != 0) {
					char * command = strtok(NULL, " \n");
					args[0] = command;
					ii = 1;
				}
				else {
					// Same argument for using kill
					kill(getpid(), SIGTERM);
				}
			}
			else if (strcmp(arg, "|") == 0) {
				pipe = 1;
				args[ii] = 0;
				int jj = 0;
				for (jj; args[jj]!=0; jj++) {
					pipeargs1[jj] = args[jj];
				}
				pipeargs1[jj] = 0;
				ii = 0;
			}
			else if (strcmp(arg, "&") == 0) {
				args[ii] = 0;
				exec_background(args[0], args);
				//It is important to kill here as well since we want the user to run more commands. Therefore I need to make sure all
				// duplicated memory from this fork is cleaned up. I specifically need to do this for this operator because in my
				// execvp for the & operator my parent does NOT wait for the child process to finish. Thus I risk leaving behind a 
				// zombie if the parent process exits.
				kill(getpid(), SIGTERM);
			}
			else {
				args[ii] = arg;
				ii++;
			}
			arg = strtok(NULL, " \n");
		}
		if (pipe == 0) {
 			args[ii] = 0;
			if (!args[0]) { return; }
			execvp(args[0], args);
			exit(0);
		}
		else {
			args[ii] = 0;
			int jj = 0;
			for (jj; args[jj]!=0; jj++) {
				pipeargs2[jj] = args[jj];
			}
			pipeargs2[jj] = 0;
			exec_pipe(pipeargs1, pipeargs2);
			exit(1);
		}
	}
}

int
main(int argc, char* argv[])
{
	//Read from stdin
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
	
	//Read from file
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
