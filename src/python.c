#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "setup.h"

// Function to set up Python environment inside the project directory
void setup_python(const char* project_name) {
    printf("Setting up Python environment...\n");

    // Create the virtual environment inside the project directory
    char venv_cmd[1024];
    snprintf(venv_cmd, sizeof(venv_cmd), "python3 -m venv %s/venv", project_name);
    if (system(venv_cmd) == -1) {
        perror("Error creating virtual environment");
        exit(1);
    }

    // Create requirements.txt inside the project directory
    char requirements_path[1024];
    snprintf(requirements_path, sizeof(requirements_path), "%s/requirements.txt", project_name);
    
    FILE *requirements_file = fopen(requirements_path, "w");
    if (!requirements_file) {
        perror("Error creating requirements.txt");
        exit(1);
    }
    fprintf(requirements_file, "# Add your Python dependencies here\n");
    fclose(requirements_file);
    printf("requirements.txt created inside '%s'.\n", project_name);

    // Create a helper script to activate the virtual environment
    char activate_script_path[1024];
    snprintf(activate_script_path, sizeof(activate_script_path), "%s/activate_venv.sh", project_name);

    FILE *activate_script = fopen(activate_script_path, "w");
    if (!activate_script) {
        perror("Error creating activate_venv.sh");
        exit(1);
    }
    fprintf(activate_script, "#!/bin/bash\n");
    fprintf(activate_script, "source venv/bin/activate\n");
    fclose(activate_script);

    // Make the script executable
    char chmod_cmd[1024];
    snprintf(chmod_cmd, sizeof(chmod_cmd), "chmod +x %s/activate_venv.sh", project_name);
    if (system(chmod_cmd) == -1) {
        perror("Error making activate_venv.sh executable");
        exit(1);
    }

    printf("Python virtual environment created inside '%s/venv'.\n", project_name);
    printf("To activate it, run: source %s/activate_venv.sh\n", project_name);
}
