#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Handler for SIGNINT
void handle_SIGINT(int signo){
  char* message = "Caught SIGINT, sleeping for 10 seconds\n";
  write(STDOUT_FILENO, message, 39);
  sleep(10);
}



int main(){
  struct sigaction SIGINT_action = {0}, ignore_action = {0};

  // Fill out the SIGINT_action struct
  // Register handle_SIGINT as the signal handler
  SIGINT_action.sa_handler = handle_SIGINT;
  // Block all catchable signals while handle_SIGINT is running
  sigfillset(&SIGINT_action.sa_mask);
  // No flags set
  SIGINT_action.sa_flags = 0;
  sigaction(SIGINT, &SIGINT_action, NULL);

  // The ignore_action struct as SIG_IGN as its signal handler
  ignore_action.sa_handler = SIG_IGN;

  // ignore sigint
  sigaction(SIGINT, &ignore_action, NULL);


  pid_t pid = getpid();
  printf("\nThis PID of this process is %d\n", pid);
  printf("SIGINT are disabled, so will be ignored.\n");


  while(1){
    pause();
  }
  return 0;
}