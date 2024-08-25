// Don't forget to define S_UTILS_IMPL somewhere!!

#include "se-target.h"

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

bool s_isspace(char c);
int get_argc(char* str);
char* get_arg(char** str);
const char* s_trim_left(const char*);
bool str_starts_with(const char* str, const char* prefix);

#ifdef __cplusplus
}
#endif

#ifdef S_UTILS_IMPL

bool s_isspace(char c) {
    return (c == '\t' || c == '\n' ||
            c == '\v' || c == '\f' || c == '\r' || c == ' ' ? 1 : 0);
}


char* get_arg(char** str) {
    char* ret = *str;

    if (ret != NULLPTR && *ret != '\0') {
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

const char* s_trim_left(const char* str) {
    if (str == NULLPTR) return str;
    while (*str != '\0' && s_isspace(*str))
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
