//
// Created by Yoann on 10/05/2023.
//

#ifndef UDEMY_STRING_H
#define UDEMY_STRING_H
#include <stdbool.h>

int strlen(const char* ptr);
int tonumericdigit(char c);
bool isdigit(char c);
int strnlen(const char* ptr, int max);
char* strcpy(char* dest, const char* src);

#endif //UDEMY_STRING_H
