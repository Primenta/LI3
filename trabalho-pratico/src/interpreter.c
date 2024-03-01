/**
 * @file interpreter.c
 * @brief Implementation of the command interpreter module.
 */
#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

/**
 * @brief Interprets a command from the input file or a given command line.
 * 
 * @param input The command to interpret.
 * @param res The result array.
 */
void command_interpreter(char *input, char **res) {
    // Initialize the result array with null values
    res[0] = NULL;
    res[1] = NULL;
    res[2] = NULL;

    char *token = strtok(input, " ");

    if (token) {
        // Attempt to extract the query-id (up to 10)
        int len = strlen(token);
        int i;
        for (i = 0; i < len && isdigit(token[i]); i++) {}

        if (i > 0 && i <= 2) {
            // Copy the valid query-id to res[0]
            res[0] = (char *)g_malloc((i + 1) * sizeof(char));
            strncpy(res[0], token, i);
            res[0][i] = '\0';

            // Check if there's a format-flag
            if (token[i] && isalpha(token[i])) {
                // Format-flag is a single alphabet character
                res[1] = (char *)g_malloc(2 * sizeof(char));
                res[1][0] = token[i];
                res[1][1] = '\0';

                // Remaining part is the arguments
                token = strtok(NULL, "\n");
                if (token) {
                    res[2] = g_strdup(token);
                }
            } else {
                // No valid format-flag, treat the rest as arguments
                token = strtok(NULL, "\n");
                if (token) {
                    res[2] = g_strdup(token);
                }
            }
        }
    }
}

/**
 * @brief Frees the memory allocated for the result array.
 * 
 * @param res The result array.
 */
void free_command(char **res) {
    for (int i = 0; i < 3; i++) { // its always 3 because we always have 3 positions in the array
        g_free(res[i]);
    }
}