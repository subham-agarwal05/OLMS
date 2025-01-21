#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

struct admin{
    char username[100];
    char password[100];
}; 

int main(){
    char username[100];
    char password[100];
    int fd=open("admins.bin",O_RDWR|O_CREAT,0666);
    printf("Enter username: ");
    scanf("%s",username);
    printf("Enter password: ");
    scanf("%s",password);
    struct admin *a=(struct admin*)malloc(sizeof(struct admin));
    strcpy(a->username,username);
    strcpy(a->password,password);
    lseek(fd,0,SEEK_END);
    write(fd,a,sizeof(struct admin)); 
    close(fd);
    return 0;
}