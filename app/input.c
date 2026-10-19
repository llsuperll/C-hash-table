#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* my_getline(const char* prompt) {
    char buf[81] = {0};
    char* res = NULL;
    int len = 0, n = 0;
    
    printf("%s", prompt);
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            if (!res) {
                return NULL;
            }
        } else if (n > 0) {
            int chunk_len = strlen(buf);
            char* new_res = realloc(res, len + chunk_len + 1);
            if (new_res == NULL) {
                free(res);
                return NULL;
            }
            res = new_res;
            memcpy(res + len, buf, chunk_len);
            len += chunk_len;
        } else {
            n = scanf("%80[^\n]", buf);
        }
    } while (n > 0);

    if (len > 0) {
        res[len] = '\0';
    } else {
        res = calloc(1, sizeof(char));
        if (res == NULL) {
            return NULL;
        }
    }
    return res;
}

int input(int* value, const char* prompt) {
    int retval = 0, flag = 0;
    printf("%s", prompt);
    retval = scanf("%u", value);
    while (!flag) {
        if (retval == -1) {
            return -1;
        } else if (retval != 1) {
            printf("Ошибка: введено не число\n");
        } else if (*value <= 0) {
            printf("Введённое число должно быть больше 0\n");
        } else {
            int check = getchar();
            if (check != '\n' && check != EOF) {
                printf("Ошибка: введены лишние символы\n");
            } else {
                flag = 1;
                continue;
            }
        }
        scanf("%*[^\n]%*c");
        printf("%s", prompt);
        retval = scanf("%d", value);
    }
    return 0;
}
