# Multi-Level Piped Linux Shell


Implementation of process_cmd (Read in raw format)
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
Part 4: Parent waits for child created at Part 3 
End of function
