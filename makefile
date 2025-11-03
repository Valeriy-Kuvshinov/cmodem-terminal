# Compile flags and directory names
CC = gcc
FLAGS = -std=c11 -pedantic -Wall -g -O2
SRC_DIR = src
INC_DIR = include

# Program-specific settings
EXEC = modem_terminal

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)

HEADERS = $(wildcard $(INC_DIR)/*.h) $(wildcard $(INC_DIR)/*/*.h)

OBJ_DIR = obj
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

all: $(OBJ_DIR) $(EXEC)
	@rm -rf $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/modem $(OBJ_DIR)/io $(OBJ_DIR)/threads $(OBJ_DIR)/utils $(OBJ_DIR)/globals

$(EXEC): $(OBJECTS)
	@echo "Linking $(EXEC)..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -o $@ $(OBJECTS) -lpthread

$(OBJ_DIR)/modem/%.o: $(SRC_DIR)/modem/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/io/%.o: $(SRC_DIR)/io/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/threads/%.o: $(SRC_DIR)/threads/%.c
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/globals/%.o: $(SRC_DIR)/globals/%.c
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