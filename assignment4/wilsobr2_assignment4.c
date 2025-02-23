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

<<<<<<< HEAD
 const char *built_in[] = {"cd", "status", "exit", NULL};
 int is_builtin = 0;

=======
>>>>>>> refs/remotes/origin/main
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

<<<<<<< HEAD
 int non_built_in(struct command_line *curr_command){
=======
 void non_built_in(struct command_line *curr_command){
>>>>>>> refs/remotes/origin/main

    pid_t spawnpid = fork();
    int childStatus;
    int childPid;

    //foreground

    //child process

    switch (spawnpid)
    {
    case -1:
        perror("fork()\n");
        exit(1);
        break;
    case 0:
        // The child process executes this branch
        if (execvp(curr_command->argv[0], curr_command->argv) == -1){
            // exec only returns if there is an error
            perror(curr_command->argv[0]);
            exit(1);
        }
        break;
    default:
        // The parent process executes this branch
        // Wait for child's termination
        spawnpid = waitpid(spawnpid, &childStatus, 0);
        return childStatus;
    }
    


    //background
 }
 
 int builtin_check(struct command_line *curr_command){
    for (int arg = 0; built_in[arg] != NULL; arg ++){
        if(strcmp(curr_command->argv[0], built_in[arg]) == 0){
            is_builtin = 1;
        }
    }
    return is_builtin;
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
            if( exit_status != 0){
                exit_status = 1;
            }
            
            if (signal_terminated == 0){
                printf("exit value %d\n", exit_status);
                fflush(stdout);
            }
            else{
                printf("terminated by signal %d\n", exit_status);
                fflush(stdout);
            }
        }
        
        is_builtin = builtin_check(curr_command);

        if(is_builtin == 0){
            exit_status = non_built_in(curr_command);
            
        }
        is_builtin = 0;


     }
     return EXIT_SUCCESS;
 }