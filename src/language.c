#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h> // For tolower()
#include <limits.h>

#define CONFIG_DIR "/.config/setup"
#define LANGUAGE_FILE "/language.txt"
#define DEFAULT_LANGUAGES "python,python3\nC\ncpp,c++\njava\njavascript\nassembly,asm\nrust\ngo\nruby"
#define MAX_PATH_LEN 2048 // Increased buffer size to avoid truncation

// Helper function to create the configuration directory and file if they don't exist
void create_language_config_file(const char* config_path, const char* file_path) {
    struct stat st = {0};
    if (stat(config_path, &st) == -1) {
        mkdir(config_path, 0700); // Create the directory with full user permissions
    }

    FILE *file = fopen(file_path, "r");
    if (!file) {
        file = fopen(file_path, "w");
        if (file) {
            fprintf(file, "%s", DEFAULT_LANGUAGES);
            printf("Language configuration file created and populated with default languages.\n");
        }
    }
    fclose(file);
}

// Helper function to convert a string to lowercase
void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Reads the language configuration file and stores languages in a dynamically allocated array
char** load_languages(const char* file_path, int* lang_count) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening language file");
        return NULL;
    }

    char line[256];
    char** languages = NULL;
    *lang_count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        char *token = strtok(line, ",");

        while (token != NULL) {
            languages = realloc(languages, sizeof(char*) * (*lang_count + 1));
            if (!languages) {
                perror("Memory allocation error");
                fclose(file);
                return NULL;
            }
            languages[*lang_count] = strdup(token); // Copy token
            if (!languages[*lang_count]) {
                perror("String duplication error");
                fclose(file);
                return NULL;
            }

            // Convert the language to lowercase
            to_lowercase(languages[*lang_count]);
            (*lang_count)++;
            token = strtok(NULL, ",");
        }
    }

    fclose(file);
    return languages;
}

// Function to check if a segment matches any known languages
const char* check_language_in_segment(const char* segment, char** languages, int lang_count) {
    printf("Checking segment: '%s'\n", segment); // Debugging: Print each segment being checked

    char lower_segment[256];
    strncpy(lower_segment, segment, sizeof(lower_segment));
    lower_segment[sizeof(lower_segment) - 1] = '\0'; // Ensure null termination

    // Convert the segment to lowercase
    to_lowercase(lower_segment);

    for (int i = 0; i < lang_count; i++) {
        printf("Comparing with language: '%s'\n", languages[i]); // Debugging: Print each language being compared
        if (strcmp(lower_segment, languages[i]) == 0) {
            printf("Match found: '%s'\n", languages[i]); // Debugging: Print matched language
            return languages[i]; // Return the matched language
        }
    }
    return NULL;
}

// Helper function to get the parent directory path
void get_parent_directory(char* path) {
    char* last_slash = strrchr(path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0'; // Remove the last directory segment
    }
}

// Function to detect the language based on the working directory path
const char* detect_language(const char* start_path) {
    // Get the HOME environment variable
    char* home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "Could not get HOME directory\n");
        return NULL;
    }

    // Define config and file paths
    char config_path[MAX_PATH_LEN], file_path[MAX_PATH_LEN];
    snprintf(config_path, sizeof(config_path), "%s%s", home, CONFIG_DIR);
    
    // Check if the combined path will fit within the buffer
    if (snprintf(file_path, sizeof(file_path), "%s%s", config_path, LANGUAGE_FILE) >= sizeof(file_path)) {
        fprintf(stderr, "File path too long\n");
        return NULL;
    }

    // Ensure the config file exists and populate it if necessary
    create_language_config_file(config_path, file_path);

    // Load languages from the config file
    int lang_count;
    char** languages = load_languages(file_path, &lang_count);
    if (!languages) {
        return NULL;
    }

    // Start searching from the provided path and move upward
    char current_path[MAX_PATH_LEN];
    strncpy(current_path, start_path, MAX_PATH_LEN - 1);
    current_path[MAX_PATH_LEN - 1] = '\0'; // Ensure null termination

    char* detected_language = NULL;
    while (strcmp(current_path, home) != 0) { // Stop when reaching the home directory
        // Extract the last segment of the path and check for a language match
        char* last_segment = strrchr(current_path, '/');
        if (last_segment != NULL) {
            last_segment++; // Move past the '/' to get the directory name
            const char* match = check_language_in_segment(last_segment, languages, lang_count);
            if (match != NULL) {
                detected_language = strdup(match); // Copy the matched language to prevent memory issues
                break; // Exit loop once a language is detected
            }
        }

        // Move up one directory level
        get_parent_directory(current_path);

        // If the path is empty (root directory), break
        if (strlen(current_path) == 0) {
            break;
        }
    }

    // Cleanup
    for (int i = 0; i < lang_count; i++) {
        free(languages[i]); // Free allocated language strings
    }
    free(languages); // Free language array

    return detected_language; // Return detected language or NULL
}
