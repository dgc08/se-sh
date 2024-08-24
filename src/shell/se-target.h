#ifndef SE_TARGET_H_
#define SE_TARGET_H_

#include "s_types.h"

#ifdef __cplusplus
extern "C" {
#endif
    // General IO
    typedef struct {
        bool echo;
    } Terminal;

    extern Terminal target_terminal;

    int target_system(char* command); // This is for exposing platform-specific functions
    int target_shell();               // The REPL itself
    void target_check_exit_condition();

    void target_write_output(const char* data, size_t nbytes); // write(STDOUT_FILENO, ...
    void target_print(const char* str);
    void target_exit(int code); // shutdown
    void target_print_int(int num);
    void target_newline();
    void target_sleep(uint32_t millis);

    // stdlib functions which need to be implemented one way or another
    void *malloc(size_t);
    void *realloc(void *, size_t);
    void free(void *);

    char *strcpy (char *__restrict, const char *__restrict);
    size_t strlen (const char *);
    int strcmp (const char *, const char *);

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
