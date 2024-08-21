#ifndef SE_TARGET_H_
#define SE_TARGET_H_

#ifdef __cplusplus
extern "C" {
#endif

int target_system(char* command);
void target_print(const char* str);
void target_exit(int code);
void target_print_int(int num);

#ifdef __cplusplus
}
#endif

int target_shell();

#endif // SE_TARGET_H_
