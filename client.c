#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "client.h"

int ret;
char username[100];
char password[100];

void print_admin_menu()
{
    printf("1. Add book\n");
    printf("2. Remove book\n");
    printf("3. View all books\n");
    printf("4. Search book\n");
    printf("5. Show member details\n");
    printf("6. Remove a member\n");
    printf("7. View all members\n");
    printf("8. Exit\n");

}

void add_book(int sockfd){
    int book_id;
    char name[100];
    char author[100];
    memset(name,0,100);
    memset(author,0,100);
    printf("Enter book id: ");
    scanf("%d",&book_id);
    write(sockfd,&book_id,sizeof(int));
    getchar();
    printf("Enter name of the book: ");
    fgets(name,100,stdin);
    write(sockfd,name,strlen(name));
    printf("Enter author of the book: ");
    fgets(author,100,stdin);
    write(sockfd,author,strlen(author));
    char response[100];
    ret=read(sockfd,response,100);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in adding book\n");
        exit(1);
    }
    printf("%s\n",response);
}

void remove_book(int sockfd){
    int book_id;
    printf("Enter book id to remove: ");
    scanf("%d",&book_id);
    getchar();
    write(sockfd,&book_id,sizeof(int));
    char response[100];
    ret=read(sockfd,response,100);
    response[ret]='\0';
    printf("%s\n",response);
}

void view_books(int sockfd){
    char response[500];
    ret=read(sockfd,response,500);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in viewing books\n");
        exit(1);
    }
    printf("List of all books:\n ");
    while(strcmp(response,"End")!=0){
        printf("%s\n",response);
        ret=read(sockfd,response,500);
        response[ret]='\0';
    }
}

void search_book(int sockfd){
    int book_id;
    printf("Enter book id to search: ");
    scanf("%d",&book_id);
    getchar();
    write(sockfd,&book_id,sizeof(int));
    char response[500];
    ret=read(sockfd,response,500);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in searching book\n");
        exit(1);
    }
    printf("Book details:\n");
    printf("%s\n",response);
}

void search_member(int sockfd){
    char username[100];
    printf("Enter username to search: ");
    scanf("%s",username);
    getchar();
    write(sockfd,username,strlen(username));
    char response[2000];
    ret=read(sockfd,response,2000);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in searching member\n");
        exit(1);
    }
    if(strcmp(response,"Member not found")==0){
        printf("Member not found\n");
        return;
    }
    printf("Member details:\n");
    while(strcmp(response,"End")!=0){
        printf("%s\n",response);
        ret=read(sockfd,response,2000);
        response[ret]='\0';
    }

}

void view_all_members(int sockfd){
    char response[500];
    ret=read(sockfd,response,500);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in viewing members\n");
        exit(1);
    }
    printf("List of all members:\n ");
    while(strcmp(response,"End")!=0){
        printf("%s\n",response);
        ret=read(sockfd,response,500);
        response[ret]='\0';
    }
}

void remove_member(int sockfd){
    char username[100];
    printf("Enter username to remove: ");
    scanf("%s",username);
    getchar();
    write(sockfd,username,strlen(username));
    char response[100];
    ret=read(sockfd,response,100);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in removing member\n");
        exit(1);
    }
    printf("%s\n",response);
}


void print_member_menu(){
    printf("1. View all books\n");
    printf("2. Search book by name\n");
    printf("3. Show books of a particular author\n");
    printf("4. Borrow book\n");
    printf("5. Return book\n");
    printf("6. Change password\n");
    printf("7. Exit\n");
}

void search_book_name(int sockfd){
    char name[100];
    printf("Enter name of the book to search: ");
    fgets(name,100,stdin);
    write(sockfd,name,strlen(name));
    char response[500];
    ret=read(sockfd,response,500);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in searching book\n");
        exit(1);
    }
    printf("Books with name- %s\n",name);
    while(strcmp(response,"End")!=0){
        printf("%s\n",response);
        ret=read(sockfd,response,500);
        response[ret]='\0';
    }
    printf("\n");
}

void search_book_author(int sockfd){
    char author[100];
    printf("Enter name of the author to search: ");
    fgets(author,100,stdin);
    write(sockfd,author,strlen(author));
    char response[500];
    ret=read(sockfd,response,500);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in searching author\n");
        exit(1);
    }
    printf("Books by- %s\n",author);
    while(strcmp(response,"End")!=0){
        printf("%s\n",response);
        ret=read(sockfd,response,500);
        response[ret]='\0';
    }
}

void borrow_book(int sockfd){
    write(sockfd,username,strlen(username));
    int book_id;
    printf("Enter book id to borrow: ");
    scanf("%d",&book_id);
    getchar();
    write(sockfd,&book_id,sizeof(int));
    char response[100];
    ret=read(sockfd,response,100);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in borrowing book\n");
        exit(1);
    }
    printf("%s\n",response);
}

void return_book(int sockfd){
    write(sockfd,username,strlen(username));
    int book_id;
    printf("Enter book id to return: ");
    scanf("%d",&book_id);
    getchar();
    write(sockfd,&book_id,sizeof(int));
    char response[100];
    ret=read(sockfd,response,100);
    response[ret]='\0';
    if(strcmp(response,"Error")==0){
        printf("Error in returning book\n");
        exit(1);
    }
    printf("%s\n",response);
}

void change_password(int sockfd){
    while(1){
        write(sockfd,username,strlen(username));
        char old_password[100];
        printf("Enter old password: ");
        scanf("%s",old_password);
        getchar();
        write(sockfd,old_password,strlen(old_password));
        char response[100];
        ret=read(sockfd,response,100);
        response[ret]='\0';
        if(strcmp(response,"Error")==0){
            printf("Error in changing password\n");
            exit(1);
        }
        if(strcmp(response,"Incorrect password")==0){
            printf("%s\n1. Try again 2. Go back: ",response);
            int choice;
            scanf("%d",&choice);
            getchar();
            write(sockfd,&choice,sizeof(int));
            if(choice==2)
                return;
            continue;
        }
        char new_password[100];
        printf("Enter new password: ");
        scanf("%s",new_password);
        getchar();
        write(sockfd,new_password,strlen(new_password));
        ret=read(sockfd,response,100);
        response[ret]='\0';
        if(strcmp(response,"Password changed successfully")==0){
            printf("%s\n",response);
            break;
        }
    }
}

void reg(int sockfd)
{
    while (1)
    {
        char response[100];
        char username[100];
        char password[100];
        printf("Enter username: ");
        scanf("%s", username);
        getchar();
        write(sockfd, username, strlen(username));
        ret = read(sockfd, response, 100);
        response[ret] = '\0';
        if (strcmp(response, "Username already exists") == 0)
        {
            printf("Username already exists. Register with another username.\n");
            continue;
        }
        printf("%s\n", response);
        printf("Enter password: ");
        scanf("%s", password);
        getchar();
        write(sockfd, password, strlen(password));
        ret = read(sockfd, response, 100);
        response[ret] = '\0';
        if (strcmp(response, "Registration successful") == 0)
        {
            printf("Registration successful\n");
            break;
        }
    }

}

int login(int sockfd)
{
    char response[100];
    while (1)
    {
        printf("Login as admin/member?: ");
        char choice[10];
        scanf("%s", choice);
        getchar();
        if(strcmp(choice,"admin")!=0 && strcmp(choice,"member")!=0){
            printf("Invalid choice\n");
            continue;
        }
        write(sockfd, choice, strlen(choice));
        printf("Enter username: ");
        scanf("%s", username);
        getchar();
        write(sockfd, username, strlen(username));
        ret = read(sockfd, response, 100);
        response[ret] = '\0';
        if (strcmp(response, "Username does not exist") == 0)
        {
            printf("Username does not exist. 1. Try again 2. Go back\n");
            char ch;
            scanf("%c", &ch);
            getchar();
            write(sockfd, &ch, 1);
            if (ch == '2')
                return 0;
            continue;
        }
        printf("Enter password: ");
        scanf("%s", password);
        getchar();
        write(sockfd, password, strlen(password));
        ret = read(sockfd, response, 100);
        response[ret] = '\0';
        if (strcmp(response, "Incorrect password") == 0)
        {
            printf("Incorrect password. 1. Try again 2. Go back\n");
            char ch;
            scanf("%c", &ch);
            getchar();
            write(sockfd, &ch, 1);
            if (ch == '2')
                return 0;
            continue;
        }
        printf("%s\n", response);
        if(strcmp(choice,"admin")==0){
            return 1;
        }
        else if(strcmp(choice,"member")==0){
            return 2;
        }
    }
}

int main()
{
    int sockfd;
    int r;
    struct sockaddr_in server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket():");
        exit(1);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to the server
    r = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (r < 0)
    {
        perror("connect():");
        exit(1);
    }
    printf("Welcome to Online Library Management System\n");
    int login_flag = 0;
    // login
    while (1)
    {
        printf("Login,register or exit? (login/register/exit): ");
        char choice[10];
        scanf("%s", choice);
        getchar();
        if (strcmp(choice, "login") == 0)
        {
            write(sockfd, choice, strlen(choice));
            login_flag = login(sockfd);
            if(login_flag)
                break;
            if(login_flag==0)
                continue;
        }
        else if (strcmp(choice, "register") == 0)
        {
            write(sockfd, choice, strlen(choice));
            reg(sockfd);
            continue;
        }
        else if (strcmp(choice, "exit") == 0)
        {
            write(sockfd, choice, strlen(choice));
            char response[100];
            ret = read(sockfd, response, 100);
            response[ret] = '\0';
            printf("%s\n", response);
            close(sockfd);
            exit(0);
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    while(1){
        if(login_flag==1){
            print_admin_menu();
            int choice;
            printf("Enter operation choice: ");
            scanf("%d",&choice);
            getchar();
            write(sockfd,&choice,sizeof(int));
            switch(choice){
                case 1:
                {
                    add_book(sockfd);
                    break;
                }
                case 2:
                {
                    remove_book(sockfd);
                    break;
                }
                case 3:
                {
                    view_books(sockfd);
                    break;
                }
                case 4:
                {
                    search_book(sockfd);
                    break;
                }
                case 5:
                {
                    search_member(sockfd);
                    break;
                }

                case 6:
                {
                    remove_member(sockfd);
                    break;
                }

                case 7:
                {
                    view_all_members(sockfd);
                    break;
                }
                case 8:
                {
                    close(sockfd);
                    exit(0);
                }
                default:
                {
                    printf("Invalid choice\n");
                    break;
                }
            }
        }
        else if(login_flag==2){
            print_member_menu();
            int choice;
            printf("Enter operation choice: ");
            scanf("%d",&choice);
            getchar();
            write(sockfd,&choice,sizeof(int));
            switch(choice){
                case 1:
                {
                    view_books(sockfd);
                    break;
                }

                case 2:
                {
                    search_book_name(sockfd);
                    break;
                }

                case 3:
                {
                    search_book_author(sockfd);
                    break;
                }

                case 4:
                {
                    borrow_book(sockfd);
                    break;
                }

                case 5:
                {
                    return_book(sockfd);
                    break;
                }   

                case 6:
                {
                    change_password(sockfd);
                    break;
                }

                case 7:
                {
                    close(sockfd);
                    exit(0);
                }

                default:
                {
                    printf("Invalid choice\n");
                    break;
                }
            }

        }
    }
    
}