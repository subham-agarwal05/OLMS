#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"./login/login.h"
#include"./utilities/utilities.h"

int main(){
    //welcome message
    printf("\n");
    printf("---%*s%*s---\n", 75 + (int)strlen("IIITB ONLINE LIBRARY MANAGEMENT SYSTEM") / 2, "IIITB ONLINE LIBRARY MANAGEMENT SYSTEM", 75 - (int)strlen("IIITB ONLINE LIBRARY MANAGEMENT SYSTEM") / 2, "");
    printf("\n");
    printc("1. Admin\n");
    printc("2. User\n");
    printc("3. Exit\n");

    int choice;
    scanf("%d", &choice);
    if(choice==1){
        printc("Admin selected\n");
        printc("1.Login\n");
        printc("2.Signup\n");
        int admin_choice;
        scanf("%d", &admin_choice);
        if(admin_choice==1){
            printf("Enter Admin username: ");
            char username[100];
            scanf("%s", username);
            printf("Enter Admin password: ");
            char password[100];
            scanf("%s", password);
            int login_result = admin_login(username,password);
            if(login_result == 0){
                printc("Admin login successful\n");
                show_admin_menu();
            }
            else{
                printc("Admin login failed\n");
            }
        }
        else if(admin_choice==2){
            printf("Enter a username: ");
            char username[100];
            scanf("%s", username);
            printf("Enter password: ");
            char password[100];
            scanf("%s", password);
            int signup_result = admin_signup(username,password);
            if(signup_result == 0){
                printc("Admin signup successful\n");
            }
            else{
                printc("Admin signup failed\n");
            }
        }
    }

}