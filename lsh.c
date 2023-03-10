// lsh simple shell in c
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<termios.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

struct sigaction child_act;
int promptFlag = 1;
int pid;

void welcomeScreen(){
        printf("\n\t============================================\n");
        printf("\t                 C Shell - lsh              \n");
        printf("\t--------------------------------------------\n");
        printf("\t             author: Dawid Lipiak           \n");
        printf("\t============================================\n");
        printf("\n\n");
}

void getDirectory()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));   
    printf("%s",cwd);
}

void child_handler(int signum){
    while(waitpid(-1,NULL, WNOHANG) > 0){}
    printf("\n");
}

void promt() {
    char* username = getenv("USER");
    char hostname[1024];
    gethostname (hostname, sizeof(hostname));
    printf("\033[38;5;50mlsh>\033[38;5;34m%s@%s:", username, hostname); 
    printf("\033[38;5;26m");
    getDirectory();
    printf("\033[38;5;255m# ");
}

void runProgramm(char *args[], int bg){
    if((pid = fork()) == -1 ){
        printf("Could not create child process\n");
        return;
    }
    if(pid == 0){
        if(execvp(args[0],args) == -1 ){
            printf("Command not found\n");
            kill(getpid(), SIGTERM);
        }
    }
    
    if(bg == 0){
        waitpid(pid, NULL, 0);
    }
    else{
        
    }    
}

// Handling standard input commands 
void commendExec(char* args[])
{
    int j = 0;
    int bg = 0;

    while ( args[j] != NULL){
		if (strcmp(args[j],"&") == 0){
			bg = 1;
            args[j] = NULL;
            break;
		}
		j++;
	}

    if (strcmp(args[0],"exit") == 0){
        exit(0);
    }
    else if(strcmp(args[0], "pwd") == 0){
        getDirectory();
        printf("\n");
    }
    else if(strcmp(args[0],"clear") == 0) {
        system("clear");
    }
    else if(strcmp(args[0],"cd") == 0){
        if (args[1] == NULL) {
		    chdir(getenv("HOME"));
	    }
        else if(chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
		}
    }
    else{
        runProgramm(args,bg);
    }
    promptFlag = 1;
}
int main (int argc, char *argv[], char** envp){
    int MAXLINE = 1024;
    char line[MAXLINE];
    char *line_token[256];
    int numTokens;
    promptFlag = 1;

    child_act.sa_handler = child_handler;
    sigaction(SIGCHLD, &child_act, 0);

    welcomeScreen();

    while(1){
        pid = getpid();
        if(promptFlag == 1)
            promt();
        promptFlag = 0;

        //memset ( line, '\0', MAXLINE );

		fgets(line, MAXLINE, stdin);

        // If nothing is written, the loop is executed again
		line_token[0] = strtok(line," \n\t");
        if(line_token[0] == NULL){ 
            promptFlag = 1;
            continue;
        }
		// We read all the tokens of the input and pass it to our
		// commandHandler as the argument
		numTokens = 1;
        while((line_token[numTokens] = strtok(NULL, " \n\t")) != NULL){
            //printf("%s\n",line_token[numTokens] );
            numTokens++;
        }

		commendExec(line_token);
    }

    return 0;
}
