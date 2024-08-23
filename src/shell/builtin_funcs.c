#include "builtin_funcs.h"

#include "s_utils.h"
#include "se-target.h"
#include "se-sh.h"
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
    Container f = target_read_file(command);
    if (!f.content) {
        target_print("cat: No such file or directory");
        target_newline();
        return 1;
    }

    target_write_output(f.content, f.size);
    free(f.content);
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

int sh_exec(char* args) {
    char* filename = get_arg(&args);

    Container f = target_read_file(filename);

    if (!f.content) {
        target_print("exec: No such file or directory");
        target_newline();
        return 1;
    }

    char* content = (char*)f.content;

    size_t lines_amount = 0;
    for (size_t i = 0; i < f.size; i++) {
        if (content[i] == '\n')
            lines_amount++;
    }

    char** lines = malloc(lines_amount * sizeof(void*));
    size_t j = 0;

    {   // Populate "lines"
        size_t i = 0;
        do {
            lines[j] = content+i;
            while (content[i] != '\n' && i < f.size) {
                i++;
            }
            if (content[i] == '\n')
                content[i] = '\0';
            else
                content[i-1] = '\0';
            i++;
            j++;
        } while (i < f.size);
    }

    size_t pc = 0;
    int code = 0;

    while (pc < lines_amount) {
        target_check_exit_condition();
        if (lines[pc] != NULL) {
            code = prompt(lines[pc]);
            if (code < 0)
                goto f_ret;
        }
        pc++;
    }

    f_ret:
    free(f.content);
    free(lines);
    if (code < 0)
        return (code+1)*-1;
    else
        return code;
}
