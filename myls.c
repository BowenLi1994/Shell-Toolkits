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


int main(int argc, char *argv[])
{
    DIR *d;
    struct dirent *w;
    struct stat buf;
    char temp[255]="";

    if(argc>2){
        printf("Wrong Input!\n");
        exit(1);
    }
    else if(argc==1){
        strcpy(temp,".");
    }
    else
        strcpy(temp,argv[1]);

    d = opendir(temp);
    
    
    
    
    while((w = readdir(d) )!= NULL){
        if(w->d_name[0]!='.'){
        char path[1024]="";
        strcat(path,temp);
        strcat(path, "/");
        strcat(path, w->d_name);
        if (lstat(path, &buf) <0) {
                printf("stat %s failed (probably file does not exist).\n", argv[1]);
                exit(0);
                }
        if (S_ISREG(buf.st_mode)) printf("-");
        else if (S_ISDIR(buf.st_mode)) printf("d");
        else if (S_ISCHR(buf.st_mode)) printf("c");
        else if (S_ISBLK(buf.st_mode)) printf("b");
        else if (S_ISFIFO(buf.st_mode)) printf("f");
        else if (S_ISLNK(buf.st_mode)) printf("l");
        else if (S_ISSOCK(buf.st_mode)) printf("s");
        else printf("u");
        if (S_IRUSR & buf.st_mode) printf("r");
        else printf("-");
        if (S_IWUSR & buf.st_mode) printf("w");
        else printf("-");
        if (S_IXUSR & buf.st_mode) printf("x");
        else printf("-");
        if (S_IRGRP & buf.st_mode) printf("r");
        else printf("-");
        if (S_IWGRP & buf.st_mode) printf("w");
        else printf("-");
        if (S_IXGRP & buf.st_mode) printf("x");
        else printf("-");
        if (S_IROTH & buf.st_mode) printf("r");
        else printf("-");
        if (S_IWOTH & buf.st_mode) printf("w");
        else printf("-");
        if (S_IXOTH & buf.st_mode) printf("x");
        else printf("-");
        printf("%2d ", (int)buf.st_nlink);    //print the link number
        printf("%4s ", getpwuid(buf.st_uid)->pw_name);
        printf("%4s ", getgrgid(buf.st_gid)->gr_name);
        printf(" %6d", (int)buf.st_size);        //print the file size
        char buf_time[32];
        strcpy(buf_time, ctime(&buf.st_mtime));
        buf_time[strlen(buf_time) - 1] = '\0';
        printf(" %.12s", 4+buf_time);        //print the time
        printf(" %s\n", w->d_name);        //print the name
        printf("\n");
        }
        
    }

    return 0;
}

