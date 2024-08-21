#include "utils.h"
#include <stdlib.h>

char* get_arg(char** str) {
    char* ret = *str;

    if (ret != NULL) {
        while (*(*str) != ' ' && *(*str) != '\0') {
            (*str)++;
        }
        *(*str) = '\0';
        (*str)++;
    }

    return ret;
}
