# OS-Processes
This program contains the code for creation of a child process and attach a pipe to it. The child will send messages through the pipe which will then be sent to standard output.

## Instructions to execute
How to execute the program in a UNIX terminal.
```
gcc -o cpr cpr.c ; ./cpr <INSERT_POSITIVE_INTEGER>
```
For example,
```
MY_LAPTOP# gcc -o cpr cpr.c ; ./cpr 5
Process 5 begins
Process 4 begins
Process 3 begins
Process 2 begins
Process 1 begins
Process 1 ends
Process 2 ends
Process 3 ends
Process 4 ends
Process 5 ends
```
