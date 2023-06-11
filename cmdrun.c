/*
 * Skeleton code for Lab 2 - Shell processing
 * This file contains skeleton code for executing parsed commands.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "cmdparse.h"
#include "cmdrun.h"

static pid_t
cmd_exec(command_t *cmd, int *pass_pipefd)
{
        (void)pass_pipefd;      // get rid of unused warning
	pid_t pid = -1;		// process ID for child
	int pipefd[2];		// file descriptors for this process's pipe
	int cmd_status;		// cmd status for Sub-shell
	int exitFlag = 0;
	int exit_satus = 0;
	
	
		
	pid = fork();
	if ( pid == -1 ) {
		perror("fork");
		return -1;
	}

	// Create a pipe, if this command is the left-hand side of a pipe.
	// Return -1 if the pipe fails.
	if (cmd->controlop == CMD_PIPE) {
		/* Your code here*/
		if (pipe(pipefd) < 0)
			perror("pipe");
		
	}
	
	

	/* Your code here */
	
	if ( pid == 0 ) {
	
	// Execute Subshell
		if(cmd->subshell) {
		cmd_status = cmd_line_exec(cmd->subshell);
		
		if (cmd_status != 0) {
			cmd_status = 5;
		}
		
		//printf("--%d--",cmd_status );
		_exit(cmd_status);
		}
		
	// Execute Input Redirection 
		if (cmd->redirect_filename[0] != NULL ) {
			int fd_input = open(cmd->redirect_filename[0], O_RDONLY);
			dup2(fd_input,STDIN_FILENO);
			close(fd_input);
		
		}
		
	// Execute Output Redirection 
		if (cmd->redirect_filename[1] != NULL ) {
			int fd_out = open(cmd->redirect_filename[1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
			dup2(fd_out,STDOUT_FILENO);
			close(fd_out);
		}
	
	// Execute Error Redirection 
		if (cmd->redirect_filename[1] != NULL ) {
			int fd_out = open(cmd->redirect_filename[2], O_WRONLY|O_CREAT|O_TRUNC, 0666);
			dup2(fd_out,STDERR_FILENO);
			close(fd_out);
		}
		
		// Special Case - EXIT 
	if (strcmp(cmd->argv[0],"exit") == 0) {
		//exit(0);
			if (cmd->argv[2]) {
				perror("exit: too many args");
				return pid;
			}
			
			else if (cmd->argv[1]) {
				exit_satus = atoi(cmd->argv[1]);
			
			// Non-numerical value
				if(exit_satus == 0) 
				 	execvp("echo",cmd->argv);	
				
				_exit(exit_satus);
				exitFlag = 1;
			}
		
			else {
				_exit(exit_satus);
				exitFlag = 1;
			}
			
		}
	// Special Case - PWD 	
	if (strcmp(cmd->argv[0],"our_pwd") == 0) {
		if (cmd->argv[1]) {
				perror("pwd: Syntax Error! Wrong number of arguments!");
				return pid;
			}
			
		
			else {
				cmd->argv[0] = "pwd";
			}	
				
	}
	
	// Special Case - CD 
	if (strcmp(cmd->argv[0],"cd") == 0) {
			if (cmd->argv[2]) {
				perror("pwd: Syntax Error! Wrong number of arguments!");
				return pid;
			}
			else if (!cmd->argv[1]) {
				perror("pwd: Syntax Error! Wrong number of arguments!");
				return pid;
			}
			else {
				chdir(cmd->argv[1]);
				perror("chdir");
				return pid;
			}	
				
	}
	
	// Handling pipes
	if (cmd->controlop == CMD_PIPE) {
		close(pipefd[0]);
		if ( dup2(pipefd[1], STDOUT_FILENO) == -1 ) {
		perror("dup2");
		_exit(1);
		}
		close(pipefd[1]);
		
	}
		
	
	execvp(cmd->argv[0],cmd->argv);
	perror("execvp");
	_exit(0);
	}
	//Parent process
	else if(pid > 0){
	
		
	}
	else {
	perror("Error: Failed to fork");
	}
	
	
	


	// return the child process ID
	return pid;
	
	
	
}



/* cmd_line_exec(cmdlist)
 *
 *   Execute the command list.
 *
 *   Execute each individual command with 'cmd_exec'.
 *   String commands together depending on the 'cmdlist->controlop' operators.
 *   Returns the exit status of the entire command list, which equals the
 *   exit status of the last completed command.
 *
 *   The operators have the following behavior:
 *
 *      CMD_END, CMD_SEMICOLON
 *                        Wait for command to exit.  Proceed to next command
 *                        regardless of status.
 *      CMD_AND           Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status 0.  Otherwise
 *                        exit the whole command line.
 *      CMD_OR            Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status != 0.
 *                        Otherwise exit the whole command line.
 *      CMD_BACKGROUND, CMD_PIPE
 *                        Do not wait for this command to exit.  Pretend it
 *                        had status 0, for the purpose of returning a value
 *                        from cmd_line_exec.
 */
int
cmd_line_exec(command_t *cmdlist)
{
	int cmd_status = 0;	    // status of last command executed
	int* pipefd = STDIN_FILENO;  // read end of last pipe

	while (cmdlist) {
		int wp_status;	    // Use for waitpid's status argument!
				    // Read the manual page for waitpid() to
				    // see how to get the command's exit
				    // status (cmd_status) from this value.

		// EXERCISE 4: Fill out this function!
		// If an error occurs in cmd_exec, feel free to abort().

		/* Your code here */
		
		pid_t pid = cmd_exec(cmdlist, pipefd);
		
		
		int wait_stat = waitpid(pid, &wp_status, 0);
		
		
		
		switch(cmdlist->controlop) {
		
		case CMD_END:
		case CMD_SEMICOLON: {
			
			if (WIFEXITED(wp_status)) {
			cmd_status = WEXITSTATUS(wp_status);		
		}
		else {
			cmd_status = 1;
		}
		
		break;
			}
		case CMD_AND: {
		if (WIFEXITED(wp_status)) {
			cmd_status = WEXITSTATUS(wp_status);		
		}
		else {
			cmd_status = 1;
		}
			if ( cmd_status != 0 ) 
				goto done;
		
		break; 
			}
		case CMD_OR: {
		if (WIFEXITED(wp_status)) {
			cmd_status = WEXITSTATUS(wp_status);		
		}
		else {
			cmd_status = 1;
		}
			if ( cmd_status == 0 ) 
				goto done;
		
		
		break; 
			}
		case CMD_BACKGROUND:
		case CMD_PIPE: {
		cmd_status = 0;
		break;
			}
		
		default: goto done;
		
		}
		
		
		

		cmdlist = cmdlist->next;
	}

        while (waitpid(0, 0, WNOHANG) > 0);

done:
	return cmd_status;
}
