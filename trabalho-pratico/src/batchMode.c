/**
 * @file batchMode.c
 * @brief Implementation of the batch mode.
 */
#include "batchMode.h"
#include "interpreter.h"
#include "queries.h"
#include "utils.h"

#include <time.h>

void log_ctime(clock_t start, clock_t end, int query_id, int runninTests) {
    if (runninTests) {
        double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
        printf("Query %d executed in time: %fs\n", query_id, time_taken);
    }
}

/**
 * @brief Executes a command from the input file or a given command line.
 * 
 * @param line The line number of the command (command ID/number)
 * @param command The command to execute
 * @param outputDirectory The output directory to save the result
 * @param c The passed catalog @see struct CATALOG
 * @param runninTests Flag to indicate if we are running tests or not
 */
void execute_command(int line, char* command, char* outputDirectory, CATALOG *c, int runninTests) {
	char* result_command[3];
	command_interpreter(command, result_command);
    /*printf("Line: %d\n", line);
	printf("Query ID: %s\n", result_command[0]);
	printf("Format flag: %s\n", result_command[1]);
	printf("Args: %s\n", result_command[2]);*/
	int query_id = atoi(result_command[0]);
    // if format_flag is not empty/null, we return 1
    int format_flag = (result_command[1] != NULL && strcmp(result_command[1], "") != 0);
    // if we got args we return an array of strings (each string is an arg, they are separated by a space)
    // lets start by getting the args by splitting the arg string by a space
    char **args = g_malloc(sizeof(char*) * MAX_ARGS);
    int args_size = 0;
    if (result_command[2] != NULL && strcmp(result_command[2], "") != 0) {
        // Flag to track whether we are inside quotes
        int inside_quotes = 0;

        // Pointer to keep track of the current position in the input string
        char* token = result_command[2];

        while (*token != '\0' && args_size < MAX_ARGS) {          
            // Skip leading spaces
            while (*token == ' ') {
                token++;
            }

            // Check if the token contains quotes
            if (*token == '\"') {
                inside_quotes = 1;
                // Skip the opening quote
                token++;
            }

            // Set the current argument pointer
            args[args_size++] = token;

            // Move to the next space or the closing quote
            while (*token != '\0' && (*token != ' ' || inside_quotes)) {
                if (*token == '\"') {
                    inside_quotes = !inside_quotes;
                }
                token++;
            }

            // Replace the space or closing quote with a null terminator
            if (*token != '\0') {
                *token = '\0';
                token++;
            }

            // remove the last '"' if it exists
            if (args[args_size - 1][strlen(args[args_size - 1]) - 1] == '\"') {
                args[args_size - 1][strlen(args[args_size - 1]) - 1] = '\0';
            }
        }
    }
    /*printf("Args size: %d\n", args_size);
    printf("Args: ");
    for (int i = 0; i < args_size; i++) {
        printf("%s ", args[i]);
    }*/
    clock_t query_start, query_end;
	switch (query_id) {
		case 1:
            query_start = clock();
            char* res_1 = query_1(c, format_flag, args, args_size);
            save_result(line, res_1, outputDirectory);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            //printf("query_1 output: %s\n", res_1);
            g_free(res_1);
			break;
        case 2:
            GList *res_2 = NULL;
            query_start = clock();
            query_2(c, format_flag, args, args_size, &res_2);
            FILE *save_result_2 = initialize_file_saving(line, outputDirectory);
            GList *iterList2 = res_2;
            //printf("Data for command %d:\n", line);
            while (iterList2 != NULL) {
                char *str = iterList2->data;
                //printf("%s", str);
                save_line(save_result_2, str);
                g_free(str);
                iterList2 = g_list_next(iterList2);
            }
            close_file_saving(save_result_2);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            g_list_free(res_2);
            break;
        case 3:
            query_start = clock();
            char *res_3 = query_3(c, format_flag, args, args_size);
            save_result(line, res_3, outputDirectory);
            //printf("query_3 output: %s\n", res_3);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            g_free(res_3);
            break;
        case 4:
            GList *res_4 = NULL;
            query_start = clock();
            query_4(c, format_flag, args, args_size, &res_4);
            FILE *save_result_4 = initialize_file_saving(line, outputDirectory);
            // Iterate through the list and save the result
            GList *iterList4 = res_4;
            //printf("Reservations for command %d:\n", line);
            while (iterList4 != NULL) {
                char *str = iterList4->data;
                //printf("%s", str);
                save_line(save_result_4, str);
                g_free(str);
                iterList4 = g_list_next(iterList4);
            }
            close_file_saving(save_result_4);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            // Free the list
            g_list_free(res_4);
            break;
        case 5:
            GList *res_5 = NULL;
            query_start = clock();
            query_5(c, format_flag, args, args_size, &res_5);
            FILE *save_result_5 = initialize_file_saving(line, outputDirectory);
            GList *iterList5 = res_5;
            //printf("Flights for command %d:\n", line);
            while (iterList5 != NULL) {
                char *str = iterList5->data;
                //printf("%s", str);
                save_line(save_result_5, str);
                g_free(str);
                iterList5 = g_list_next(iterList5);
            }
            close_file_saving(save_result_5);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            g_list_free(res_5);
            break;
        case 6:
            GList *res_6 = NULL;
            query_start = clock();
            query_6(c, format_flag, args, args_size, &res_6);
            FILE *save_result_6 = initialize_file_saving(line, outputDirectory);
            GList *iterList6 = res_6;
            //printf("Flights for command %d:\n", line);
            while (iterList6 != NULL) {
                char *str = iterList6->data;
                //printf("%s", str);
                save_line(save_result_6, str);
                g_free(str);
                iterList6 = g_list_next(iterList6);
            }
            close_file_saving(save_result_6);
            g_list_free(res_6);
            break;
        case 7:
            GList *res_7 = NULL;
            query_start = clock();
            query_7(c, format_flag, args, args_size, &res_7);
            FILE *save_result_7 = initialize_file_saving(line, outputDirectory);
            GList *iterList7 = res_7;
            //printf("Flights for command %d:\n", line);
            while (iterList7 != NULL) {
                char *str = iterList7->data;
                //printf("%s", str);
                save_line(save_result_7, str);
                g_free(str);
                iterList7 = g_list_next(iterList7);
            }
            close_file_saving(save_result_7);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            g_list_free(res_7);
            break;
        case 8:
            char* res_8 = query_8(c, format_flag, args, args_size);
            save_result(line, res_8, outputDirectory);
            //printf("query_8 output: %s\n", res_8);
            g_free(res_8);
            break;
        case 9:
            GList *res_9 = NULL;
            query_start = clock();
            query_9(c, format_flag, args, args_size, &res_9);
            FILE *save_result_9 = initialize_file_saving(line, outputDirectory);
            GList *iterList9 = res_9;
            //printf("User for command %d:\n", line);
            while (iterList9 != NULL) {
                char *str = iterList9->data;
                //printf("%s", str);
                save_line(save_result_9, str);
                g_free(str);
                iterList9 = g_list_next(iterList9);
            }
            close_file_saving(save_result_9);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            g_list_free(res_9);
            break;  
        case 10:
            GList *res_10 = NULL;
            query_start = clock();
            //query_10(c, format_flag, args, args_size, &res_10);
            FILE *save_result_10 = initialize_file_saving(line, outputDirectory);
            GList *iterList10 = res_10;
            //printf("User for command %d:\n", line);
            while (iterList10 != NULL) {
                char *str = iterList10->data;
                //printf("%s", str);
                save_line(save_result_10, str);
                g_free(str);
                iterList10 = g_list_next(iterList10);
            }
            close_file_saving(save_result_10);
            query_end = clock();
            log_ctime(query_start, query_end, query_id, runninTests);
            g_list_free(res_10);
            break;
		default:
			printf("Query ID inválido!\n");
			break;
	}
    g_free(args);
	free_command(result_command);
}

/**
 * @brief Executes the batch mode.
 * 
 * @param inputFile The input file with the commands to execute
 * @param outputDirectory The output directory to save the results
 * @param c The catalog @see struct CATALOG
 * @param runninTests Flag to indicate if we are running tests or not
 */
void batchMode(char *inputFile, char *outputDirectory, CATALOG *c, int runninTests) {
    FILE *fp = fopen(inputFile, "r");
    if (fp == NULL) {
        perror("Error opening input file!\n");
        exit(1);
    }
    char *command = g_malloc(sizeof(char) * MAX_COMMAND_SIZE);
    int line = 1;
    while (fgets(command, MAX_COMMAND_SIZE, fp) != NULL) {
        execute_command(line, command, outputDirectory, c, runninTests);
        line++;
    }
    g_free(command);
    fclose(fp);
}