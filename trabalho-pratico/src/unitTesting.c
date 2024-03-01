/**
* @file unitTesting.c
* @brief Source file for the unit testing module.
*/
#include "unitTesting.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

void run_unit_tests(char *outputDir, char *expectedOutputDir) {
    // first lets make a check on the path, if they dont end in / we add it
    if (outputDir[strlen(outputDir) - 1] != '/') {
        strcat(outputDir, "/");
    }
    if (expectedOutputDir[strlen(expectedOutputDir) - 1] != '/') {
        strcat(expectedOutputDir, "/");
    }
    // for each file inside expectedOutputDir we need to have a corresponding file inside outputDir
    // and the content of the files must be the same
    DIR *dir = opendir(expectedOutputDir);
    if (dir == NULL) {
        printf("Error opening directory %s\n", expectedOutputDir);
        exit(1);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // we need to check if the file exists in outputDir
        char *expectedOutputFilePath = g_malloc(strlen(expectedOutputDir) + strlen(entry->d_name) + 1);
        strcpy(expectedOutputFilePath, expectedOutputDir);
        strcat(expectedOutputFilePath, entry->d_name);
        char *outputFilePath = g_malloc(strlen(outputDir) + strlen(entry->d_name) + 1);
        strcpy(outputFilePath, outputDir);
        strcat(outputFilePath, entry->d_name);
        FILE *expectedOutputFile = fopen(expectedOutputFilePath, "r");
        FILE *outputFile = fopen(outputFilePath, "r");
        if (outputFile == NULL) {
            printf("File %s does not exist in %s\n", entry->d_name, outputDir);
            exit(1);
        }
        if (expectedOutputFile == NULL) {
            printf("File %s does not exist in %s\n", entry->d_name, expectedOutputDir);
            exit(1);
        }
        /*printf("Reading expected output file %s\n", expectedOutputFilePath);
        printf("Reading output file %s\n", outputFilePath);*/
        // count the number of lines in the file (so we can allocate the memory for the strings)
        int lines = 0;
        char c;
        while ((c = fgetc(expectedOutputFile)) != EOF) {
            if (c == '\n') {
                lines++;
            }
        }
        //printf("Number of lines: %d for file %s\n", lines, expectedOutputFilePath);
        // now we need to compare the content of the files
        char **expectedOutput = malloc(lines * sizeof(char *));
        char **output = malloc(lines * sizeof(char *));
        for (int i = 0; i < lines; i++) {
            expectedOutput[i] = malloc(1000);
            output[i] = malloc(1000);
        }
        rewind(expectedOutputFile);
        rewind(outputFile);
        int line = 0;
        // run through the files and compare the lines (independent if the file has lines (is null or not), if it is empty and should have something, it counts as a difference)
        while (1) {
            char *expectedOutputLine = fgets(expectedOutput[line], 1000, expectedOutputFile);
            char *outputLine = fgets(output[line], 1000, outputFile);
            //printf("Expected: %s\nGot: %s\n", expectedOutput[line], output[line]);
            if (expectedOutputLine == NULL && outputLine == NULL) {
                // we reached the end of both files
                break;
            }
            if (expectedOutputLine == NULL || outputLine == NULL) {
                // one of the files has less lines than the other
                printf("Difference in file %s at line %d\n", entry->d_name, line);
                printf("Expected: %sGot: %s\n\n", expectedOutputLine, outputLine ? outputLine : "(empty line)");
                break; // we can remove this if we want to check all the lines
            }
            // Trim newline character from expectedOutput
            if (expectedOutput[line][strlen(expectedOutput[line]) - 1] == '\n') {
                expectedOutput[line][strlen(expectedOutput[line]) - 1] = '\0';
            }
            // Trim newline character from output
            if (output[line][strlen(output[line]) - 1] == '\n') {
                output[line][strlen(output[line]) - 1] = '\0';
            }
            if (strcmp(expectedOutput[line], output[line]) != 0) {
                printf("Difference in file %s at line %d\n", entry->d_name, line);
                printf("Expected: %s\nGot: %s\n\n", expectedOutput[line], output[line]);
                //printf("Line %d in %s and line %d in %s\n", line, expectedOutputFilePath, line, outputFilePath);
            }
            line++;
        }
        fclose(expectedOutputFile);
        fclose(outputFile);
        g_free(expectedOutputFilePath);
        g_free(outputFilePath);
    }
    closedir(dir);
    printf("All unit tests done!\n");
}