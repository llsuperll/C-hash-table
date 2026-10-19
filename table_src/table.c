#include "../include/table.h"
#include "../include/input.h"
#include <string.h>

unsigned long long int hash(char* string) {
    unsigned long long int hash_value = 0, p = 31, p_pow = 1;
    for (int i = 0; string[i] != '\0'; i++) {
        hash_value += (string[i] - 'a' + 1) * p_pow;
        p_pow *= p;
    }
    return hash_value;
}

Table* create_table(IndexType table_size) {
    Table* table = malloc(sizeof(Table));
    if (!table) {
        return NULL;
    }
    table -> ks = malloc(table_size * sizeof(KeySpace*));
    if (!table -> ks) {
        free(table);
        return NULL;
    }
    for (IndexType i = 0; i < table_size; i++) {
        table -> ks[i] = NULL;
    }
    table -> msize = table_size;
    table -> csize = 0;
    return table;
}

ErrorCode insert(Table* table, KeyType key, InfoType info) {
    if (!table || table -> msize == 0) {
        return ERR_TABLE_NOT_EXISTS;
    } else if (!info || !key) {
        return ERR_NULL_INFO;
    }
    unsigned long long int hash_val = hash(key) % table -> msize;
    KeySpace* cur_ks = table -> ks[hash_val];
    KeySpace* new_ks;
    while (cur_ks) {
        if (strcmp(cur_ks -> key, key) == 0) {
            break;
        }
        cur_ks = cur_ks -> link;
    }
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        return ERR_MEMORY;
    }
    new_node -> info = strdup(info);
    new_node -> release = 1;
    new_node -> next = NULL;

    if (!cur_ks) {
        new_ks = malloc(sizeof(KeySpace));
        if (!new_ks) {
            free(new_node -> info);
            free(new_node);
            return ERR_MEMORY;
        }
        new_ks -> key = strdup(key);
        new_ks -> node = new_node;
        new_ks -> link = table -> ks[hash_val];
        table -> ks[hash_val] = new_ks;
        return OK;
    }
    Node* cur_node = cur_ks -> node;
    if (!cur_node) {
        cur_ks -> node = new_node;
        return OK;
    }
    IndexType last_release = 0;
    while (cur_node) {
        last_release = cur_node -> release;
        if (cur_node -> next == NULL) {
            break;
        }
        cur_node = cur_node -> next;
    }
    new_node -> release += last_release;
    cur_node -> next = new_node;
    return OK;
}

ErrorCode delete_key(Table* table, KeyType key) {
    if (!table || table -> msize <= 0) {
        return ERR_TABLE_NOT_EXISTS;
    } else if (!key) {
        return ERR_NULL_INFO;
    }
    unsigned long long int hash_val = hash(key) % table -> msize;
    KeySpace* ks = table -> ks[hash_val];
    KeySpace* prev_ks = NULL;
    while (ks) {
        if (strcmp(ks -> key, key) == 0) {
            break;
        }
        prev_ks = ks;
        ks = ks -> link;
    }
    if (!ks) {
        return ERR_KEY_NOT_FOUND;
    }
    if (!prev_ks) {
        table -> ks[hash_val] = ks -> link;
    } else {
        prev_ks -> link = ks -> link;
    }
    Node* node = ks -> node;
    Node* to_free = NULL;
    while (node) {
        to_free = node;
        node = node -> next;
        free(to_free -> info);
        free(to_free);
    }
    free(ks -> key);
    free(ks);
    return OK;
}

ErrorCode find_key(Table* table, Node*** arr, int* arr_size, KeyType key) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    } else if (!key) {
        return ERR_NULL_INFO;
    }
    unsigned long long int hash_val = hash(key) % table -> msize;
    *arr = NULL;
    *arr_size = 0;
        
    KeySpace* ks = table -> ks[hash_val];
    KeySpace* prev_ks = NULL;
    while (ks) {
        if (strcmp(ks -> key, key) == 0) {
            break;
        }
        prev_ks = ks;
        ks = ks -> link;
    }
    if (!ks) {
        return OK;
    }
    int size = 0;
    Node* node = ks -> node;
    while (node) {
        size++;
        node = node -> next;
    }
    if (size == 0) {
        return OK;
    }
    Node** result = malloc(size * sizeof(Node*));
    if (!result) {
        return ERR_MEMORY;
    }
    node = ks -> node;
    for (int i = 0; i < size; i++) {
        result[i] = node;
        node = node -> next;
    }
    *arr = result;
    *arr_size = size;
    return OK;
}

ErrorCode find_version(Table* table, Node*** arr, KeyType key, RelType release) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    } else if (!key) {
        return ERR_NULL_INFO;
    }
    unsigned long long int hash_val = hash(key) % table -> msize;
    *arr = NULL;
        
    KeySpace* ks = table -> ks[hash_val];
    KeySpace* prev_ks = NULL;
    while (ks) {
        if (strcmp(ks -> key, key) == 0) {
            break;
        }
        prev_ks = ks;
        ks = ks -> link;
    }
    if (!ks) {
        return OK;
    }
    Node* node = ks -> node;
    while (node) {
        if (node -> release == release) {
            break;
        }
        node = node -> next;
    }
    if (!node) {
        return OK;
    }
    Node** result = malloc(sizeof(Node*));
    if (!result) {
        return ERR_MEMORY;
    }
    *arr = result;
    return OK;
}

ErrorCode reorganize_table(Table* table) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    }
    for (IndexType i = 0; i < table -> msize; i++) {
        KeySpace* ks = table -> ks[i];
        while (ks) {
            Node* node = ks -> node;
            Node* to_free = NULL;
            while (node && node -> next) {
                to_free = node;
                node = node -> next;
                free(to_free -> info);
                free(to_free);
            }
            ks -> node = node;
            ks = ks -> link;
        }
    }
    return OK;
}

ErrorCode import_from_binary(Table** table, const char* filename) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    }
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return ERR_FILE;
    }
    int retval;
    IndexType msize;
    retval = fread(&msize, sizeof(IndexType), 1, file);
    if (retval != 1) {
        fclose(file);
        return ERR_FILE;
    }
    Table* new_table = create_table(msize);
    if (!new_table) {
        fclose(file);
        return ERR_MEMORY;
    }
    size_t key_size = 0;
    for (IndexType i = 0; i < new_table -> msize; i++) {
        KeySpace** ks = &(new_table -> ks[i]);
        while (fread(&key_size, sizeof(size_t), 1, file) == 1) {
            if (key_size == 0) {
                break;
            }
            char* key = malloc(key_size * sizeof(char));
            if (!key) {
                fclose(file);
                free_table(new_table);
                return ERR_MEMORY;
            }
            size_t read = fread(key, sizeof(char), key_size, file);
            if (read != key_size) {
                free(key);
                fclose(file);
                free_table(new_table);
                return ERR_FILE_READ;
            }
            KeySpace* new_ks = malloc(sizeof(KeySpace));
            if (!new_ks) {
                free(key);
                free_table(new_table);
                fclose(file);
                return ERR_MEMORY;
            }
            new_ks -> key = key;
            new_ks -> node = NULL;
            new_ks -> link = NULL;
            *ks = new_ks;
            ks = &new_ks -> link;
            
            Node** node = &new_ks -> node;
            RelType release;
            size_t info_size;
            while (fread(&release, sizeof(RelType), 1, file) == 1) {
                if (release == 0) {
                    break;
                }
                retval = fread(&info_size, sizeof(size_t), 1, file);
                if (retval != 1) {
                    free_table(new_table);
                    fclose(file);
                    return ERR_FILE_READ;
                }
                char* info = malloc(info_size * sizeof(char));
                if (!info) {
                    free_table(new_table);
                    fclose(file);
                    return ERR_MEMORY;
                }
                read = fread(info, sizeof(char), info_size, file);
                if (read != info_size) {
                    free(info);
                    free_table(new_table);
                    fclose(file);
                    return ERR_FILE_READ;
                }
                Node* new_node = malloc(sizeof(Node));
                if (!new_node) {
                    free(info);
                    free_table(new_table);
                    fclose(file);
                    return ERR_MEMORY;
                }
                new_node -> info = info;
                new_node -> release = release;
                new_node -> next = NULL;
                
                *node = new_node;
                node = &new_node -> next;
            }
            *node = NULL;
        }
    }
    *table = new_table;
    fclose(file);
    return OK;
}

ErrorCode export_to_binary(Table* table, const char* filename) {
    int retval = 0, retval2 = 0;
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    } else if (!filename) {
        return ERR_FILE;
    }
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return ERR_FILE;
    }
    retval = fwrite(&table -> msize, sizeof(IndexType), 1, file);
    if (retval != 1) {
        fclose(file);
        return ERR_FILE_WRITE;
    }
    for (IndexType i = 0; i < table -> msize; i++) {
        KeySpace* ks = table -> ks[i];
        while (ks) {
            size_t key_size = strlen(ks -> key) + 1;
            if (fwrite(&key_size, sizeof(size_t), 1, file) != 1 || 
                fwrite(ks -> key, sizeof(char), key_size, file) != key_size) {
                fclose(file);
                return ERR_FILE_WRITE;
            }
            Node* node = ks -> node;
            while (node) {
                size_t info_size = strlen(node -> info) + 1;
                retval = fwrite(&(node -> release), sizeof(RelType), 1, file);
                retval2 = fwrite(&info_size, sizeof(size_t), 1, file);
                if (!retval || !retval2) {
                    fclose(file);
                    return ERR_FILE_WRITE;
                }
                if (fwrite(node -> info, sizeof(char), info_size, file) != info_size) {
                    fclose(file);
                    return ERR_FILE_WRITE;
                }
                node = node -> next;
            }
            RelType end = 0;
            retval = fwrite(&end, sizeof(RelType), 1, file);
            if (!retval) {
                fclose(file);
                return ERR_FILE_WRITE;
            }
            ks = ks -> link;
        }
        size_t keyspace_end = 0;
        retval = fwrite(&keyspace_end, sizeof(size_t), 1, file);
        if (!retval) {
            fclose(file);
            return ERR_FILE_WRITE;
        }
    }
    fclose(file);
    return OK;
}

void print_table(Table* table) {
    if (!table) {
        printf("Таблица пуста");
        return;
    }
    for (unsigned int i = 0; i < table -> msize; i++) {
        printf("┌ Индекс %u\n", i);
        KeySpace* ks = table -> ks[i];
        if (!ks) {
            printf("├─ (пусто)\n");
            printf("└────────────────────────\n\n");
            continue;
        }
        while (ks) {
            printf("├─ Ключ: ");
            printf("%s\n", ks -> key);
            Node* node = ks -> node;
            if (!node) {
                printf("│   └─ Нет версий\n");
                printf("└────────────────────────\n\n");
                continue;
            }
            while (node) {
                printf("│   ├─ Версия %d: ", node -> release);
                printf("%s\n", node -> info);
                node = node -> next;
            }
            ks = ks -> link;
            if (ks) {
                printf("├─────────────────\n");
            }
        }
        printf("└────────────────────────\n\n");
    }
}

void free_table(Table* table) {
    if (!table) {
        return;
    }
    for (IndexType i = 0; i < table -> msize; i++) {
        KeySpace* ks = table -> ks[i];
        KeySpace* ks_free = NULL;
        while (ks) {
            ks_free = ks;
            ks = ks -> link;
            if (ks_free) {
                Node* node = ks_free -> node;
                Node* node_free = NULL;
                while (node) {
                    node_free = node;
                    node = node -> next;
                    if (node_free) {
                        free(node_free -> info);
                        node_free -> info = NULL;
                        free(node_free);
                        node_free = NULL;
                    }
                }
                free(ks_free -> key);
                ks_free -> key = NULL;
                free(ks_free);
                ks_free = NULL;
            }
        }
        table -> ks[i] = NULL;
    }
    free(table -> ks);
    table -> ks = NULL;
    free(table);
    table = NULL;
}
