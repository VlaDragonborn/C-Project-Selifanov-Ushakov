#ifndef DATABACE
#define DATABACE

#define FILE_NAME "clientsdb.txt"

struct user_t
{
    int id;
    char* login;
};

int get_free_index(void);

void user_regestration(user_t* user);

int find_id(char* log);

char* find_login(int id);

#endif