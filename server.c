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
#include "server.h"
#include <pthread.h>
#include <semaphore.h>

int user_size = sizeof(struct user);
int admin_size = sizeof(struct admin);
int ret;
int cont = 0;
int login_flag = 0;

//semaphores
sem_t members_sem;
sem_t books_sem;
sem_t admin_sem;

void handle_disconnect(int new_sock)
{
    printf("Client disconnected\n");
    close(new_sock);
    pthread_exit(NULL);
}

void reg(int new_sock)
{
    char username[100];
    char password[100];
    char response[100];
    int u_fd;
    u_fd = open(MEMBER_FILE, O_RDWR | O_CREAT, 0666);
    while (1)
    {
        lseek(u_fd, 0, SEEK_SET);
        ret = read(new_sock, username, 100);
        if(ret<=0) handle_disconnect(new_sock);
        username[ret] = '\0';
        int f = 0;
        struct user *u = (struct user *)malloc(user_size);
        sem_wait(&members_sem);
        while (read(u_fd, u, user_size))
        {
            if (strcmp(u->username, username) == 0)
            {
                f = 1;
                break;
            }
        }
        sem_post(&members_sem);
        if (f == 1)
        {
            strcpy(response, "Username already exists");
            write(new_sock, response, 100);
            continue;
        }
        strcpy(response, "Username available");
        write(new_sock, response, 100);
        ret = read(new_sock, password, 100);
        if(ret<=0) handle_disconnect(new_sock);
        password[ret] = '\0';
        strcpy(u->username, username);
        strcpy(u->password, password);
        memset(u->books_borrowed, 0, sizeof(u->books_borrowed));
        u->num_books = 0;
        u->total_borrows = 0;
        lseek(u_fd, 0, SEEK_END);
        sem_wait(&members_sem);
        write(u_fd, u, user_size);
        sem_post(&members_sem);
        strcpy(response, "Registration successful");
        write(new_sock, response, 100);
        close(u_fd);
        break;
    }
    return;
}

void login(int new_sock)
{
    while (1)
    {
        char username[100];
        char password[100];
        char response[100];
        int u_fd;
        int a_fd;
        char choice[10];
        ret = read(new_sock, choice, 10);
        if(ret<=0) handle_disconnect(new_sock);
        choice[ret] = '\0';
        if (strcmp(choice, "admin") == 0)
        {
            // admin login
            a_fd = open(ADMIN_FILE, O_RDONLY);
            if(a_fd<0){
                perror("open():");
                pthread_exit(NULL);
            }
            if (a_fd < 0)
            {
                perror("open():");
                pthread_exit(NULL);
            }
            if (u_fd < 0)
            {
                perror("open():");
                pthread_exit(NULL);
            }
            ret = read(new_sock, username, 100);
            if(ret<=0) handle_disconnect(new_sock);
            username[ret] = '\0';
            struct admin *a = (struct admin *)malloc(admin_size);
            int f = 0;
            while (read(a_fd, a, admin_size))
            {
                if (strcmp(a->username, username) == 0)
                {
                    f = 1;
                    break;
                }
            }
            if (f == 0)
            {
                strcpy(response, "Username does not exist");
                write(new_sock, response, 100);
                char ch;
                ret = read(new_sock, &ch, sizeof(char));
                if(ret<=0) handle_disconnect(new_sock);
                if (ch == '2')
                {
                    close(a_fd);
                    cont = 1;
                    return;
                }
                close(a_fd);
                continue;
            }
            strcpy(response, "Username exists");
            write(new_sock, response, 100);
            ret = read(new_sock, password, 100);
            if(ret<=0) handle_disconnect(new_sock);
            password[ret] = '\0';
            if (strcmp(a->password, password) != 0)
            {
                strcpy(response, "Incorrect password");
                write(new_sock, response, 100);
                char ch;
                ret = read(new_sock, &ch, sizeof(char));
                if(ret<=0) handle_disconnect(new_sock);
                if (ch == '2')
                {
                    close(a_fd);
                    cont = 1;
                    return;
                }
                close(a_fd);
                continue;
            }
            strcpy(response, "Login successful");
            write(new_sock, response, 100);
            close(u_fd);
            if (strcmp(choice, "admin") == 0)
            {
                login_flag = 1;
            }
            else if (strcmp(choice, "member") == 0)
            {
                login_flag = 2;
            }
            return;
        }
        else if (strcmp(choice, "member") == 0)
        {
            // user login
            u_fd = open(MEMBER_FILE, O_RDONLY);
            if (u_fd < 0)
            {
                perror("open():");
                pthread_exit(NULL);
            }
            ret = read(new_sock, username, 100);
            if(ret<=0) handle_disconnect(new_sock);
            username[ret] = '\0';
            struct user *u = (struct user *)malloc(user_size);
            int f = 0;
            sem_wait(&members_sem);
            while (read(u_fd, u, user_size))
            {
                if (strcmp(u->username, username) == 0)
                {
                    f = 1;
                    break;
                }
            }
            sem_post(&members_sem);
            if (f == 0)
            {
                strcpy(response, "Username does not exist");
                write(new_sock, response, 100);
                char ch;
                ret = read(new_sock, &ch, sizeof(char));
                if(ret<=0) handle_disconnect(new_sock);
                if (ch == '2')
                {
                    close(u_fd);
                    cont = 1;
                    return;
                }
                close(u_fd);
                continue;
            }
            strcpy(response, "Username exists");
            write(new_sock, response, 100);
            ret = read(new_sock, password, 100);
            if(ret<=0) handle_disconnect(new_sock);
            password[ret] = '\0';
            if (strcmp(u->password, password) != 0)
            {
                strcpy(response, "Incorrect password");
                write(new_sock, response, 100);
                char ch;
                ret = read(new_sock, &ch, sizeof(char));
                if(ret<=0) handle_disconnect(new_sock);
                if (ch == '2')
                {
                    close(u_fd);
                    cont = 1;
                    return;
                }
                close(u_fd);
                continue;
            }
            strcpy(response, "Login successful");
            write(new_sock, response, 100);
            close(u_fd);
            if (strcmp(choice, "admin") == 0)
            {
                login_flag = 1;
            }
            else if (strcmp(choice, "member") == 0)
            {
                login_flag = 2;
            }
            return;
        }
    }
}


void add_book(int new_sock)
{
    printf("Adding book\n");
    int book_id;
    char name[100];
    char author[100];
    int num_borrowed=0;
    char status[150];

    // Read data from the socket
    ret=read(new_sock, &book_id, sizeof(int));
    if(ret<=0) handle_disconnect(new_sock);
    ret=read(new_sock, name, sizeof(name));
    if(ret<=0) handle_disconnect(new_sock);
    name[ret-1]='\0';
    ret=read(new_sock, author, sizeof(author));
    if(ret<=0) handle_disconnect(new_sock);
    author[ret-1]='\0';
    strcpy(status, "Available");
    struct book* b = (struct book*)malloc(sizeof(struct book));
    b->book_id = book_id;
    strcpy(b->name, name);
    strcpy(b->author, author);
    b->num_borrowed = num_borrowed;
    strcpy(b->status, status);

    int fd = open(BOOK_FILE, O_RDWR | O_CREAT, 0666);
    if (fd < 0)
    {
        perror("open()");
        write(new_sock, "Error", sizeof("Error"));
        pthread_exit(NULL);
    }

    // Check for duplicate book ID
    lseek(fd, 0, SEEK_SET);
    int f = 0;
    struct book* temp=(struct book*)malloc(sizeof(struct book));
    sem_wait(&books_sem);
    while (read(fd, temp, sizeof(struct book)) == sizeof(struct book))
    {
        if (temp->book_id == book_id)
        {
            f = 1;
            break;
        }
    }
    sem_post(&books_sem);

    if (f == 1)
    {
        write(new_sock, "Book already exists", sizeof("Book already exists"));
        printf("Book already exists\n");
        close(fd);
    }
    else
    {
        lseek(fd, 0, SEEK_END);
        sem_wait(&books_sem);
        write(fd, b, sizeof(struct book));
        sem_post(&books_sem);
        printf("Book added\n");
        write(new_sock, "Book added successfully", sizeof("Book added successfully"));
        close(fd);
    }
    return;
}

void remove_book(int new_sock)
{
    int book_id;
    ret=read(new_sock, &book_id, sizeof(int));
    if(ret<=0) handle_disconnect(new_sock);
    int fd = open(BOOK_FILE, O_RDWR);
    if (fd < 0)
    {
        perror("open()");
        write(new_sock, "Error", sizeof("Error"));
        pthread_exit(NULL);
    }
    int temp_fd = creat("temp.dat", 0666);
    if (temp_fd < 0)
    {
        perror("creat()");
        write(new_sock, "Error", sizeof("Error"));
        pthread_exit(NULL);
    }
    struct book* b=(struct book*)malloc(sizeof(struct book));
    int f = 0;
    sem_wait(&books_sem);
    while (read(fd, b, sizeof(struct book)) != 0)
    {
        if (b->book_id == book_id)
        {
            if(strcmp(b->status,"Available")!=0){
                write(new_sock,"Book is currently borrowed by a member",sizeof("Book is currently borrowed by a member"));
                sem_post(&books_sem);
                close(fd);
                close(temp_fd);
                return;
            }
            f = 1;
            continue;
        }
        write(temp_fd, b, sizeof(struct book));
    }
    sem_post(&books_sem);
    if (f == 0)
    {
        write(new_sock, "Book not found", sizeof("Book not found"));
        remove("temp.dat");
        close(fd);
        close(temp_fd);
        return;
    }
    else
    {
        write(new_sock, "Book removed successfully", sizeof("Book removed successfully"));
        rename("temp.dat", BOOK_FILE);
        close(fd);
        close(temp_fd);
        return;
    }
}

void view_books(int new_sock){
    int fd=open(BOOK_FILE,O_RDONLY);
    if(fd<0){
        perror("open():");
        write(new_sock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    printf("Viewing all books\n");
    struct book* b=(struct book*)malloc(sizeof(struct book));
    lseek(fd,0,SEEK_SET);
    char response[500];
    sem_wait(&books_sem);
    while(read(fd,b,sizeof(struct book))!=0){
        sprintf(response,"\tBook ID: %d\n\tName: %s\n\tAuthor: %s\n\tNumber of times the book was borrowed: %d\n\tStatus: %s\n",b->book_id,b->name,b->author,b->num_borrowed,b->status);
        write(new_sock,response,sizeof(response));
    }
    write(new_sock,"End",sizeof("End"));
    sem_post(&books_sem);
    close(fd);
    return;

}

void search_book(int new_sock){
    int book_id;
    ret=read(new_sock,&book_id,sizeof(int));
    if(ret<=0) handle_disconnect(new_sock);
    int fd=open(BOOK_FILE,O_RDONLY);
    if(fd<0){
        perror("open():");
        write(new_sock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    printf("Searching book\n");
    struct book* b=(struct book*)malloc(sizeof(struct book));
    lseek(fd,0,SEEK_SET);
    int f=0;
    sem_wait(&books_sem);
    while(read(fd,b,sizeof(struct book))!=0){
        if(b->book_id==book_id){
            f=1;
            break;
        }
    }
    sem_post(&books_sem);
    if(f==0){
        write(new_sock,"Book not found",sizeof("Book not found"));
        close(fd);
        return;
    }
    else{
        char response[500];
        memset(response,0,500);
        sprintf(response,"\tBook ID: %d\n\tName: %s\n\tAuthor: %s\n\tNumber of times the book was borrowed: %d\n\tStatus: %s\n",b->book_id,b->name,b->author,b->num_borrowed,b->status);
        write(new_sock,response,sizeof(response));
        close(fd);
        return;
    }

}

void search_member(int new_sock){
    char username[100];
    ret=read(new_sock,username,100);
    if(ret<=0) handle_disconnect(new_sock);
    username[ret]='\0';
    int fd=open(MEMBER_FILE,O_RDONLY);
    if(fd<0){
        perror("open():");
        write(new_sock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    printf("Searching member\n");
    struct user* u=(struct user*)malloc(sizeof(struct user));
    lseek(fd,0,SEEK_SET);
    int f=0;
    sem_wait(&members_sem);
    while(read(fd,u,sizeof(struct user))!=0){
        if(strcmp(u->username,username)==0){
            f=1;
            break;
        }
    }
    sem_post(&members_sem);
    if(f==0){
        write(new_sock,"Member not found",sizeof("Member not found"));
        close(fd);
        return;
    }
    char response[2000];
    memset(response,0,2000);
    sprintf(response,"Username: %s\nNumber of books borrowed: %d\n",u->username,u->num_books);
    strcat(response,"Books borrowed:\n");
    write(new_sock,response,sizeof(response));
    memset(response,0,2000);
    for(int i=0;i<u->total_borrows;i++){
        sprintf(response,"\tBook ID: %d\n\tName: %s\n\tAuthor: %s\n\tBorrowed date: %s\tReturn date: %s\tStatus: %s\n",u->books_borrowed[i].book_id,u->books_borrowed[i].name,u->books_borrowed[i].author,u->books_borrowed[i].borrow_date,u->books_borrowed[i].return_date,u->books_borrowed[i].status);
        write(new_sock,response,sizeof(response));
    }
    write(new_sock,"End",sizeof("End"));
    close(fd);
    return;
    
}

void view_all_members(int newsock){
    int fd=open(MEMBER_FILE,O_RDONLY);
    if(fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    printf("Viewing all members\n");
    struct user* u=(struct user*)malloc(sizeof(struct user));
    lseek(fd,0,SEEK_SET);
    char response[500];
    sem_wait(&members_sem);
    while(read(fd,u,sizeof(struct user))!=0){
        sprintf(response,"\tUsername: %s\n\tNumber of books borrowed: %d\n",u->username,u->num_books);
        write(newsock,response,sizeof(response));
    }
    sem_post(&members_sem);
    write(newsock,"End",sizeof("End"));
    close(fd);
    return;
}

void remove_member(int newsock){
    char username[100];
    ret=read(newsock,username,100);
    if(ret<=0) handle_disconnect(newsock);
    username[ret]='\0';
    int fd=open(MEMBER_FILE,O_RDWR);
    if(fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    int temp_fd=creat("temp.dat",0666);
    if(temp_fd<0){
        perror("creat():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    struct user* u=(struct user*)malloc(sizeof(struct user));
    int f=0;
    sem_wait(&members_sem);
    while(read(fd,u,sizeof(struct user))!=0){
        if(strcmp(u->username,username)==0){
            if(u->num_books>0){
                write(newsock,"Member has books to return",sizeof("Member has books to return"));
                sem_post(&members_sem);
                close(fd);
                close(temp_fd);
                return;
            }
            f=1;
            continue;
        }
        write(temp_fd,u,sizeof(struct user));
    }
    sem_post(&members_sem);
    if(f==0){
        write(newsock,"Member not found",sizeof("Member not found"));
        remove("temp.dat");
        close(fd);
        close(temp_fd);
        return;
    }
    else{
        write(newsock,"Member removed successfully",sizeof("Member removed successfully"));
        remove(MEMBER_FILE);
        rename("temp.dat",MEMBER_FILE);
        close(fd);
        close(temp_fd);
        return;
    }
}

void search_book_name(int newsock){
    char name[100];
    ret=read(newsock,name,100);
    if(ret<=0) handle_disconnect(newsock);
    name[ret-1]='\0'; //removing the newline character from the end of the string
    int fd=open(BOOK_FILE,O_RDONLY);
    if(fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    printf("Searching book by name\n");
    struct book* b=(struct book*)malloc(sizeof(struct book));
    lseek(fd,0,SEEK_SET);
    int f=0;
    sem_wait(&books_sem);
    while(read(fd,b,sizeof(struct book))!=0){
        if(strcmp(b->name,name)==0){
            f=1;
            break;
        }
    }
    sem_post(&books_sem);
    if(f==0){
        write(newsock,"Book not found",sizeof("Book not found"));
        close(fd);
        return;
    }
    else{
        char response[500];
        memset(response,0,500);
        lseek(fd,0,SEEK_SET);
        sem_wait(&books_sem);
        while(read(fd,b,sizeof(struct book))!=0){
            if(strcmp(b->name,name)==0){
                sprintf(response,"\tBook ID: %d\n\tName: %s\n\tAuthor: %s\n\tNumber of times the copy was borrowed: %d\n\tStatus: %s\n",b->book_id,b->name,b->author,b->num_borrowed,b->status);
                write(newsock,response,sizeof(response));
            }
        }
        sem_post(&books_sem);
        write(newsock,"End",sizeof("End"));
        close(fd);
        return;
    }
}

void show_books_author(int newsock){
    char author[100];
    ret=read(newsock,author,100);
    if(ret<=0) handle_disconnect(newsock);
    author[ret-1]='\0';
    int fd=open(BOOK_FILE,O_RDONLY);
    if(fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    printf("Showing books by a particular author\n");
    struct book* b=(struct book*)malloc(sizeof(struct book));
    lseek(fd,0,SEEK_SET);
    int f=0;
    char response[500];
    sem_wait(&books_sem);
    while(read(fd,b,sizeof(struct book))!=0){
        if(strcmp(b->author,author)==0){
            f=1;
            sprintf(response,"\tBook ID: %d\n\tName: %s\n\tAuthor: %s\n\tNumber of times the copy was borrowed: %d\n\tStatus: %s\n",b->book_id,b->name,b->author,b->num_borrowed,b->status);
            write(newsock,response,sizeof(response));
        }
    }
    sem_post(&books_sem);
    if(f==0){
        write(newsock,"No books found by given author",sizeof("No books found by given author"));
        close(fd);
        return;
    }
    else{
        write(newsock,"End",sizeof("End"));
        close(fd);
        return;
    }
}

void borrow_book(int newsock){
    char username[100];
    int book_id;
    ret=read(newsock,username,100);
    if(ret<=0) handle_disconnect(newsock);
    username[ret]='\0';
    ret=read(newsock,&book_id,sizeof(int));
    if(ret<=0) handle_disconnect(newsock);
    int u_fd=open(MEMBER_FILE,O_RDWR);
    if(u_fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    int b_fd=open(BOOK_FILE,O_RDWR);
    if(b_fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    struct user* u=(struct user*)malloc(sizeof(struct user));
    struct book* b=(struct book*)malloc(sizeof(struct book));
    int f=0;
    sem_wait(&members_sem);
    while(read(u_fd,u,sizeof(struct user))!=0){
        if(strcmp(u->username,username)==0){
            f=1;
            break;
        }
    }
    sem_post(&members_sem);
    if(f==0){
        write(newsock,"Member not found",sizeof("Member not found"));
        close(u_fd);
        close(b_fd);
        return;
    }
    f=0;
    sem_wait(&books_sem);
    while(read(b_fd,b,sizeof(struct book))!=0){
        if(b->book_id==book_id){
            f=1;
            break;
        }
    }
    sem_post(&books_sem);
    if(f==0){
        write(newsock,"Book not found",sizeof("Book not found"));
        close(u_fd);
        close(b_fd);
        return;
    }
    if(strcmp(b->status,"Available")!=0){
        write(newsock,"Book unavailable to borrow",sizeof("Book unavailable to borrow"));
        close(u_fd);
        close(b_fd);
        return;
    }
    struct book* temp=(struct book*)malloc(sizeof(struct book));
    temp->book_id=b->book_id;
    strcpy(temp->name,b->name);
    strcpy(temp->author,b->author);
    temp->num_borrowed=b->num_borrowed+1;
    char status[150];
    sprintf(status,"Borrowed by %s",username);
    strcpy(temp->status,status);
    lseek(b_fd,-1*sizeof(struct book),SEEK_CUR);
    sem_wait(&books_sem);
    write(b_fd,temp,sizeof(struct book));
    sem_post(&books_sem);
    struct borrowed_book* bb=(struct borrowed_book*)malloc(sizeof(struct borrowed_book));
    bb->book_id=b->book_id;
    strcpy(bb->name,b->name);
    strcpy(bb->author,b->author);
    time_t now;
    time(&now);
    strcpy(bb->borrow_date,ctime(&now));    
    strcpy(bb->return_date,"Not returned yet\n");
    strcpy(bb->status,"Not returned");
    u->books_borrowed[u->total_borrows]=*bb;
    u->total_borrows+=1;
    u->num_books+=1;
    lseek(u_fd,-1*sizeof(struct user),SEEK_CUR);
    sem_wait(&members_sem);
    write(u_fd,u,sizeof(struct user));
    sem_post(&members_sem);
    write(newsock,"Book borrowed successfully",sizeof("Book borrowed successfully"));
    close(u_fd);
    close(b_fd);
    return;
}

void return_book(int newsock){
    char username[100];
    int book_id;
    ret=read(newsock,username,100);
    if(ret<=0) handle_disconnect(newsock);
    username[ret]='\0';
    ret=read(newsock,&book_id,sizeof(int));
    if(ret<=0) handle_disconnect(newsock);
    int u_fd=open(MEMBER_FILE,O_RDWR);
    if(u_fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    int b_fd=open(BOOK_FILE,O_RDWR);
    if(b_fd<0){
        perror("open():");
        write(newsock,"Error",sizeof("Error"));
        pthread_exit(NULL);
    }
    struct user* u=(struct user*)malloc(sizeof(struct user));
    struct book* b=(struct book*)malloc(sizeof(struct book));
    int f=0;
    lseek(u_fd,0,SEEK_SET);
    sem_wait(&members_sem);
    while(read(u_fd,u,sizeof(struct user))!=0){
        if(strcmp(u->username,username)==0){
            f=1;
            break;
        }
    }
    sem_post(&members_sem);
    if(f==0){
        write(newsock,"Member not found",sizeof("Member not found"));
        close(u_fd);
        close(b_fd);
        return;
    }
    f=0;
    lseek(b_fd,0,SEEK_SET);
    sem_wait(&books_sem);
    while(read(b_fd,b,sizeof(struct book))!=0){
        if(b->book_id==book_id){
            f=1;
            break;
        }
    }
    sem_post(&books_sem);
    if(f==0){
        write(newsock,"Book not found",sizeof("Book not found"));
        close(u_fd);
        close(b_fd);
        return;
    }
    f=0;
    struct borrowed_book* bb=(struct borrowed_book*)malloc(sizeof(struct borrowed_book));
    for(int i=0;i<u->num_books;i++){
        if(u->books_borrowed[i].book_id==book_id && strcmp(u->books_borrowed[i].status,"Not returned")==0){
            f=1;
            bb=&(u->books_borrowed[i]);
            break;
        }
    }
    if(f==0){
        write(newsock,"This book has not been borrowed by you.",sizeof("This book has not been borrowed by you."));
        close(u_fd);
        close(b_fd);
        return;
    }
    if(strcmp(bb->status,"Returned")==0){
        write(newsock,"This book has already been returned.",sizeof("This book has already been returned."));
        close(u_fd);
        close(b_fd);
        return;
    }
    strcpy(b->status,"Available");
    lseek(b_fd,-1*sizeof(struct book),SEEK_CUR);
    sem_wait(&books_sem);
    write(b_fd,b,sizeof(struct book));
    sem_post(&books_sem);
    strcpy(bb->status,"Returned");
    time_t now;
    time(&now);
    strcpy(bb->return_date,ctime(&now));
    u->num_books-=1;
    lseek(u_fd,-1*sizeof(struct user),SEEK_CUR);
    sem_wait(&members_sem);
    write(u_fd,u,sizeof(struct user));
    sem_post(&members_sem);
    write(newsock,"Book returned successfully",sizeof("Book returned successfully"));
    close(u_fd);
    close(b_fd);
    return;

}

void change_password(int newsock){
    char username[100];
    char old_password[100];
    char new_password[100];
    while(1){
        ret=read(newsock,username,100);
        if(ret<=0) handle_disconnect(newsock);
        username[ret]='\0';
        ret=read(newsock,old_password,100);
        if(ret<=0) handle_disconnect(newsock);
        old_password[ret]='\0';
        int u_fd=open(MEMBER_FILE,O_RDWR);
        if(u_fd<0){
            perror("open():");
            write(newsock,"Error",sizeof("Error"));
            pthread_exit(NULL);
        }
        struct user* u=(struct user*)malloc(sizeof(struct user));
        int pf=0;
        lseek(u_fd,0,SEEK_SET);
        sem_wait(&members_sem);
        while(read(u_fd,u,sizeof(struct user))!=0){
            if(strcmp(u->username,username)==0){
                if(strcmp(u->password,old_password)==0){
                    pf=1;
                    break;
                }
            }
        }
        sem_post(&members_sem);
        if(pf==0){
            write(newsock,"Incorrect password",sizeof("Incorrect password"));
            int ch;
            ret=read(newsock,&ch,sizeof(int));
            if(ret<=0) handle_disconnect(newsock);
            if(ch==2){
                close(u_fd);
                return;
            }
            continue;
        }
        write(newsock,"Enter new password",sizeof("Enter new password"));
        ret=read(newsock,new_password,100);
        if(ret<=0) handle_disconnect(newsock);
        new_password[ret]='\0';
        strcpy(u->password,new_password);
        lseek(u_fd,-1*sizeof(struct user),SEEK_CUR);
        sem_wait(&members_sem);
        write(u_fd,u,sizeof(struct user));
        sem_post(&members_sem);
        write(newsock,"Password changed successfully",sizeof("Password changed successfully"));
        close(u_fd);
        return;
    }
}

void library(void* arg)
{
    int new_sock = *(int*)arg;  
    printf("New client connected\n");
    while (1)
    {
        cont = 0;
        char str[100];
        ret = read(new_sock, str, 100);
        if(ret<=0) handle_disconnect(new_sock);
        str[ret] = '\0';
        if (strcmp(str, "register") == 0)
        {
            reg(new_sock);
        }
        else if (strcmp(str, "login") == 0)
        {
            login(new_sock);
            if (cont == 1)
            {
                continue;
            }
            if (login_flag)
                break;
        }
        //
        else if (strcmp(str, "exit") == 0)
        {
            char str[100];
            strcpy(str, "Exitting");
            write(new_sock, str, 100);
            close(new_sock);
            pthread_exit(0);
        }
    }
    if (login_flag == 1)
    {
        printf("Admin logged in\n");
        // admin logged in
        while (1)
        {
            int choice;
            ret = read(new_sock, &choice, sizeof(int));
            if(ret<=0) handle_disconnect(new_sock);
            switch (choice)
            {
            case 1: // add book
            {
                add_book(new_sock);
                break;
            }

            case 2:
            {
                // Remove a book
                remove_book(new_sock);
                break;
            }
            case 3:
            {
                // view all books in the library
                view_books(new_sock);
                break;
            }

            case 4:
            {
                //search book by id
                search_book(new_sock);
                break;
            }
            
            case 5:
            {
                //show member details
                search_member(new_sock);
                break;
            }
            case 6:
            {
                //remove a member
                remove_member(new_sock);
                break;
            }
            case 7:
            {
                //view all members in the library
                view_all_members(new_sock);
                break;
            }
            case 8:
            {
                //admin exits
                printf("Admin logged out\n");
                close(new_sock);
                pthread_exit(NULL);
                break;
            }
            default:
            {
                printf("Invalid choice\n");
                break;
            }
            }
        }
        
    }
    else if(login_flag==2){
        printf("Member logged in\n");
        while(1){
            int choice;
            ret=read(new_sock,&choice,sizeof(int));
            if(ret<=0) handle_disconnect(new_sock);
            switch(choice){
                case 1:
                {
                    //view all books
                    view_books(new_sock);
                    break;
                }

                case 2:
                {
                    //search book by name
                    search_book_name(new_sock);
                    break;
                }

                case 3:
                {
                    //view all books of a particular author
                    show_books_author(new_sock);
                    break;
                }

                case 4:
                {
                    //borrow a book
                    borrow_book(new_sock);
                    break;
                }

                case 5:
                {
                    //return a book;
                    return_book(new_sock);
                    break;
                }

                case 6:
                {
                    //change password of a member
                    change_password(new_sock);
                    break;
                }

                case 7:
                {
                    //member exits
                    printf("Member logged out\n");
                    close(new_sock);
                    pthread_exit(NULL);
                    break;
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

int main()
{
    int sockfd, new_sock;
    int r;
    //semaphore initialization
    if(sem_init(&members_sem, 0, 1)!=0){
        perror("sem_init():");
        exit(1);
    }
    if(sem_init(&books_sem, 0, 1)!=0){
        perror("sem_init():");
        exit(1);
    }
    if(sem_init(&admin_sem, 0, 1)!=0){
        perror("sem_init():");
        exit(1);
    }

    struct sockaddr_in local, remote;
    int remote_size = sizeof(remote);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket():");
        exit(1);
    }
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(SERVER_PORT);
    r = bind(sockfd, (struct sockaddr *)&local, sizeof(local));
    if (r < 0)
    {
        perror("bind():");
        exit(1);
    }
    r = listen(sockfd, 10);
    if (r < 0)
    {
        perror("listen():");
        exit(1);
    }

    // starting concurrent server
    while (1)
    {
        memset(&remote, 0, sizeof(remote));
        new_sock = accept(sockfd, (struct sockaddr *)&remote, &remote_size);
        if (new_sock < 0)
        {
            perror("accept():");
            exit(1);
        }
        pthread_t tid;
        if (pthread_create(&tid, NULL, (void *)library, (void*)&new_sock) != 0)
        {
            perror("pthread_create():");
            close(new_sock);
            continue;
        }
        pthread_detach(tid);
    }
    close(sockfd);
    return 0;
}