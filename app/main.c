#include "../include/table.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/ui.h"
#include "../include/input.h"

int main() {
    IndexType table_size; 
    if (input(&table_size, "Введите размер таблицы: ") == -1) {
        return 0;
    }
    Table* table = create_table(table_size);
    if (!table) {
        printf("Ошибка: Не удалось инициализировать таблицу\n");
        return 1;
    }
    int choice = 0, status = 1;
    while (status) {
        display_menu();
        int retval = scanf("%d", &choice);
        if (retval == -1) {
            status = 0;
        } else if (retval != 1) {
            scanf("%*[^\n]%*c");
            printf("Ошибка: Такой команды нет!\n");
        } else {
            status = menu_choice(choice, &table);
        }
    }
    free_table(table);
    return 0;
}
