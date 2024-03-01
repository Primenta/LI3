/**
 * @file utils.h
 * @brief Header file for the utility functions.
 */
#ifndef UTILS_H
#define UTILS_H

#define MAX_BUFFER_SIZE 1024

#include <stdio.h>

char *toupper_str(const char *str);
FILE *initialize_error_registery(const char *outputDir, const char *error_dataset);
void register_error_line(FILE *file, const char *line);
void close_error_registery(FILE *file);
void save_result(int line, const char *res, const char *outputDirectory);
FILE *initialize_file_saving(int line, const char *outputDirectory);
void save_line(FILE *file, const char *line);
void close_file_saving(FILE *file);

#endif