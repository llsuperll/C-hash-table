#include <stdio.h>
#include "../include/table.h"
#include "../include/ui.h"
#include "../include/input.h"

int menu_choice(int choice, Table** table) {
    switch(choice) {
        case 1: 
            case_insert(*table); 
            break;
        case 2: 
            case_delete(*table); 
            break;
        case 3: 
            case_find_key(*table); 
            break;
        case 4: 
            case_find_version(*table); 
            break;
        case 5:
            case_reorganize_table(*table);
        case 6: 
            printf("\n=== Текущее состояние таблицы ===\n");
            print_table(*table);
            break;
        case 7: 
            case_import_table(&table); 
            break;
        case 8:
            case_export_table(*table);
            break;
        case 9:
            return 0;
        default: 
            printf("Ошибка: Такой команды нет!\n");
    }
    return 1;
}

void display_menu() {
    printf(
        "\n=== Топ 8 крутых команд в таблице ===\n"
        "(1) Добавить новый элемент\n"
        "(2) Удалить ключ\n"
        "(3) Поиск всех версий ключа\n"
        "(4) Поиск конкретной версии ключа\n"
        "(5) Реорганизация таблицы\n"
        "(6) Вывод содержимого таблицы\n"
        "(7) Импорт таблицы из файла\n"
        "(8) Экспорт таблицы в файл\n"
        "(9) Выход\n"
        "Выберите действие (1-9): "
    );
}

const char* error_message(ErrorCode code) {
    switch(code) {
        case OK: 
            return "Успешно";
        case ERR_MEMORY: 
            return "Ошибка выделения памяти";
        case ERR_KEY_NOT_FOUND: 
            return "Ключ не найден";
        case ERR_VER_NOT_FOUND: 
            return "Версия не найдена";
        case ERR_FILE: 
            return "Ошибка работы с файлом";
        case ERR_TABLE_NOT_EXISTS: 
            return "Таблицы не существует";
        case ERR_NULL_INFO:
            return "Поле информация не инициализировано";
        case ERR_FILE_WRITE:
            return "Ошибка при записи в файл";
        case ERR_FILE_READ:
            return "Ошибка при чтении из файла";
        default: 
            return "Неизвестная ошибка";
    }
}

void case_insert(Table* table) {
    scanf("%*c");
    KeyType key = my_getline("Введите ключ: ");
    scanf("%*c");
    InfoType info = my_getline("Введите информацию: ");
    ErrorCode err = insert(table, key, info);
    if (err == OK) {
        printf("Элемент успешно добавлен\n");
    } else {
        printf("%s\n", error_message(err));
    }
    if (key) {
        free(key);
    }
    if (info) {
        free(info);
    }
}

void case_delete(Table* table) {
    scanf("%*c");
    KeyType key = my_getline("Введите ключ: ");
    ErrorCode err = delete_key(table, key);
    if (err == OK) {
        printf("Элементы удалены успешно\n");
    } else {
        printf("%s\n", error_message(err));
    }
    if (key) {
        free(key);
    }
}

void case_reorganize_table(Table* table) {
    ErrorCode err = reorganize_table(table);
    if (err == OK) {
        printf("Таблица реорганизована\n");
    } else {
        printf("%s\n", error_message(err));
    }
}

void case_find_key(Table* table) {
    int k;
    scanf("%*c");
    KeyType key = my_getline("Введите ключ: ");
    Node** result;
    ErrorCode err = find_key(table, &result, &k, key);
    if (err == OK) {
        printf("Ключ %s:\n", key);
        for (int i = 0; i < k; i++) {
            printf("Версия %d: %s\n", result[i] -> release, result[i] -> info);
        }
        free(result);
    } else {
        printf("%s\n", error_message(err));
    }
    if (key) {
        free(key);
    }
}

void case_find_version(Table* table) {
    scanf("%*c");
    KeyType key = my_getline("Введите ключ: ");
    scanf("%*c");
    RelType release;
    if (input(&release, "Введите версию: ") == -1) {
        return;
    }
    Node** result;
    ErrorCode err = find_version(table, &result, key, release);
    if (err == OK) {
        printf("Ключ %s, Версия %d: %s\n", key, result[0] -> release, result[0] -> info);
        free(result);
    } else {
        printf("%s\n", error_message(err));
    }
    if (key) {
        free(key);
    }
}

void case_import_table(Table*** table) {
    scanf("%*c");
    char* filename = my_getline("Введите имя файла: ");
    Table* new_table;
    ErrorCode err = import_from_binary(&new_table, filename);
    print_table(new_table);
    if (err == OK) {
        if (**table) {
            free_table(**table);
        }
        **table = new_table;
        printf("Данные успешно импортированы\n");
    } else {
        printf("%s\n", error_message(err));
    }
    if (filename) {
        free(filename);
    }
}

void case_export_table(Table* table) {
    scanf("%*c");
    char* filename = my_getline("Введите имя файла: ");

    ErrorCode err = export_to_binary(table, filename);
    if (err == OK) {
        printf("Данные успешно экспортированы");
    } else {
        printf("%s\n", error_message(err));
    }
    if (filename) {
        free(filename);
    }
}
