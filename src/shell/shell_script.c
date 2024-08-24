#include "executable_formats.h"
#include "se-sh.h"
#include "s_utils.h"

int exec_shell_script(Container f) {
    char* content = (char*) f.content;
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
        char* line = (char*) trim_left(lines[pc]);
        if (line != NULL && line[0] != '#') {
            code = prompt(line);
            if (code < 0)
                goto f_ret;
        }
        pc++;
    }

    f_ret:
    free(lines);
    if (code < 0)
        return (code+1)*-1;
    else
        return code;
}
