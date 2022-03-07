/*
    Implementation of process_cmd

    Part 1: Check if cmdline is exit, if it is print The shell program terminates and exit(0)
    Part 2: Create 2 variables and use tokenize function to split cmdline into pipe_segments
    Part 3: Create a child process (everything is a child is listed as 3.x)
        Part 3.1: Create variables
        Part 3.2: Use a forloop to make 2d pipe of size num_pipe_segments         
        Part 3.3: Create a forloop that runs num_pipe_segments-1 times (so if there is only one pipe segment, this for loop will not run)
                  (this forloop contains the to part 3.6) (starts from i = 1 to i<numpipesegments)
        Part 3.4: Use a foorloop to change all blank characters in pipe_segments into space character so tokenize will work
        Part 3.5: Use tokenize function to split pipe_segments into cmd_segments
            Part 3.5a: If it is the first pipe segment, create a child
                Part 3.5a.1: Close stdout, make pfds[i] as std output, close pipe output, then run execvp with the correct cmd_segments
                Part 3.5a.2: Parent waits for child created from part 3.5a
            Part 3.5b:  If it is not the first pipe segment, close stdin, make pfds[i-1] std input, close pipeinput then create a child
                Part 3.5b.1: Close stdout, make pfds[i] as std output, close pipe output, then run execvp with the correct cmd_segments
                Part 3.5b.2: Parent waits for child created at part 3.5b
        Part 3.6: Back to start of forloop
        Part 3.7: (3.7 is the final run of the command) Close stdout, make pfds[i] as std output, close pipe output, then run execvp with the correct cmd_segments
        Part 3.8: Use a foorloop to change all blank characters in pipe_segments into space character so tokenize will work
        Part 3.9: Use tokenize function to split pipe_segments into cmd_segments
        Part 3.10: Run execvp with the correct cmd_segments
    Part 4: Parent waits for child created at Part 3 *end of function*
    

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

 // Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 16 pipe segments
#define MAX_PIPE_SEGMENTS 16

// Assume that each segment has at most 256 characters (including NULL)
#define MAX_SEGMENT_LENGTH 256

// This function will be invoked by main()
// TODO: Implement the multi-level pipes below
void process_cmd(char* cmdline);

// This function will be invoked by main()
// TODO: Modify the show_prompt function  
void show_prompt();


// This function will be invoked by main()
// The implementation is given
int get_cmd_line(char* cmdline);

// tokenize function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// char cmdline[MAX_CMDLINE_LEN]; // The input command line
//
// Sample usage of this tokenize function:
//
//  tokenize(pipe_segments, cmdline, &num_pipe_segments, "|");
// 
void tokenize(char** argv, char* line, int* numTokens, char* token);


/* The main function implementation */
int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    printf("The shell program (pid=%d) starts\n", getpid());
    while (1)
    {
        show_prompt();
        if (get_cmd_line(cmdline) == -1)
            continue; /* empty line handling */

        process_cmd(cmdline);
    }
    return 0;
}


void process_cmd(char* cmdline)
{
    // TODO: write down your code for process_cmd
    //printf("Debug: %s\n", cmdline);
    if (strcmp(cmdline, "exit") == 0) {
        int z = getpid();
        printf("The shell program (pid=%d) terminates\n", z);
        exit(0);
    }
    
    char* pipe_segments[MAX_PIPE_SEGMENTS];
    int num_pipe_segments = 0;
    tokenize(pipe_segments, cmdline, &num_pipe_segments, "|");

    if (fork() == 0) {
        int i = 1;
        int j = 0;
        int pfds[num_pipe_segments][2];
        for (j = 0; j <= num_pipe_segments; j++) {
            pipe(pfds[j]);
        }
        char* cmd_segments[4];
        int num_cmd_segments = 0;

        for (i = 1; i < num_pipe_segments; i++) {

            for (j = 0; j <= strlen(pipe_segments[i - 1]); j++) {
                if (pipe_segments[i - 1][j] == '\t' || pipe_segments[i - 1][j] == '\r' || pipe_segments[i - 1][j] == '\n' || pipe_segments[i - 1][j] == '\v' || pipe_segments[i - 1][j] == '\f') {
                    pipe_segments[i - 1][j] = ' ';
                }
            }            
            tokenize(cmd_segments, pipe_segments[i-1], &num_cmd_segments, " ");

            //printf("Integer value is %d\n", i); 
            //printf("Processing command segment (in loop) %s\n", cmd_segments[0]); 

            if (i == 1) {
                if (fork() == 0) {
                    close(1);
                    dup2(pfds[i][1], 1);
                    close(pfds[i][0]);

                    execvp(cmd_segments[0], cmd_segments);
                    exit(0);
                }
                else {
                    wait(0);
                }
                
            }
            else {
                close(0);
                dup2(pfds[i - 1][0], 0);
                close(pfds[i - 1][1]);
                if (fork() == 0) {                  
                    close(1);
                    dup2(pfds[i][1], 1);
                    close(pfds[i][0]);

                    execvp(cmd_segments[0], cmd_segments);
                    exit(0);
                }
                else {
                    wait(0);
                }
            }
        }
        
        close(0);
        dup2(pfds[i - 1][0], 0);
        close(pfds[i - 1][1]);
        for (j = 0; j <= strlen(pipe_segments[i - 1]); j++) {
            if (pipe_segments[i - 1][j] == '\t' || pipe_segments[i - 1][j] == '\r' || pipe_segments[i - 1][j] == '\n' || pipe_segments[i - 1][j] == '\v' || pipe_segments[i - 1][j] == '\f') {
                pipe_segments[i - 1][j] = ' ';
            }
        }
        tokenize(cmd_segments, pipe_segments[i-1], &num_cmd_segments, " ");
        //printf("Processing command segment (outside of for loop) %s\n", cmd_segments[0]);

        execvp(cmd_segments[0], cmd_segments);
        exit(0);

    }
    else {
        wait(0);
    }

}
    


void show_prompt()
{
    // TODO: update the shell prompt using your ITSC email (without @connect.ust.hk)
    printf("$hlatong> ");
}

int get_cmd_line(char* cmdline)
{
    int i;
    int n;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ') {
        ++i;
    }
    if (i == n) {
        // Empty command
        return -1;
    }
    return 0;
}

// Implementation of tokenize function
void tokenize(char** argv, char* line, int* numTokens, char* delimiter)
{
    int argc = 0;
    char* token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    argv[argc++] = NULL;
    *numTokens = argc - 1;
}