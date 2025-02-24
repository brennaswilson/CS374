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
 #include <fcntl.h>
 
 #define INPUT_LENGTH 	2048
 #define MAX_ARGS		 512
 
 int exit_status = 0;
 int signal_terminated = 0;
 int position = 0;

 const char *built_in[] = {"cd", "status", "exit", NULL};
 int is_builtin = 0;
 int children_background[100] = {0};

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
    
     if(input == NULL || input[0] == '#' || input[0] == '\n'){
        free(curr_command);
        return NULL;
     }

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

 int non_built_in(struct command_line *curr_command){

    pid_t spawnpid = fork();
    int childStatus;


    //foreground
    if(curr_command->is_bg != true){
        if (spawnpid == 0) {

            //foreground
            if(curr_command->input_file != NULL){

                //open file for read only
                int sourceFD = open(curr_command->input_file, O_RDONLY);
                if (sourceFD == -1) { 
                    perror(curr_command->input_file); 
                    fflush(stdout);
                    exit_status = 1;
                    exit(1);
                }
        
                // Redirect stdin to source file
                int result = dup2(sourceFD, 0);
                if (result == -1) { 
                    perror(curr_command->input_file); 
                    exit_status = 1;
                    close(sourceFD);
                    exit(1);
                }
        
                close(sourceFD);
        
            }

            if (curr_command->output_file != NULL){
                int targetFD = open(curr_command->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (targetFD == -1) { 
                    perror(curr_command->output_file); 
                    fflush(stdout);
                    exit_status = 1;
                    exit(1);
                }
                int result = dup2(targetFD, 1);
                if (result == -1) { 
                    perror(curr_command->output_file); 
                    exit_status = 1;
                    close(targetFD);
                    exit(1);
                }
                close(targetFD);
            }
        }


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
    
    }

    //background

    if(curr_command->is_bg == true){

        if (spawnpid == 0) {

            /////if source files not given//////

            if(curr_command->input_file == NULL){
                int sourceFD = open("/dev/null", O_RDONLY);
                if (sourceFD == -1) { 
                    perror("/dev/null"); 
                    fflush(stdout);
                    exit_status = 1;
                    exit(1);
                }
                int result = dup2(sourceFD, 0);
                if (result == -1) { 
                    perror("/dev/null"); 
                    exit_status = 1;
                    close(sourceFD);
                    exit(1);
                }
                close(sourceFD);
            }

            if(curr_command->output_file == NULL){
                int targetFD = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (targetFD == -1) { 
                    perror("/dev/null"); 
                    fflush(stdout);
                    exit_status = 1;
                    exit(1);
                }
                int result = dup2(targetFD, 1);
                if (result == -1) { 
                    perror("/dev/null"); 
                    exit_status = 1;
                    close(targetFD);
                    exit(1);
                }
                close(targetFD);
            }

            ////source files given///////
            if(curr_command->input_file != NULL){

                //open file for read only
                int sourceFD = open(curr_command->input_file, O_RDONLY);
                if (sourceFD == -1) { 
                    perror(curr_command->input_file); 
                    fflush(stdout);
                    exit_status = 1;
                    exit(1);
                }
        
                // Redirect stdin to source file
                int result = dup2(sourceFD, 0);
                if (result == -1) { 
                    perror(curr_command->input_file); 
                    exit_status = 1;
                    close(sourceFD);
                    exit(1);
                }
            }
            if (curr_command->output_file != NULL){
                int targetFD = open(curr_command->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (targetFD == -1) { 
                    perror(curr_command->output_file); 
                    fflush(stdout);
                    exit_status = 1;
                    exit(1);
                }
                int result = dup2(targetFD, 1);
                if (result == -1) { 
                    perror(curr_command->output_file); 
                    exit_status = 1;
                    close(targetFD);
                    exit(1);
                }
                close(targetFD);
            }
        
        }

        else {
            printf("background pid is %d\n", spawnpid);
            fflush(stdout);
            children_background[position] = spawnpid;
            position += 1;
            
        }

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
            return childStatus;
        }

    }
}
 
 int builtin_check(struct command_line *curr_command){
    for (int arg = 0; built_in[arg] != NULL; arg ++){
        if(strcmp(curr_command->argv[0], built_in[arg]) == 0){
            is_builtin = 1;
        }
    }
    return is_builtin;
 }

 void check_background(int children_background[]){
    int childStatus;
    for(int i = 0; i < position; i++){
        int childPid = children_background[i];
        int result = waitpid(children_background[i], &childStatus, WNOHANG);

        if(result > 0 ){
            if(WIFEXITED(childStatus)){
                printf("background pid %d is done: exit value %d\n", childPid, WEXITSTATUS(childStatus));

            }
            else if (WIFSIGNALED(childStatus)){
                printf("background pid %d is done: terminated by signal %d\n", childPid, WTERMSIG(childStatus));
            }

            for(int x = i; x < (position -1); x ++){
                children_background[x] = children_background[x+1];
            }
            position --;
            i--;


        }
    }
 }


 int main(int argc, char* argv[])
 {
     struct command_line *curr_command;

     int old_stdin = dup(STDIN_FILENO);
     int old_stdout = dup(STDOUT_FILENO);

     while(true)
     {
        
        check_background(children_background);
    

        curr_command = parse_input();
        
        if(curr_command != NULL){
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

        else{
            continue;
        }
        dup2(old_stdin, STDIN_FILENO);
        dup2(old_stdout, STDIN_FILENO);

     }
     return EXIT_SUCCESS;
 }