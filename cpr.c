/* -------------------------------------------------------------
File: cpr.c

Last name: Do
Student number: 300163370

Description: This program contains the code for creation
 of a child process and attach a pipe to it.
	 The child will send messages through the pipe
	 which will then be sent to standard output.

Explanation of the zombie process
(point 5 of "To be completed" in the assignment):

	A zombie process is a child process that has completed its execution but its PID 
	is still stored as an entry in the file descriptor table.

	This happens when the parent process never calls the wait() function. As such,
	the parent never removes the child's PID from the file descriptor table.

	Zombies processes do not take up any memory or use CPU resources, but it can clutter up 
	the file descriptor table, which only has limited slots.
	
	Cleaning up zombie processes is very difficult, and usually, the only way to remove them
	is by restarting the system.

------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/select.h>

/* Prototype */
void createChildAndRead(int);

#define BUFFER_SIZE 25
#define READ_END	0
#define WRITE_END	1

/* -------------------------------------------------------------
Function: main
Arguments: 
	int ac	- number of command arguments
	char **av - array of pointers to command arguments
Description:
	Extract the number of processes to be created from the
	Command line. If an error occurs, the process ends.
	Call createChildAndRead to create a child, and read
	the child's data.
-------------------------------------------------------------- */

int main(int ac, char ** av) {
    int processNumber;

    if (ac == 2) {
        if (sscanf(av[1], "%d", & processNumber) == 1) {
            createChildAndRead(processNumber);
        } else fprintf(stderr, "Cannot translate argument\n");
    } else fprintf(stderr, "Invalid arguments\n");
    return (0);
}

/* --------------------------------------------------------------
Function: createChildAndRead
Arguments: 
	int prcNum - the process number
Description:
	Create the child, passing prcNum-1 to it. Use prcNum
	as the identifier of this process. Also, read the
	messages from the reading end of the pipe and sends it to 
	the standard output (df 1). Finish when no data can
	be read from the pipe.
--------------------------------------------------------------- */

void createChildAndRead(int prcNum) {
	
	pid_t pid;
	int fd[2]; // fd[0] = 3 READ, f[1] = 4 WRITE

	if (pipe(fd) == -1) { // create a pipe and verify successful creation
		fprintf(stderr,"Pipe creation failed in process");
		exit(-1);
	}

	fflush(stdout);
	printf("Process %d begins\n", prcNum);
	fflush(stdout);

	pid = fork(); // create child process

	// this is the recursive ./cpr call
	if (prcNum > 1) {

		if (pid == 0) { // child process
			close(fd[READ_END]); // close fd[0] = 3
			/* Currently, the process' write end (1) takes stdin
				we want to replace that write end with the write end of the pipe that we created earlier,
				which is fd[1]
			 */
			dup2(fd[WRITE_END], WRITE_END); // fd[1] = 4 replaces the stdin fd of the current child process

			char nextPrcNum[5]; // buffer to store prcNum as a string- assume prcNum can be up to 5 digits
			sprintf(nextPrcNum, "%d", --prcNum); // convert prcNum to a string by storing it in nextPrcNum
			execlp("./cpr", "./cpr", nextPrcNum, NULL); // this is what is being sent to the write-end of this process' pipe

			fprintf(stderr, "\nERROR: Please compile the program like the example below: \n\n\tgcc -o cpr cpr.c ; ./cpr 4\n\n");
			
			exit(-1); // should not be reachable

		} else { // parent process
			close(fd[WRITE_END]); // close fd[1] = 4
			char buffer[BUFFER_SIZE]; // store data received from read-end of the pipe with data from the child
			
			// read data from the read-end of the pipe until the child closes the write-end
			while (read(fd[READ_END], buffer, BUFFER_SIZE) > 0) {
				printf("%s", buffer);
				fflush(stdout);
				memset(buffer, 0, BUFFER_SIZE); // clear --> char *strncpy(char *dest, const char *src, size_t n)
			}

			// once the parent process finishes printing out text to the screen
			// we want to print the process termination text
			fflush(stdout);
			printf("Process %d ends\n", prcNum);
			fflush(stdout);
		}	
	} else if (prcNum == 1 && pid == 0) { // this is the "base case" of the recursive ./cpr call
		// either the parent or child process can do this task; Here, pid == 0, so the child process will print this
		// if pid == 0 (or pid != 0) is not specified, this part of the code will run twice; once by parent, once by child
		sleep(5);
		printf("Process %d ends\n", prcNum);
		fflush(stdout);
	}

}