#define SERVER_PORT 9991
#define MEMBER_FILE "members.bin"
#define BOOK_FILE "books.bin"
#define ADMIN_FILE "admins.bin"


struct book{
    int book_id;
    char name[100];
    char author[100];
    int num_borrowed;
    char status[150];
};

struct borrowed_book{
    int book_id;
    char name[100];
    char author[100];
    char borrow_date[100];
    char return_date[100];
    char status[100];
};

struct user{
    char username[100];
    char password[100];
    struct borrowed_book books_borrowed[100];
    int num_books;
    int total_borrows;
};

struct admin{
    char username[100];
    char password[100];
};

int extern admin_size;

int extern user_size;




void reg(int new_sock);
void login(int new_sock);
