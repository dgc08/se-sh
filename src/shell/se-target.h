#ifndef SE_TARGET_H_
#define SE_TARGET_H_

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        bool echo;
    } Terminal;

    extern Terminal target_terminal;

    int target_system(char* command);
    void target_print(const char* str);
    void target_exit(int code); // shutdown
    void target_print_int(int num);
    void target_newline();
    int target_shell();

    // Files
    typedef struct {
        void* content;
        size_t size;
    } Container;

    size_t target_write_file(const char* filename, Container contents);
    Container target_read_file(const char* filename);
    Container target_list_files();
    bool target_remove_file(const char* filename);

#ifdef __cplusplus
}
#endif


#endif // SE_TARGET_H_
