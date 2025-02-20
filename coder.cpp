#include <iostream>
#include <random>

using namespace std;


int* generate_key()
{
    // генерация рандомного ключа
    int orientation, step;
    // рандомайзеры будут конфликтовать, поэтому засунул в разные области памяти
    {
        random_device rd;
        mt19937 gen(rd());  
        uniform_int_distribution<> dist(1, 5000);
        step = dist(gen);
    }

    {
        random_device rd1;
        mt19937 gen(rd1());
        uniform_int_distribution<> dist(1,2);
        orientation = dist(gen);
    }

    int* key = new int[2];
    key[0] = orientation; key[1] = step;
    return key;
}


void shifrator(int* key, char* text)
{
    // шифровка текста Цезарем
    const int lenAlphabet = 128;
    const int startAlphabet = 0;
    const int endAlphabet = 127;

    int orientation = key[0], step = key[1];
    step -= step / lenAlphabet * lenAlphabet;

    if (orientation == 1)
    {
        int index = 0;
        while(text[index] != '\0')
        {
            if ((int)text[index] + step > endAlphabet) text[index] = text[index] + step - lenAlphabet;
            else text[index] += step;
            index++; 
        }
    }
    else
    {
        int index = 0;
        while(text[index] != '\0')
        {
            if ((int)text[index] - step > startAlphabet) text[index] = text[index] - step + lenAlphabet;
            else text[index] -= step;
            index++; 
        }        
    }
}


void deshifrator(int* key, char* text)
{
    // тупо меняем ориентацию с таким же шагом и вызываем шифратор 
    int orientation = key[0], step = key[1];
    if (orientation == 1) orientation = 0;
    else orientation = 1;
    int* new_key = new int[2];
    new_key[0] = orientation; new_key[1] = step;

    shifrator(new_key, text);
    delete(new_key);
}
