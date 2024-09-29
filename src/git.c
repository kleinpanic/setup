#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "setup.h"

#define CONFIG_DIR "/.config/setup"
#define AUTOGITUPDATE_FILE "/autogitupdate.txt"
#define CONFIG_FILE "/config.config"
#define MAX_PATH_LEN 4096

// Declare the function prototype for is_merge_in_progress
int is_merge_in_progress(const char* project_dir);

// Function to check if autogit is enabled
int is_autogit_enabled() {
    char config_path[MAX_PATH_LEN];
    snprintf(config_path, sizeof(config_path), "%s%s", getenv("HOME"), CONFIG_DIR CONFIG_FILE);

    FILE *config_file = fopen(config_path, "r");
    if (!config_file) {
        perror("Error opening config.config");
        return 0; // Default to false if the file cannot be read
    }

    char line[256];
    while (fgets(line, sizeof(line), config_file)) {
        if (strstr(line, "autogit=true")) {
            fclose(config_file);
            return 1; // autogit=true found
        }
    }

    fclose(config_file);
    return 0; // autogit=false or not found
}

// Function to check if a merge is in progress
int is_merge_in_progress(const char* project_dir) {
    char merge_head_path[MAX_PATH_LEN];
    snprintf(merge_head_path, sizeof(merge_head_path), "%s/.git/MERGE_HEAD", project_dir);

    struct stat st;
    if (stat(merge_head_path, &st) == 0) {
        return 1;  // Merge in progress
    }
    return 0;  // No merge in progress
}

// Function to check if the current directory is part of an existing Git repository
int is_git_repo() {
    return system("git rev-parse --is-inside-work-tree > /dev/null 2>&1") == 0;
}

// Function to set up the git repository
void setup_git(const char* project_name, const char* project_path) {
    if (!is_autogit_enabled()) {
        printf("Git setup skipped (autogit=false).\n");
        return;
    }

    // Check if the current directory is already part of a Git repository
    if (is_git_repo()) {
        printf("Directory is already part of a Git repository. Skipping Git setup.\n");
        return;
    }

    printf("Setting up Git...\n");

    // Initialize Git repository inside the project directory
    char git_init_cmd[MAX_PATH_LEN];
    snprintf(git_init_cmd, sizeof(git_init_cmd), "cd %s && git init", project_name);
    if (system(git_init_cmd) == -1) {
        perror("Error initializing Git repository");
        exit(1);
    }

    // Add all files to the Git repository without committing
    char git_add_cmd[MAX_PATH_LEN];
    snprintf(git_add_cmd, sizeof(git_add_cmd), "cd %s && git add .", project_name);
    if (system(git_add_cmd) == -1) {
        perror("Error adding files to Git repository");
        exit(1);
    }

    // Check if a merge is in progress and handle conflicts only if necessary
    if (is_merge_in_progress(project_name)) {
        printf("Merge in progress, attempting to resolve...\n");
        char git_merge_abort_cmd[MAX_PATH_LEN];
        snprintf(git_merge_abort_cmd, sizeof(git_merge_abort_cmd), "cd %s && git merge --abort", project_name);
        if (system(git_merge_abort_cmd) == -1) {
            perror("Error aborting Git merge");
        } else {
            printf("Merge conflict resolved.\n");
        }
    }

    // Resolve the full path of the project directory using realpath
    char full_project_path[MAX_PATH_LEN];
    if (realpath(project_name, full_project_path) == NULL) {
        perror("Error resolving full path of project directory");
        exit(1);
    }

    // Add the full project path to ~/.config/setup/autogitupdate.txt
    create_git_update_dir();  // Ensure the config directory and file exist

    char git_update_path[MAX_PATH_LEN];
    if (snprintf(git_update_path, sizeof(git_update_path), "%s%s", getenv("HOME"), CONFIG_DIR AUTOGITUPDATE_FILE) >= sizeof(git_update_path)) {
        fprintf(stderr, "Error: Path too long\n");
        exit(1);
    }

    FILE *file = fopen(git_update_path, "a");
    if (file == NULL) {
        perror("Error opening autogitupdate.txt");
        exit(1);
    }

    // Append the full project path to autogitupdate.txt
    fprintf(file, "%s\n", full_project_path);
    fclose(file);

    printf("Git repository initialized, and path added to ~/.config/setup/autogitupdate.txt.\n");
}

// Function to create the ~/.config/setup directory if it doesn't exist
void create_git_update_dir() {
    char config_dir[MAX_PATH_LEN];
    snprintf(config_dir, sizeof(config_dir), "%s%s", getenv("HOME"), CONFIG_DIR);

    struct stat st = {0};
    if (stat(config_dir, &st) == -1) {
        if (mkdir(config_dir, 0700) == -1) {
            perror("Error creating ~/.config/setup directory");
            exit(1);
        }
    }

    // Check if the autogitupdate.txt file exists; create it if not
    char git_update_path[MAX_PATH_LEN];
    if (snprintf(git_update_path, sizeof(git_update_path), "%s%s", config_dir, AUTOGITUPDATE_FILE) >= sizeof(git_update_path)) {
        fprintf(stderr, "Error: Path too long\n");
        exit(1);
    }

    FILE *file = fopen(git_update_path, "a");
    if (file == NULL) {
        perror("Error creating autogitupdate.txt");
        exit(1);
    }
    fclose(file);
}

