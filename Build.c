#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 128

#define COMMENT_OP '~'
#define OUT_DIR_OP '!'
#define OUT_NAME_OP '@'
#define MAIN_FILE_OP '$'
#define CLASS_OP '#'
#define INCLUDE_OP '*'
#define LINK_OP '&'

#define DEBUG_PRINT 0

typedef struct {
    char out_dir[MAX_LENGTH];
    char out_name[MAX_LENGTH];
    char main_file[MAX_LENGTH];
    char classes[10][MAX_LENGTH];
    int class_count;
    char include_dirs[10][MAX_LENGTH];
    int include_count;
    char link_libs[10][MAX_LENGTH];
    int link_count;
} Config;

void parse_config(FILE *file, Config *config) {
    char line[MAX_LENGTH];
    int i;

    // Initialize counts to zero
    config->class_count = 0;
    config->include_count = 0;
    config->link_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == COMMENT_OP) {
            continue; // Skip comments
        }
        char op = line[0];
        switch (op) {
            case OUT_DIR_OP:
                sscanf(line + 1, "%s", config->out_dir);
                break;
            case OUT_NAME_OP:
                sscanf(line + 1, "%s", config->out_name);
                break;
            case MAIN_FILE_OP:
                sscanf(line + 1, "%s", config->main_file);
                break;
            case CLASS_OP:
                while (fgets(line, sizeof(line), file) && line[0] != COMMENT_OP && line[0] != INCLUDE_OP && line[0] != LINK_OP && line[0] != '#') {
                    sscanf(line, "%s", config->classes[config->class_count]);
                    config->class_count++;
                }
                break;
            case INCLUDE_OP:
                while (fgets(line, sizeof(line), file) && line[0] != COMMENT_OP && line[0] != CLASS_OP && line[0] != LINK_OP && line[0] != '*') {
                    sscanf(line, "%s", config->include_dirs[config->include_count]);
                    config->include_count++;
                }
                break;
            case LINK_OP:
                while (fgets(line, sizeof(line), file) && line[0] != COMMENT_OP && line[0] != CLASS_OP && line[0] != INCLUDE_OP && line[0] != '&') {
                    sscanf(line, "%s", config->link_libs[config->link_count]);
                    config->link_count++;
                }
                break;
            default:
                break;
        }
    }
}



void print_config(Config *config) {
    int i;
    printf("Output Directory: %s\n", config->out_dir);
    printf("Output Name: %s\n", config->out_name);
    printf("Main File: %s\n", config->main_file);

    printf("Classes:\n");
    for (i = 0; i < config->class_count; i++) {
        // Remove trailing '$' characters if present
        char *pos = strchr(config->classes[i], '$');
        if (pos != NULL) *pos = '\0';
        printf("  %s\n", config->classes[i]);
    }

    printf("Include Directories:\n");
    for (i = 0; i < config->include_count; i++) {
        printf("  -I%s\n", config->include_dirs[i]);
    }

    printf("Link Libraries:\n");
    for (i = 0; i < config->link_count; i++) {
        printf("  -l%s\n", config->link_libs[i]);
    }
}

int main(int argc, char const *argv[])
{
    FILE *file = fopen("Config.bs", "rb");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    Config config = { "", "", "", {""}, 0, {""}, 0, {""}, 0 };

    parse_config(file, &config);
    fclose(file);

    // Print parsed configuration for debugging
    if (DEBUG_PRINT)
        print_config(&config);

    // Build command
    char command[MAX_LENGTH * 20] = {0};
    strcat(command, "g++ -o ");
    strcat(command, config.out_dir);
    strcat(command, config.out_name);
    strcat(command, " ");
    strcat(command, config.main_file);

    for (int i = 0; i < config.class_count; i++) {
        strcat(command, " ");
        strcat(command, config.classes[i]);
    }

    for (int i = 0; i < config.include_count; i++) {
        strcat(command, " -I");
        strcat(command, config.include_dirs[i]);
    }

    for (int i = 0; i < config.link_count; i++) {
        strcat(command, " -l");
        strcat(command, config.link_libs[i]);
    }

    if(DEBUG_PRINT)
        printf("Build Command: %s\n", command);
    system(command);

    
     char run_command[MAX_LENGTH * 20] = {0};
    strcat(run_command, "\"\"");
    strcat(run_command, config.out_dir);
    strcat(run_command, config.out_name);
    strcat(run_command, "\"\"");

    if (DEBUG_PRINT)
        printf("Run Command: %s\n", run_command);
    system(run_command);
    return EXIT_SUCCESS;
}

