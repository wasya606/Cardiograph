#ifndef __sys_conf_H
#define __sys_conf_H
#ifdef __cplusplus
 extern "C" {
#endif

void SystemClock_Config(void);
void _Error_Handler(char *, int);
int _write(int file, char *ptr, int len);


#ifdef __cplusplus
}
#endif
#endif /*__sys_conf_H */
