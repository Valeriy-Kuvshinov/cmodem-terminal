# Compile flags and directory names
CC = gcc
FLAGS = -std=c99 -pedantic -Wall -g -O2
SRC_DIR = source
INC_DIR = headers

# Program-specific settings
EXEC = modem_terminal
LDFLAGS = -lpthread

# Listen to updates from .h and .c files
HEADERS = $(wildcard $(INC_DIR)/*.h)
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=%.o)

all: $(EXEC)
	@rm -f $(OBJECTS)

$(EXEC): $(OBJECTS)
	@echo "Linking $(EXEC)..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: $(SRC_DIR)/%.c $(HEADERS)
	@echo "Compiling $<..."
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	@rm -f $(EXEC) $(OBJECTS)
	@echo "Cleaned up!"