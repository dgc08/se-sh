#include "executable_formats.h"
#include "se-sh.h"
#include "s_utils.h"
#include "se-target.h"

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
        char* original = (char*) s_trim_left(lines[pc]);
        char* line = (char*) malloc(strlen(original) + 1); // Allocate memory for the string including the null terminator
        strcpy(line, original);
        original = line;


        if (str_starts_with(line, "goto")) {
            get_arg(&line);
            char* label = get_arg(&line);
            if (*label == '\0'){
                target_print("Exec shell script 'goto': No label provided");
                target_newline();
                goto loop_continue;
            }
            label[-1] = ':';
            label--;

            for (size_t i = 0; i < lines_amount; i++) {
                if (strcmp(label, s_trim_left(lines[i])) == 0) {
                    pc = i;
                    goto loop_continue;
                }
            }
            target_print("Unknown label: ");
            target_print(label+1);
        }
        else if (line != NULLPTR && line[0] != '#' && line[0] != ':') { // a command or empty line
            code = prompt(line);
            if (code < 0)
                goto f_ret;
        }
        pc++;
        loop_continue:
        free(original);
    }

    f_ret:
    free(lines);
    if (code < 0)
        return (code+1)*-1;
    else
        return code;
}
