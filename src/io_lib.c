#include "../headers/io_lib.h"

void sys_print(int fd, char *str, ...) {
    char buf[BUF_SIZE];
    char tmp_buf[BUF_SIZE];
    char c;
    char *tmp_str;
    int i = 0, j;
    va_list arg;
    va_start(arg, str);

    while ((c = *str++) != '\0') {
        switch(c) {
            case '%':
                switch (c = *str++) {
                    case '%':
                        buf[i++] = '%';
                        break;
                    case 'i':
                        sprintf(tmp_buf, "%i", va_arg(arg, int));
                        j = 0;
                        while (tmp_buf[j] != '\0')
                            buf[i++] = tmp_buf[j++];
                        break;
                    case 'l':
                        c = *str++;     // skippo i di %li
                        sprintf(tmp_buf, "%li", va_arg(arg, long));
                        j = 0;
                        while (tmp_buf[j] != '\0')
                            buf[i++] = tmp_buf[j++];
                        break;
                    case 's':
                        tmp_str = va_arg(arg, char *);
                        j = 0;
                        while (tmp_str[j] != '\0')
                            buf[i++] = tmp_str[j++];
                        break;
                }
                break;

            case '\n':
                buf[i++] = '\n';
                break;

            case '\t':
                buf[i++] = '\t'; 
                break;

            default:
                buf[i++] = c;
        }
    }
    
    write(fd, buf, i); 
    va_end(arg);
}