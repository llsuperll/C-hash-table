#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>

typedef char* KeyType;
typedef unsigned int RelType;
typedef char* InfoType;
typedef unsigned int IndexType;

typedef enum {
    OK = 0,
    ERR_MEMORY = -1,
    ERR_VER_NOT_FOUND = -2,
    ERR_KEY_NOT_FOUND = -3,
    ERR_FILE = -4,
    ERR_TABLE_NOT_EXISTS = -5,
    ERR_NULL_INFO = -6,
    ERR_FILE_WRITE = -7,
    ERR_FILE_READ = -8,
} ErrorCode;

typedef struct Node {
    RelType release;
    InfoType info;
    struct Node* next;
} Node;

typedef struct KeySpace {
    KeyType key;
    Node* node;
    struct KeySpace* link;
} KeySpace;

typedef struct Table {
    KeySpace** ks;
    IndexType msize;
    IndexType csize;
} Table;

Table* create_table(IndexType);
ErrorCode insert(Table*, KeyType, InfoType);
ErrorCode delete_key(Table*, KeyType);
ErrorCode find_key(Table*, Node***, int*, KeyType);
ErrorCode find_version(Table*, Node***, KeyType, RelType);
ErrorCode import_from_binary(Table**, const char*);
ErrorCode export_to_binary(Table*, const char*);
ErrorCode reorganize_table(Table*);
void print_table(Table*);
void free_table(Table*);
#endif
