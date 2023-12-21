#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : (S_ISREG(mode)) ? "-" : "?");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char* argv[]) {
    struct stat file_stat;
    struct passwd* pwd;
    struct group* grp;
    struct tm* time_info;

    if (argc < 2) {
        printf("File name not entered\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (lstat(argv[i], &file_stat) == -1) {
            perror("Error opening file");
            continue;
        }

        print_permissions(file_stat.st_mode);

        printf(" %2ld ", (long)file_stat.st_nlink);

        if ((pwd = getpwuid(file_stat.st_uid)) != NULL) {
            printf("%8s ", pwd->pw_name);
        }
        else {
            printf(" %-8d ", file_stat.st_uid);
        }

        if ((grp = getgrgid(file_stat.st_gid)) != NULL) {
            printf("%8s ", grp->gr_name);
        }
        else {
            printf(" %-8d ", file_stat.st_gid);
        }

        if (S_ISREG(file_stat.st_mode)) {
            printf("%7ld ", (long)file_stat.st_size);
        }
        else {
            printf("  - ");
        }

        time_info = localtime(&file_stat.st_mtime);
        printf("%2d-%2d %02d:%02d ", time_info->tm_mon + 1, time_info->tm_mday, time_info->tm_hour, time_info->tm_min);

        printf("%s\n", argv[i]);
    }

    return 0;
}