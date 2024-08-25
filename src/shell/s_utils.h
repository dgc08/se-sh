// Don't forget to define S_UTILS_IMPL somewhere!!

#include "se-target.h"
#include <ctype.h>
#include <stddef.h>

#ifndef S_UTILS_H_
#define S_UTILS_H_

#define TODO(msg) do {                                       \
        target_print("[se-sh]: Not implemented / TODO: ");   \
        target_print(msg);                                   \
        target_newline();                                    \
    } while(0);

#ifdef __cplusplus
extern "C" {
#endif

int get_argc(char* str);
char* get_arg(char** str);
const char* trim_left(const char*);
bool str_starts_with(const char* str, const char* prefix);

#ifdef __cplusplus
}
#endif

#ifdef S_UTILS_IMPL

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

const char* trim_left(const char* str) {
    if (str == NULL) return str;
    while (*str != '\0' && isspace(*str))
        str++;
    return str;
}

bool str_starts_with(const char* str, const char* prefix) {
    while(*prefix) {
        if(*prefix++ != *str++)
            return false;
    }
    return true;
}

#endif

#endif // S_UTILS_H_
