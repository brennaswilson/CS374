/**
 * A sample program for parsing a command line. If you find it useful,
 * feel free to adapt this code for Assignment 4.
 * Do fix memory leaks and any additional issues you find.
 */

 #include <stdio.h>
 #include <stdbool.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 
 #define INPUT_LENGTH 	2048
 #define MAX_ARGS		 512
 
 int exit_status = 0;
 int signal_terminated = 0;

 struct command_line
 {
     char *argv[MAX_ARGS + 1];
     int argc;
     char *input_file;
     char *output_file;
     bool is_bg;
 };
 
 
 struct command_line *parse_input()
 {
     char input[INPUT_LENGTH];
     struct command_line *curr_command = (struct command_line *) calloc(1, sizeof(struct command_line));
 
     // Get input
     printf(": ");
     fflush(stdout);
     fgets(input, INPUT_LENGTH, stdin);
 
     // Tokenize the input
     char *token = strtok(input, " \n");

     while(token){
         if(!strcmp(token,"<")){
             curr_command->input_file = strdup(strtok(NULL," \n"));
         } else if(!strcmp(token,">")){
             curr_command->output_file = strdup(strtok(NULL," \n"));
         } else if(!strcmp(token,"&")){
             curr_command->is_bg = true;
         } else{
             curr_command->argv[curr_command->argc++] = strdup(token);
         }
         token=strtok(NULL," \n");
     }

     return curr_command;
 }

 void non_built_in(struct command_line *curr_command){

    pid_t spawnpid = fork();
    int childStatus;
    int childPid;

    //foreground

    //child process
    if(spawnpid == 0){
        
        if (execvp(curr_command->argv[0], curr_command->argv) == -1){
            char error_message = ("%d: no such file or directory\n", curr_command->argv[0]);
            perror(error_message);
            exit_status = EXIT_FAILURE;

        };

    }

    //background
 }
 
 int main(int argc, char* argv[])
 {
     struct command_line *curr_command;
 
     while(true)
     {
        curr_command = parse_input();
        
        if(curr_command && (strcmp(curr_command->argv[0], "exit") == 0 || strcmp(curr_command->argv[0], "&exit") == 0)){
            free(curr_command);
            printf("\n");
            exit(EXIT_SUCCESS);
         }
        

        if (curr_command && (strcmp(curr_command->argv[0], "cd") == 0 || strcmp(curr_command->argv[0], "&cd") == 0)){
            
            if(curr_command -> argc < 2){
                chdir(getenv("HOME"));
            }
            else{
                chdir(curr_command->argv[1]);
            }

        }

        if (curr_command && (strcmp(curr_command->argv[0], "status") == 0 || strcmp(curr_command->argv[0], "&status") == 0)){
            if (signal_terminated == 0){
                printf("exit value %d\n", exit_status);
                fflush(stdout);
            }
            else{
                printf("terminated by signal %d\n", exit_status);
                fflush(stdout);
            }
        }

        else{
            non_built_in(curr_command);
        }


     }
     return EXIT_SUCCESS;
 }