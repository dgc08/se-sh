#include "utils.h"
#include <stdlib.h>

char* get_arg(char** str) {
    char* ret = *str;

    if (ret != NULL && *ret != '\0') {
        while (*(*str) != ' ' && *(*str) != '\0') {
            (*str)++;
        }
        if (*(*str) == ' ') {
            *(*str) = '\0';
            (*str)++;
        }
    }

    return ret;
}
