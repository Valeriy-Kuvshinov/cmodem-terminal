#include "../include/config.h"

/* Inner STATIC methods */
/* ==================================================================== */
// Helper function to safely copy config values to arrays
static void set_config_array_value(char *dest, size_t max_len, const char *value, int index,
								   bool update_count) {
	if (index >= 0 && index < MAX_INIT_COMMANDS) {
		strncpy(dest, value, max_len - 1);

		dest[max_len - 1] = NULL_TERMINATOR;

		if (update_count && index >= config.command_count)
			config.command_count = index + 1;
	}
}

// Parse a single config line (key = value)
static bool parse_config_line(char *line) {
	char *equals_pos = strchr(line, '=');

	if (!equals_pos)
		return false;

	*equals_pos = NULL_TERMINATOR;

	char *key = trim_whitespace(line);
	char *value = trim_whitespace(equals_pos + 1);

	// Remove quotes if present
	if (*value == '"' && value[strlen(value) - 1] == '"') {
		value[strlen(value) - 1] = NULL_TERMINATOR;

		value++;
	}

	// Parse baudrate
	if (strcmp(key, CONFIG_BAUDRATE_LINE) == 0)
		config.baudrate = (uint32_t)atoi(value);

	// Parse commands (format: command_N = AT_COMMAND)
	else if (strncmp(key, CONFIG_COMMAND_LINE, CONFIG_COMMAND_LINE_LEN) == 0) {
		int index = atoi(key + CONFIG_COMMAND_LINE_LEN);

		set_config_array_value(config.init_commands[index].command, MAX_COMMAND_LENGTH, value,
							   index, false);
	}
	// Parse descriptions (format: description_N = Description text)
	else if (strncmp(key, CONFIG_DESC_LINE, CONFIG_DESC_LINE_LEN) == 0) {
		int index = atoi(key + CONFIG_DESC_LINE_LEN);

		set_config_array_value(config.init_commands[index].description, MAX_DESCRIPTION_LENGTH,
							   value, index, true);
	}
	return true;
}

/* Outer methods */
/* ==================================================================== */
bool load_config(void) {
	const char *filename = CONFIG_FILE_NAME;
	FILE *file = open_source_file(filename);

	if (!file)
		return false;

	config.command_count = 0;

	char line[MAX_CONFIG_LINE];
	int line_num = 0;

	while (fgets(line, sizeof(line), file)) {
		line_num++;

		// Remove newline
		size_t len = strlen(line);

		if (len > 0 && line[len - 1] == NEWLINE)
			line[len - 1] = NULL_TERMINATOR;

		// Skip comments and empty lines
		if (line[0] == '#' || line[0] == NULL_TERMINATOR || isspace(line[0]))
			continue;

		if (!parse_config_line(line))
			fprintf(stderr, "Warning: Invalid config line %d in file: %s%c", line_num, line,
					NEWLINE);
	}
	safe_fclose(&file);

	return true;
}

const char *get_config_command(int index) {
	if (index < 0 || index >= config.command_count)
		return NULL;

	return config.init_commands[index].command;
}

const char *get_config_description(int index) {
	if (index < 0 || index >= config.command_count)
		return NULL;

	return config.init_commands[index].description;
}
