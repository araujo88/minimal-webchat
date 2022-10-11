CC=gcc
CC_FLAGS=-g -Wall
CC_LIBS=-lpthread

SRC_DIR=src
HDR_DIR=include
OBJ_DIR=obj

# source and object files
SRC_FILES1 = $(filter-out src/client.c, $(wildcard src/*.c))
OBJ_FILES1=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES1))
BIN_FILE1=server

SRC_FILES2 = $(filter-out src/server.c, $(wildcard src/*.c))
OBJ_FILES2=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES2))
BIN_FILE2=client

all: $(OBJ_DIR) $(BIN_FILE1) $(BIN_FILE2)

$(BIN_FILE1): $(OBJ_FILES1)
	$(CC) $(CC_FLAGS) $^ -I$(HDR_DIR) -o $@ $(CC_LIBS)

$(BIN_FILE2): $(OBJ_FILES2)
	$(CC) $(CC_FLAGS) $^ -I$(HDR_DIR) -o $@ $(CC_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CC_FLAGS) -c $^ -I$(HDR_DIR) -o $@ $(LFLAGS)

$(OBJ_DIR):
	mkdir $@

clean:
	rm -rf $(BIN_FILE) $(OBJ_DIR)