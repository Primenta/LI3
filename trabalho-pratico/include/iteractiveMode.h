/**
 * @file iteractiveMode.h
 * @brief Header file for the iteractive mode module.
*/
#ifndef ITERATIVEMODE_H
#define ITERATIVEMODE_H

#include "batchMode.h"
#include <glib.h>

typedef struct {
	int isInTitleScreen; // Flag to check if the user is in the title screen
	int isDatasetLoaded; // Flag to check if the dataset is loaded
	int isLoading; // Flag to check if the dataset is being loaded
	int isInMenu; // Flag to check if the user is in the menu
	int isInPagination; // Flag to check if the user is in the pagination
	char *datasetPath; // Path to the dataset

	int pages; // Number of pages needed to print the results
	int page; // Current page

	int wx, wy; // Window size
} APPSTATUS;

void initialize_colors();
char *centeredString(char *str);
void freeCenteredString(char *str);
void showLoadingScreen();
void loadDataset(char *path, CATALOG *c);
void showTitleScreen();
void askDatasetPath();
void showMenu();
char *askForQArgs(int query_id);
void print_result(GList *result, int page_size, int *pages);
void showQResult(GList *result);
void execute_query(CATALOG *c, int query);
void execute_main_app(CATALOG *c);
void initialize_app_status();
void iteractiveMode();

#endif