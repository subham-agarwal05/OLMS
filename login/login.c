#include<stdio.h>
#include<string.h>
#include"./login.h"
#include"../utilities/utilities.h"

FILE* adminfd;
int admin_count;

int open_admin(){
    adminfd=fopen(ADMIN_FILE, "rb");
    if(adminfd==NULL){
        adminfd=fopen(ADMIN_FILE, "wb");
        int count=0;
        fwrite(&count, sizeof(int), 1, adminfd);
        fclose(adminfd);
    }
    else{
        fclose(adminfd);
    }

    adminfd=fopen(ADMIN_FILE, "rb+");
    fread(&admin_count, sizeof(int), 1, adminfd);
    return 0;

}

int admin_signup(char* username,char* password){
    if(open_admin()==1){
        return 1;
    }
    struct admin a;
    strcpy(a.username, username);
    strcpy(a.password, password);
    fwrite(&a, sizeof(struct admin), 1, adminfd);
    admin_count++;
    return 0;
}

int admin_login(char* username,char* password){
    if(open_admin()==1){
        return 1;
    }
    struct admin a;
    while(fread(&a, sizeof(struct admin), 1, adminfd)!=0){
        if(strcmp(a.username, username)==0 ){
            if(strcmp(a.password, password)==0){
                return 0;
            }
            else{
                printc("Incorrect password\n");
                return 1;
            }
        }
        else{
            printc("Admin not found\n");
            return 1;
        }
    }

}

void show_admin_menu(){
    printf("\n");
    printc("1. Add Book\n");
    printc("2. Remove Book\n");
    printc("3. View Books\n");
}