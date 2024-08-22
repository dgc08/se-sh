// Don't forget to define S_UTILS_IMPL somewhere!!

#ifndef S_UTILS_H_
#define S_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

int get_argc(char* str);
char* get_arg(char** string);

#ifdef __cplusplus
}
#endif

#ifdef S_UTILS_IMPL
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
#endif

#endif // S_UTILS_H_
