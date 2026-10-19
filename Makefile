CC = gcc

INCLUDE_DIR = include
SRC_DIR = table_src
APP_DIR = app
LIB_DIR = lib

TABLE_SRC = $(SRC_DIR)/table.c       
MAIN_SRC = $(APP_DIR)/main.c
UI_SRC = $(APP_DIR)/ui.c
INPUT_SRC = $(APP_DIR)/input.c	

LIB_TABLE = $(LIB_DIR)/libtable.a

APP = table_manager

default: vector_static

static:
	$(CC) -c $(TABLE_SRC) -o $(SRC_DIR)/table.o
	ar rcs $(LIB_TABLE) $(SRC_DIR)/table.o
	$(CC) -I $(INCLUDE_DIR) -o $(APP) $(MAIN_SRC) $(UI_SRC) $(INPUT_SRC) $(LIB_TABLE)

shared:
	$(CC) -fPIC -c $(TABLE_SRC) -o $(SRC_DIR)/table.o
	$(CC) -shared -o $(LIB_DIR)/table.so $(SRC_DIR)/table.o
	$(CC) -I $(INCLUDE_DIR) -o $(APP) $(MAIN_SRC) $(UI_SRC) $(INPUT_SRC) -L $(LIB_DIR) -ltable -Wl,-rpath,'$$ORIGIN/$(LIB_DIR)'

run:
	valgrind ./table_manager
clean:
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/* $(APP)

.PHONY: default static shared run clean
