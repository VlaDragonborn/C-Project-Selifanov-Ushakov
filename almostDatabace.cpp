#include <fstream>
#include <iostream>
#include <string>
#include "almostDatabace.h"


int get_free_index()
{
    std::string line;
    std::ifstream in(FILE_NAME);
    int index = 0;
    while (std::getline(in, line)) index++;
    in.close();
    return index;    
}


void user_regestration(user_t* user)
{
    user->id = get_free_index();
    
    std::ofstream out(FILE_NAME, std::ios::app);
    out << user->id << ":" << user->login << ':' << std::endl;
    out.close();
}


int find_id(char* log)
{
    std::string line;
    std::ifstream in(FILE_NAME);
    int index = 0;
    while(std::getline(in, line))
    {
        int position = line.rfind(":");
        std::string login = line.substr(2, position - 2);
        if (log == login)
        {
            in.close();
            return index;
        }
        index++;
    }
    in.close();
    return -1;
}


char* find_login(int id)
{
    std::string line;
    std::ifstream in(FILE_NAME);
    while(std::getline(in, line))
    {
        if (line[0] - 48 == id)
        {
            int position = line.rfind(":");
            char* login = new char(position - 2);
            for (int i = 2; i < position; i++)
                login[i - 2] = line[i];

            in.close();
            return login;        
        }
    } 
}
