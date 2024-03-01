/**
 * @file batchMode.h
 * @brief Header file for the batch mode module.
*/
#ifndef BATCHMODE_H
#define BATCHMODE_H

#include "catalog.h"

#define MAX_ARGS 20
#define MAX_COMMAND_SIZE 256 // Tamanho máximo de um comando

void batchMode(char *inputFile, char *outputDirectory, CATALOG *c, int runninTests);
void execute_command(int line, char* command, char* outputDirectory, CATALOG *c, int runninTests);

#endif