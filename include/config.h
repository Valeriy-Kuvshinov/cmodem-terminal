#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "./globals/chars.h"
#include "./utils/file_utils.h"
#include "./utils/utils.h"

#define MAX_CONFIG_LINE 256
#define MAX_COMMAND_LENGTH 64
#define MAX_DESCRIPTION_LENGTH 128
#define MAX_INIT_COMMANDS 16

#define CONFIG_FILE_NAME "app_config.txt"
#define CONFIG_BAUDRATE_LINE "baudrate"
#define CONFIG_COMMAND_LINE "command_"
#define CONFIG_DESC_LINE "description_"

// Command entry structure
typedef struct {
	char command[MAX_COMMAND_LENGTH];
	char description[MAX_DESCRIPTION_LENGTH];
} CommandEntry;

// Configuration structure
typedef struct {
	uint32_t baudrate;
	CommandEntry init_commands[MAX_INIT_COMMANDS];
	int command_count;
} Config;

// Global configuration instance
extern Config config;

// Function prototypes
bool load_config(void);

const char *get_config_command(int index);

const char *get_config_description(int index);

#endif