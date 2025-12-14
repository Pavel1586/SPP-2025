#include <iostream>
#include <time.h>

extern "C" {
    int __stdcall slength(char* buffer, char* str)
    {
        if (str == nullptr)
            return 0;
        int len = 0;
        for (int i = 0; i < 256; i++)
            if (str[i] == '\0')
            {
                len = i; break;
            }
        return len;
    }

    int __stdcall atoii(char* buffer, char* ptr)
    {
        int n = 0;
        while (*ptr >= '0' && *ptr <= '9') {
            n *= 10;
            n += *ptr++;
            n -= '0';
        }
        return n;

    }

    int __stdcall outrad(char* ptr)
    {
        if (ptr == nullptr)
        {
            std::cout << std::endl;
        }
        for (int i = 0; ptr[i] != '\0'; i++)
            std::cout << ptr[i];
        return 0;
    }

    char* __stdcall copystr(char* buffer, char* str)
    {
        int i = NULL, len1 = NULL, len2 = NULL;
        for (int j = 0; str[j] != '\0'; j++)
        {
            if (i == 255)
                break;
            buffer[i++] = str[j];
        }
        buffer[i] = '\0';
        return buffer;
    }

    int __stdcall compare(char* ptr, char* a, char* b)
    {
        if (strcmp(a, b) < 0)
            return 0;
        if (strcmp(a, b) == 0)
            return 1;
        if (strcmp(a, b) > 0)
            return 2;
        return 0;
    }

    char* __stdcall concat(char* buffer, char* str1, char* str2)
    {
        int i = 0;
        for (int j = 0; str1[j] != '\0'; j++)
        {
            if (i == 255)
                break;
            buffer[i++] = str1[j];
        }
        for (int j = 0; str2[j] != '\0'; j++)
        {
            if (i == 255)
                break;
            buffer[i++] = str2[j];
        }
        buffer[i] = '\0';
        return buffer;
    }

    int __stdcall power(char* ptr, int num, int exponent)
    {
        int res = 1;
        for (int i = 0; i < exponent; i++)
        {
            if (res * num > 4294967295)
            {
                std::cout << "Degree overflow" << std::endl;
                throw - 1;
            }
            res *= num;
        }
        return res;
    }

    int __stdcall factorial(char* ptr, int n) //  
    {
        if (n < 0)
            return 1;
        if (n == 0 || n == 1)
            return 1;
        int res = 1;
        for (int i = 2; i <= n; i++)
        {
            if (res * i > 2147483647)
            {
                std::cout << " " << std::endl;
                throw - 1;
            }
            res *= i;
        }
        return res;
    }

    int __stdcall rnd(char* ptr, int a, int b)
    {
        srand(time(NULL));
        int res;
        res = rand() % b + a;
        return res;
    }

    void _pause()
    {
        system("pause");
    }

    int __stdcall outlich(int value)
    {
        std::cout << value;
        return 0;
    }
}
