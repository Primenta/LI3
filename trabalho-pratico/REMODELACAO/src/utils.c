/**
 * @file utils.c
 * @brief Some utility functions.
 */
#include "utils.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

/**
 * @brief Puts a string in uppercase.
 * 
 * @param str The string.
 * @return char* The string in uppercase.
 */
char *toupper_str(const char *str) {
    char *str_copy = g_strdup(str);
    char *str_copy_iter = str_copy;
    while (*str_copy_iter) {
        *str_copy_iter = toupper(*str_copy_iter);
        str_copy_iter++;
    }
    return str_copy;
}

// lets make it different so old lines dont get deleted
/**
 * @brief Initializes the error registery (csv file).
 * 
 * @param outputDir The output directory to save the results.
 * @param error_dataset The name of the error dataset.
 * @return FILE* 
 */
FILE *initialize_error_registery(const char *outputDir, const char *error_dataset) {
    // create a csv file to write errors in outputDir
    char error_dataset_path[MAX_PATH_LENGTH];
    // if outputDir ends in /, remove it
    if (outputDir[strlen(outputDir) - 1] == '/') {
        snprintf(error_dataset_path, MAX_PATH_LENGTH, "%s%s", outputDir, error_dataset);
    } else {
        snprintf(error_dataset_path, MAX_PATH_LENGTH, "%s/%s", outputDir, error_dataset);
    }
    FILE *file = fopen(error_dataset_path, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

/**
 * @brief Registers an error line in the error registery (csv file).
 * 
 * @param file The error registery (csv file).
 * @param line The error line to register (line from the input file, its the same line that was read by the interpreter as a command)
 */
void register_error_line(FILE *file, const char *line) {
    // write a new line on the created csv
    fprintf(file, "%s\n", line);
}

/**
 * @brief Closes the error registery (csv file).
 * 
 * @param file The error registery (csv file).
 */
void close_error_registery(FILE *file) {
    fclose(file);
}

// output_buffer
/**
 * @brief Initializes the output buffer.
 * 
 * @param output_buffer The output buffer.
 */
void save_result(int line, const char *res, const char *outputDirectory) {
    char *output_file = g_malloc(sizeof(char) * MAX_BUFFER_SIZE);
    // sprintf(output_file, "%scommand%d_output.txt", outputDirectory, line);
    if (outputDirectory[strlen(outputDirectory) - 1] == '/') {
        snprintf(output_file, MAX_PATH_LENGTH, "%scommand%d_output.txt", outputDirectory, line);
    } else {
        snprintf(output_file, MAX_PATH_LENGTH, "%s/command%d_output.txt", outputDirectory, line);
    }
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        perror("Error opening output file!\n");
        exit(1);
    }
    fprintf(fp, "%s", res);
    fclose(fp);
    g_free(output_file);
}

// a conjunt of functions to save the result of a query line by line
/**
 * @brief Initializes the file saving.
 * 
 * @param line The line of the command.
 * @param outputDirectory The output directory to save the results.
 * @return FILE* 
 */
FILE *initialize_file_saving(int line, const char *outputDirectory) {
    char *output_file = g_malloc(sizeof(char) * MAX_BUFFER_SIZE);
    // sprintf(output_file, "%scommand%d_output.txt", outputDirectory, line);
    if (outputDirectory[strlen(outputDirectory) - 1] == '/') {
        snprintf(output_file, MAX_PATH_LENGTH, "%scommand%d_output.txt", outputDirectory, line);
    } else {
        snprintf(output_file, MAX_PATH_LENGTH, "%s/command%d_output.txt", outputDirectory, line);
    }
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        perror("Error opening output file!\n");
        exit(1);
    }
    g_free(output_file);
    return fp;
}

/**
 * @brief Saves a line in the file.
 * 
 * @param file The file to save the line.
 * @param line The line to save.
 */
void save_line(FILE *file, const char *line) {
    // write a new line on the created csv
    fprintf(file, "%s", line);
}

/**
 * @brief Closes the file saving.
 * 
 * @param file The file to close.
 */
void close_file_saving(FILE *file) {
    fclose(file);
}