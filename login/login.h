extern FILE* adminfd;
extern int admin_count;
#define ADMIN_FILE "./login/admins.dat"

struct admin{
    char username[100];
    char password[100];
};

int admin_signup(char* username,char* password);
int admin_login(char*uername,char* password);
void show_admin_menu();