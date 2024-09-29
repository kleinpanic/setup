#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "setup.h"

// Function to create a project directory
int create_project_dir(const char *dir_name) {
    if (mkdir(dir_name, 0700) == -1) {
        perror("Error creating project directory");
        return 1;
    }
    printf("Project directory '%s' created.\n", dir_name);
    return 0;
}

// Function to create additional directories for C projects
void create_c_project_structure(const char* project_name) {
    // Paths for build, obj, include, src directories
    char build_dir[1024], obj_dir[1024], include_dir[1024], src_dir[1024], readme_file[1024];
    
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_name);
    snprintf(obj_dir, sizeof(obj_dir), "%s/obj", project_name);
    snprintf(include_dir, sizeof(include_dir), "%s/include", project_name);
    snprintf(src_dir, sizeof(src_dir), "%s/src", project_name);
    snprintf(readme_file, sizeof(readme_file), "%s/README.md", project_name);
    
    // Create directories
    mkdir(build_dir, 0700);
    mkdir(obj_dir, 0700);
    mkdir(include_dir, 0700);
    mkdir(src_dir, 0700);
    
    // Create README.md file
    FILE *readme = fopen(readme_file, "w");
    if (readme) {
        fprintf(readme, "# %s Project\n\nThis is a C project generated with the setup tool.\n", project_name);
        fclose(readme);
        printf("README.md file created in '%s'.\n", project_name);
    } else {
        perror("Error creating README.md");
    }

    printf("C project structure created with directories: build, obj, include, src\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <project_name> [--language <language_name>]\n", argv[0]);
        return 1;
    }

    const char *project_name = argv[1];
    const char *language = NULL;

    if (argc == 4 && strcmp(argv[2], "--language") == 0) {
        language = argv[3];  // Get the language from the argument
    } else {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            return 1;
        }

        language = detect_language(cwd);

        // If no language is detected, prompt the user to rerun the setup
        if (language == NULL) {
            printf("Could not detect the coding language from the current directory.\n");
            printf("Please rerun the setup with --language <language_name> after the project name.\n");
            return 1;
        }
    }

    printf("Detected/selected language: %s\n", language);

    // Create project directory
    if (create_project_dir(project_name) != 0) {
        return 1;
    }

    // Perform setup based on the detected or specified language
    if (strcmp(language, "python") == 0 || strcmp(language, "python3") == 0) {
        setup_python(project_name);
    } else if (strcmp(language, "c") == 0) {
        create_c_project_structure(project_name);
    } else {
        printf("Unsupported language specified. Please rerun the setup with either 'python' or 'c'.\n");
        return 1;
    }

    // Git setup
    setup_git(project_name, project_name);  // Assuming project path is the project name

    return 0;
}
