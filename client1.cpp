#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <cmath>

#include "coder.h"

#define MAX_LEN 256


int main() {
    char buffer[MAX_LEN]; // для сокета
    char* message = new char[MAX_LEN]; // для считывания сообщения

    std::cout << "\033c" << "Добро пожаловать в сеть Интернет!\nХотите подключиться к мессенджеру??? [Y/N]" << std::endl;
    while (true)
    {
        
        std::cin.getline(message, MAX_LEN);
        if (message[0] == 'Y' || message[0] == 'N')
        {
            if (message[0] == 'Y')
            {
               break; 
            }
            else
            {
                std::cout << "На нет и суда нет..." << std::endl;
                delete[] message;
                exit(0);
            }
        }
        std::cout << "Error: Неверный формат ввода. Попробуйте еще раз" << std::endl;
    }
    
    
    int sock;
    struct sockaddr_un server_addr;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Ошибка при создании сокета");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "my_socket", sizeof(server_addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при подключении");
        close(sock);
        exit(1);
    }

    std::cout << "Добро пожаловать на сервер! Введите логин\n";

    while(true)
    {
        std::cin.getline(message, MAX_LEN);
        
        if (write(sock, message, strlen(message)) < 0) {
            perror("Ошибка при записи");
        }
        ssize_t bytes_read = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("Ошибка при чтении");
        } else {
            buffer[bytes_read] = '\0';
            if (buffer[0] == '!')
            {
                int* key = generate_key();
                std::cout << key[0] << key[1] << " - ключ шифровки этого сообщения.\nПередайте его оппоненту, чтобы он смог прочитать это смс\n";
                std::cout << "Введите смс\n";
                std::cin.getline(message, MAX_LEN);

                shifrator(key, message);

                int index = 0;
                char answer[MAX_LEN];
                answer[0] = '%';
                while(message[index] != '\0')
                {
                    answer[index + 1] = message[index];
                    index++;
                }
                std::cout << answer << std::endl;
                write(sock, answer, strlen(answer));
            }
            else printf("%s\n", buffer);

        }
    }


    close(sock);
    return 0;
}