#ifndef SETUP_H
#define SETUP_H

// Detects the language based on the working directory
const char* detect_language(const char* path);

// Sets up a Python virtual environment
void setup_python();

// Sets up Git repository
void setup_git(const char* project_name, const char* project_path);

// Creates the Git update directory if it doesn't exist
void create_git_update_dir();

#define GIT_UPDATE_DIR "/usr/local/share/gitupdate"

#endif

