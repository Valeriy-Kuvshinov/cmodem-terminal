# Compile flags and directory names
CC = gcc
FLAGS = -std=c99 -pedantic -Wall -g
SRC_DIR = src
INC_DIR = include

# Program-specific settings
EXEC = modem_terminal

SOURCES = \
    $(SRC_DIR)/modem_terminal.c \
    $(SRC_DIR)/core/modem.c \
    $(SRC_DIR)/core/response_parser.c \
    $(SRC_DIR)/io/serial.c \
    $(SRC_DIR)/threads/thread_utils.c \
    $(SRC_DIR)/threads/modem_thread.c \
    $(SRC_DIR)/threads/input_thread.c \
    $(SRC_DIR)/utils/utils.c

HEADERS = \
    $(INC_DIR)/core/modem.h \
    $(INC_DIR)/core/response_parser.h \
    $(INC_DIR)/io/serial.h \
    $(INC_DIR)/threads/threads.h \
    $(INC_DIR)/utils/utils.h

OBJ_DIR = obj
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

all: $(OBJ_DIR) $(EXEC)
	@rm -rf $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/core $(OBJ_DIR)/io $(OBJ_DIR)/threads $(OBJ_DIR)/utils

$(EXEC): $(OBJECTS)
	@echo "Linking $(EXEC)..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -o $@ $(OBJECTS)

$(OBJ_DIR)/core/%.o: $(SRC_DIR)/core/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/io/%.o: $(SRC_DIR)/io/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/threads/%.o: $(SRC_DIR)/threads/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/utils/%.o: $(SRC_DIR)/utils/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR) $(EXEC)
	@echo "Cleaned up!"