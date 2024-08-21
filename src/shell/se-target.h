#ifndef SE_TARGET_H_
#define SE_TARGET_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        bool echo;
    } Terminal;

    int target_system(char* command);
    void target_print(const char* str);
    void target_exit(int code); // shutdown
    void target_print_int(int num);
    int target_shell();

#ifdef __cplusplus
}
#endif


#endif // SE_TARGET_H_
