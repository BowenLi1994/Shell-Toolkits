//
//  myshell.c
/*
 Author: Bowen Li
 Project Name:A Simplified Unix Shell Program
 FSUID:bl17j
 */
//

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <fcntl.h>
#include<signal.h>
#define INPUT_SIZE 128
#define PATH_SIZE 255

void loop(void);//the loop function
char *read_input(void);// read in the input char by char
char *command_judge(char *line);//jugde the kind of the command you input
char** parse(char * input,char *delim);//split the "input" by "delim"
//run the built-in and external commands
void shell_command(char * input);
void shell_external(char *comd);
//run the background commands
void shell_background(char *comd);
//run the input/output redirection commands
void shell_redirect(char *comd);
//run the pipe commands
void shell_pipe(char * line);
//count the parameter number of the redirect
int redirect_cout(char *line);
void waitchildren(int signo);

/********************Main function********************************/

int main() {
    loop();
    return 0;
}
/*******************Loop Body of the Shell***************************/
void loop(){
    char *input;
    do{
        printf("Bowen's shell$ ");
        input=read_input();
        char *type = malloc(sizeof(char) *INPUT_SIZE);
        type=command_judge(input);
        if(strcmp(type,"command")==0){
            shell_command(input);
        }
        else if(strcmp(type,"pipe")==0){
            shell_pipe(input);
        }
        else if(strcmp(type,"redirect")==0){
            shell_redirect(input);
        }
        else if(strcmp(type,"background")==0){
            shell_background(input);
        }
    }while(1);
}
/*****************Read in the input*********************************/
char *read_input(void)
{
    int position = 0;
    char *input = malloc(sizeof(char) * INPUT_SIZE);
    int temp;
    
    if (!input) {
        fprintf(stderr, "Bowen's shell: Error in memory allocation!\n");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        // Read in the char
        temp = getchar();
        //Exit when input EOF(control+D)
        if(temp==EOF){
            
            printf("\nBowen's shell is terminated! by EOF\n");
            exit(EXIT_FAILURE);
            
        }
        //End input when have a return
        else if(temp=='\n'){
            input[position]='\0';
            return input;
        }else{
            input[position]=temp;
            
        }
        position++;
        //Error in more than 80 characters
        if(position>=80){
            fprintf(stderr, "Bowen's shell: Max size of your input line is 80!\n");
            exit(EXIT_FAILURE);
        }
    }
}
/*****************Judge the type of the command you input**********************************/
char *command_judge(char *line){
    int i=0;
    while(line[i]!='\0'){
        i++;
        if(line[i]=='|')
            return "pipe";
        if(line[i]=='<'||line[i]=='>')
            return "redirect";
        if(line[i]=='&')
            return "background";
    }
    return "command";
}
/*******************************Split the input command*************************************************/
char** parse(char * input,char * delim){
    char** argv=malloc(INPUT_SIZE * sizeof(char*));
    int argc=0;
    char* temp = strtok(input,delim);
    while(temp){
        argv[argc]=temp;
        argc++;
        temp = strtok(NULL,delim);
    }
    argv[argc]=NULL;
    return argv;
}

/**********************************Built-in-->>exit*************************************************/
void shell_exit(){
    fprintf(stderr, "Bowen's shell: The shell is teminated!\n");
    exit(EXIT_SUCCESS);
}
/*********************************Build-in-->pwd*************************************************/
void shell_pwd(){
    char path[PATH_SIZE];
    getcwd(path,sizeof(path));
    printf("%s\n",path);
}
/******************************Build-in--->cd****************************************************/
void shell_cd(char *path){
    if(chdir(path)!=0)
        perror("Bowen's shell");
}
/****************************Built-in--->set*****************************************************/
void shell_set(char *comd,char *path){
    if(strcmp(comd, "MYPATH")==0)
        setenv("MYPATH", path, 1);
    else
        printf("You can only set MYPATH!\n");
    char *path1 = getenv("PATH");
    char *mypath = getenv("MYPATH");
    char buf[10000];
    sprintf(buf, "%s:%s%c", path1,mypath,'\0');
    setenv("PATH", buf, 1);
}
/***************************Eexcute external commands*****************************************************/
void shell_external(char *comd){
    char comd_temp[INPUT_SIZE];
    strcpy(comd_temp, comd);
    char **ex_command=parse(comd_temp," ");
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(ex_command[0], ex_command) == -1) {
            perror("Bowen's shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("Bowen's shell");
    } else {
        // Parent process
        waitpid(pid, 0, 0);
    }
    
}
/************************Judge the type of commands**********************************************************/
void shell_command(char * input){
    char temp[INPUT_SIZE];
    strcpy(temp,input);
    char **command=parse(temp," ");
    if(strcmp(command[0],"exit")==0)
        shell_exit();
    else if(strcmp(command[0],"pwd")==0)
        shell_pwd();
    else if(strcmp(command[0],"cd")==0)
        shell_cd(command[1]);
    else if(strcmp(command[0],"set")==0){
        char **command_set=parse(command[1],"=");
        shell_set(command_set[0], command_set[1]);
        
    }
    else{
        shell_external(input);
    }
}
/***********Background commands*****************/
void shell_background(char *comd){
    char **line=parse(comd, "&");
    char **command=parse(line[0]," ");
    signal(SIGCHLD, waitchildren);
    pid_t pid;
    pid=fork();
    if (pid == 0) {
        setpgid(0,0);
        if (execvp(command[0], command) == -1) exit(-1);
        exit(1);
    }
}
/************IO redirect*********************************/
void shell_redirect(char *comd){
    int counter=0;
    counter=redirect_cout(comd);
    if(counter==1){
        char **line=parse(comd, ">");
        char **command1=parse(line[0]," ");
        char **command2=parse(line[1]," ");
        int pid = fork();
        
        if (pid == -1) {
            perror("fork");
        } else if (pid == 0) {
            int fd1 = creat(command2[0], 0644);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
            execvp(command1[0], command1);  
            fprintf(stderr, "Failed to exec %s\n", command1[0]);
            
        } else {
            waitpid(pid, 0, 0);
            free(command1);
            free(command2);
        }
    }
    else if(counter==2){
        char **line=parse(comd, "<");
        char **command1=parse(line[0]," ");
        char **command2=parse(line[1]," ");
        int pid = fork();
        
        if (pid == -1) {
            perror("fork");
        } else if (pid == 0) {
            int fd0 = open(command2[0], O_RDONLY);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
            execvp(command1[0], command1);
            fprintf(stderr, "Failed to exec %s\n", command1[0]);
            
        } else {
            waitpid(pid, 0, 0);
            free(command1);
            free(command2);
        }
    }
    else if(counter==3){
        char **line=parse(comd, "<>");
        char **command1=parse(line[0]," ");
        char **command2=parse(line[1]," ");
        char **command3=parse(line[2]," ");
        int pid1 = fork();
        if (pid1 == -1) {
            perror("fork");
        } else if (pid1 == 0) {
            int fd0 = open(command2[0], O_RDONLY);
            int fd1 = creat(command3[0], 0644);
            dup2(fd0, STDIN_FILENO);
            dup2(fd1, STDOUT_FILENO);
            close(fd0);
            close(fd1);
            execvp(command1[0], command1);
            fprintf(stderr, "Failed to exec %s\n", command1[0]);
            
        } else {
            waitpid(pid1, 0, 0);
            free(command1);
            free(command2);
            free(command3);
        }

    }
}
/************************************************/
int redirect_cout(char *line){
    int i=0;
    int flag1=0;
    int flag2=0;
    while(line[i]!='\0'){
        if(line[i]=='>')
            flag1++;
        if(line[i]=='<')
            flag2++;
        i++;
    }
    
    if(flag1==1&&flag2==0)
        return 1;
    else if(flag1==0&&flag2==1)
        return 2;
    else if(flag1==1&&flag2==1) return 3;
    else return 4;
}
/************The pipe commanmds*****************************/
void shell_pipe(char * line){
    char **pipe_command;
    pipe_command=parse(line,"|");
    int no_para=0;
    while(pipe_command[no_para]!=NULL){
        no_para++;
    }
    if (no_para==2){
        int child[2];
        int fds[2];
        char **command1=parse(pipe_command[0]," ");
        char **command2=parse(pipe_command[1]," ");
        pipe(fds);
        if (fork()== 0) {
            close(1); dup(fds[1]);  /* redirect standard output to fds[0] */
            close(fds[1]);close(fds[0]);
            execvp(command1[0],command1);
            exit(0);
        }
        if (fork() == 0) {
            close(0); dup(fds[0]); /* redirect standard input to fds[1] */
            close(fds[0]);close(fds[1]);
            execvp(command2[0],command2);
            exit(0);
        }
        close(fds[0]); close(fds[1]);
        free(command1);
        free(command2);
        int stat;
        free(pipe_command);
        wait(&stat);
        wait(&stat);
    }
    if (no_para==3){
        int child[3];
        int fds[2];
        char **command1=parse(pipe_command[0]," ");
        char **command2=parse(pipe_command[1]," ");
        char **command3=parse(pipe_command[2]," ");
        pipe(fds);
        if (fork()== 0) {
            close(1); dup(fds[1]);  /* redirect standard output to fds[0] */
            close(fds[1]);close(fds[0]);
            execvp(command1[0],command1);
            exit(0);
        }
        if (fork() == 0) {
            close(1); dup(fds[1]);  /* redirect standard output to fds[0] */
            close(0); dup(fds[0]); /* redirect standard input to fds[1] */
            close(fds[0]);close(fds[1]);
            execvp(command2[0],command2);
            exit(0);
        }
        if (fork() == 0) {
            close(0); dup(fds[0]); /* redirect standard input to fds[1] */
            close(fds[0]);close(fds[1]);
            execvp(command3[0],command3);
            exit(0);
        }
        close(fds[0]); close(fds[1]);
        int stat;
        free(command1);
        free(command2);
        free(command3);
        free(pipe_command);
        wait(&stat);
        wait(&stat);
        wait(&stat);
        
    }
}
/************Kill the child process*****************************/
void waitchildren(int signo)    // kill the zombie process in background
{
    int status;
    while(waitpid(-1, &status, WNOHANG) > 0);
}

