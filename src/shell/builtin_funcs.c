#include "builtin_funcs.h"

#include "s_utils.h"
#include "se-target.h"
#include <string.h>

Terminal target_terminal;
int atoi(const char *str); // Can't include stdlib
 
int sh_exit(char* args) {
    int code = atoi(args);
    return 0-(code+1);
}

int sh_echo(char* args) {
    if (strcmp(args, "$?") == 0) {
        target_print_int(se_exit_code);
        target_newline();
        return 0;
    }
    target_print(args);
    target_newline();
    return 0;
}

int sh_stty(char* args) {
    if (strcmp(args, "-echo") == 0)
        target_terminal.echo = false;
    if (strcmp(args, "echo") == 0)
        target_terminal.echo = true;
    return 0;
}

int sh_ord(char* args) {
    for (; *args != '\0'; args++) {
        target_print_int(*args);
        target_print(" ");
    }
    target_newline();
    return 0;
}

int sh_sleep(char* args) {
    int millis = atoi(args);
    target_sleep(millis);
    return 0;
}

int sh_cat(char* command) {
    Container contents = target_read_file(command);
    if (!contents.content) {
        target_print("cat: No such file or directory");
        target_newline();
        return 1;
    }

    contents.content = realloc(contents.content, contents.size+1);
    if (!contents.content) {
        target_print("cat: Allocation Error");
        target_newline();
        return 1;
    }
    ((char*)contents.content)[contents.size] = '\0';

    target_print(contents.content);
    free(contents.content);
    return 0;
}

int sh_rm(char* command) {
    if (target_remove_file(command))
        return 0;
    else {
        target_print("rm: No such file or directory");
        target_newline();
        return 1;
    }
}

int sh_ls() {
    Container list = target_list_files();
    for (size_t i = 0; i < list.size; i++) {
        if (((char*)list.content)[i] != '\n') {
            char buf[2] = {((char*)list.content)[i], '\0'};
            target_print(buf);
        }
        else
            target_newline();
    }

    free(list.content);
    return 0;
}

int sh_write(char* command) {
    char* filename = get_arg(&command);
    size_t size = strlen(command);
    command[size] = '\n'; // Replace 0 terminator with newline for file

    size_t w = target_write_file(filename, (Container){command, size+1});
    target_print("Written ");
    target_print_int(w);
    target_print(" bytes.");
    target_newline();
    return 0;
    
}
