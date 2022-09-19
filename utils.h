#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
#include "include/dirent.h"
const unsigned int D_TYPE_DIR = 16384;
const unsigned int D_TYPE_FILE = 32768;
#else
#include <dirent.h>
const unsigned int D_TYPE_DIR = 4;
const unsigned int D_TYPE_FILE = 8;
#endif

// reads file from input path &
// returns string of dynamic size ptr
char *read_file(char *input_path);

// writes a file, takes output path
// & strign content (any size)
int write_file(char *output_path, char *content);

// Function to list all the files and folders
// Takes point to dirent object and updates it
//
// {
//    {
//      struct dirent *file1,
//      struct dirent *file2
//    } *files_array
//  } * container
//
// struct dirent ***container pseudocode ^
int list_dir(char *dir_path, struct dirent ***container);

// makes the dir if that
// doesn't exist already
int mkdir_p(char *dir_path);

// check if a path
// (file or dir) exists
int path_exists(char *path);

// frees dirent ptr
// and its content
void free_dirent(struct dirent ***container, size_t count);

#endif