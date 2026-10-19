#ifndef HANDLERS_H
#define HANDLERS_H

#include "table.h"

void display_menu();
int menu_choice(int, Table**);
const char* error_message(ErrorCode);
void case_insert(Table*);
void case_delete(Table*);
void case_reorganize_table(Table*);
void case_find_key(Table*);
void case_find_version(Table*);
void case_import_table(Table***);
void case_export_table(Table*);

#endif
