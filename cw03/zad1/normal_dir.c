#define _GNU_SOURCE
#define __USE_XOPEN
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <limits.h>
//#include <linux/limits.h>


void exitFail(char *msg){
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}

void printStats(const char *path, const struct stat *f_stat){
    printf("Absolute path:        %s\n",path);
    printf("File Size:            %li B\n",(long int) f_stat->st_size);
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

void checkTimeComponent (char * arg, int from, int to, char *component){
    int val = atoi(arg);
    if ((val > to) && (val < from)){
        printf("Wrong %s\n",component);
        exit(1);
    }
}

int convert(char *arg){
    int mode;
    if (strcmp(arg,"<") == 0)
        mode = -1;
    if (strcmp(arg,"=") == 0)
        mode = 0;
    if (strcmp(arg,">") == 0)
        mode = 1;
    return mode;
}

void convertTime(char **argv, time_t *time){
    char *string = malloc(19 * sizeof(char));
    strcat(string,argv[3]);
    for (int i=4; i<9; i++){
        strcat(string," ");
        strcat(string,argv[i]);
    }
    struct tm tmp;
    strptime(string, "%Y %m %d %H %M %S", &tmp);
    free(string);
    *time = mktime(&tmp);
}

void dirRush(char *path, int mode, time_t date){
    DIR *dir = opendir(realpath(path,NULL));
    if (path == NULL) return;    

    if (dir == NULL)
        exitFail("Error when opening dir!");
    
    struct stat f_stat;
    struct dirent *rdir = readdir(dir);

    char n_path[PATH_MAX];

    while(rdir != NULL){
        sprintf(n_path,"%s/%s",path,rdir->d_name);
        lstat(n_path,&f_stat);
	if (strcmp(rdir->d_name, ".") == 0 || strcmp(rdir->d_name, "..") == 0){
	    rdir = readdir(dir);
	    continue;
	}
        if(S_ISREG(f_stat.st_mode)){
            if  ((mode == -1 && date > f_stat.st_mtime) ||
            (mode == 0 && date == f_stat.st_mtime) ||
            (mode == 1 && date < f_stat.st_mtime)){
                printStats(n_path,&f_stat);
                char modTime[25];
                strftime(modTime,sizeof(modTime),"%d-%m-%Y %H:%M:%S",localtime(&f_stat.st_mtime));
                printf("\nLast modification:    %s\n\n",modTime);
            }
        }
        else if (S_ISLNK(f_stat.st_mode) || S_ISBLK(f_stat.st_mode)){
            rdir = readdir(dir);
            continue;
        }
        else if (S_ISDIR(f_stat.st_mode)){
            pid_t pid = vfork();
            if(!pid){
                printf("CHILD_PROCESS\n");
                dirRush(n_path,mode,date);
                exit(EXIT_SUCCESS);
            }
        }
        rdir = readdir(dir);
    }
    closedir(dir);
}

/***
 *      __  __              _____   _   _      __ __  
 *     |  \/  |     /\     |_   _| | \ | |    / / \ \ 
 *     | \  / |    /  \      | |   |  \| |   | |   | |
 *     | |\/| |   / /\ \     | |   | . ` |   | |   | |
 *     | |  | |  / ____ \   _| |_  | |\  |   | |   | |
 *     |_|  |_| /_/    \_\ |_____| |_| \_|   | |   | |
 *                                            \_\ /_/                                                    
 */

int main(int argc, char **argv){
    if (argc < 9){
        printf("Too few args\n");
        printf("path (< | = | >)  year month day hour minute second\n");
        return 1;
    }
    int mode;
    time_t date;
    mode = convert(argv[2]);
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
    convertTime(argv,&date);

    dirRush(realpath(path,NULL),mode,date);

    return 0;
}

