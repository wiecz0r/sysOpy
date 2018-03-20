#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ftw.h>
#include <limits.h>

#define _XOPEN_SOURCE 500

int mode;
time_t date;

void exitFail(char *msg){
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}
/*
char *absolutePath(const char *path){
    char aPath[PATH_MAX];
    realpath(path,aPath);
    return aPath;
}
*/

void printStats(const char *path, const struct stat *f_stat){
    printf("Absolute path:        %s\n",path);
    printf("File Size:            %lli B\n",f_stat->st_size);
    printf("File permissions:     ");
    printf( (f_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf( (f_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf( (f_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf( (f_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf( (f_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf( (f_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf( (f_stat->st_mode & S_IROTH) ? "r" : "-");
    printf( (f_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf( (f_stat->st_mode & S_IXOTH) ? "x" : "-");
}

int fn(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
    if (tflag == FTW_F){
        if  ((mode == -1 && date > sb->st_mtime) ||
        (mode == 0 && date == sb->st_mtime) ||
        (mode == 1 && date < sb->st_mtime)){
            printStats(fpath,sb);
            char modTime[25];
            strftime(modTime,sizeof(modTime),"%d-%m-%Y %H:%M:%S",localtime(&sb->st_mtime));
            printf("\nLast modification:    %s\n\n",modTime);
        }
    }
    return 0;
}

void checkTimeComponent (char * arg, int from, int to, char *component){
    int val = atoi(arg);
    if ((val > to) && (val < from)){
        printf("Wrong %s\n",component);
        exit(1);
    }
}

void convert(char *arg){
    
    if (strcmp(arg,"<") == 0)
        mode = -1;
    if (strcmp(arg,"=") == 0)
        mode = 0;
    if (strcmp(arg,">") == 0)
        mode = 1; 
}

void convertTime(char **argv){
    char *string = malloc(19 * sizeof(char));
    strcat(string,argv[3]);
    for (int i=4; i<9; i++){
        strcat(string," ");
        strcat(string,argv[i]);
    }
    struct tm tmp;
    strptime(string, "%Y %m %d %H %M %S", &tmp);
    date = mktime(&tmp);
    free(string);
}

int main(int argc, char **argv){
    if (argc < 9){
        printf("Too few args\n");
        printf("path (< | = | >)  year month day hour minute second\n");
        return 1;
    }

    convert(argv[2]);

    checkTimeComponent(argv[3],1990,2018,"year");
    checkTimeComponent(argv[4],1,12,"month");
    checkTimeComponent(argv[5],1,31,"day");
    checkTimeComponent(argv[6],0,23,"hour");
    checkTimeComponent(argv[7],0,59,"minute");
    checkTimeComponent(argv[8],0,59,"seconds");

    char *path = argv[1];
    if (argv[1][0] != '/'){
        char cwd[PATH_MAX];
        if(!getcwd(cwd,sizeof(cwd)))
            exitFail("Problem with CWD");
        strcat(cwd,"/");
        strcat(cwd,argv[1]);
        path = cwd;
    }
    convertTime(argv);
    printf("%i",mode);
    nftw(path,fn,20,FTW_PHYS);
    return 0;
}

