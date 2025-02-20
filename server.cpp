#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iostream>

#include "almostDatabace.h"

#define SOCKET_PATH "my_socket"
#define BUFFER_SIZE 256

int **user_in_online;


void send_massage(int client_sock, char* response)
{
    if (write(client_sock, response, strlen(response)) < 0) {
        perror("Ошибка при отправке ответа");
    }
}


int regestration_user(int client_sock, user_t* user)
{
    char* responce;

    char buf[BUFFER_SIZE];
    ssize_t bytes_read;

    bytes_read = read(client_sock, buf, sizeof(buf) - 1);

    buf[bytes_read] = '\0';
    char* login = new char(bytes_read);

    for (int i = 0; i < bytes_read; i++)
        login[i] = buf[i];
           
    user->login = login;

    int id = find_id(user->login);
    if (id < 0)
    {
        id = get_free_index();
        if (id > 9)
        {
            responce = "Cервер переполнен. Регистрация невозможна\n";
            send_massage(client_sock, responce);
            return 0;
        }
        user->id = id;
        user_regestration(user);
    }
    else user->id = id;

    std::cout << id << std::endl;

    user_in_online[id][0] = client_sock;
    user_in_online[id][1] = 1;

    responce = "Добро пожаловать!\n";
    send_massage(client_sock, responce);
    return 1;
}


void* handle_client(void* arg) {
    int client_sock = *(int*) arg;
    free(arg); 

    char* responce;

    user_t user;
    regestration_user(client_sock, &user);

    char buf[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while (true)
    {
        responce = "В онлайне:\n";
        send_massage(client_sock, responce);

        int stop = get_free_index();
        for (int i = 0; i < stop; i++)
        {
            if (user_in_online[i][1] == 1 && user.id != i)
            {
                responce = find_login(i);
                send_massage(client_sock, responce);
                send_massage(client_sock, "\n");
            }
        }
        responce = "Введите что угодно, чтобы обновить\nИли введите имя пользователя, чтобы написать ему\n";
        send_massage(client_sock, responce);

        bytes_read = read(client_sock, buf, sizeof(buf) - 1);
        buf[bytes_read] = '\0';

        int id = find_id(buf);
        if (id >= 0 && id < stop)
        {
            if (user_in_online[id][1] == 1)
            {
                responce = "!";
                send_massage(client_sock, responce);

                bytes_read = read(client_sock, buf, sizeof(buf) - 1);
                buf[bytes_read] = '\0';           

                send_massage(user_in_online[id][0], buf);
            }
        }
        std::cin.ignore();
    }

    close(client_sock);
    printf("Клиент отключен.\n");

    return NULL;
}

int main() {
    std::cout << "\033cСервер запущен\n";

    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    user_in_online = new int*[10];
    for(int i = 0; i < 10; i++)
    {
        user_in_online[i] = new int[2];   
        user_in_online[i][0] = 0; user_in_online[i][1] = 0;
    }
    
    unlink(SOCKET_PATH);

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Ошибка при создании сокета");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при привязке");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 10) < 0) {
        perror("Ошибка при listen");
        close(server_sock);
        exit(1);
    }

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Ошибка при accept");
            continue;
        }

        printf("Новый клиент подключен.\n");

        int* client_sock_ptr = (int *)malloc(sizeof(int));
        if (!client_sock_ptr) {
            perror("Ошибка при выделении памяти");
            close(client_sock);
            continue;
        }
        *client_sock_ptr = client_sock;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_sock_ptr) != 0) {
            perror("Ошибка при создании потока");
            free(client_sock_ptr);
            close(client_sock);
        } else {
            pthread_detach(thread);
        }
    }

    close(server_sock);
    unlink(SOCKET_PATH);

    return 0;
}