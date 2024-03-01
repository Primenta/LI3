/**
 * @file iteractiveMode.c
 * @brief Implementation of the iteractive mode module.
 */
#include "iteractiveMode.h"
#include "interpreter.h"
#include "parser.h"
#include "queries.h"

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h> // importa ncurses

APPSTATUS *appStatus;
GList *result; // List to store the result of a query

#define DEFAULT_OUTPUT_DIR "Resultados/"
#define NUM_QUERIES 10

/**
 * @brief Initializes the color pairs for the iteractive mode (ncurses).
 */
void initialize_colors() {
	// init_pair(nome, cor, bgcor)
	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK); // Initialize color pairs to black
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK); // White
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK); // Red
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK); // Green
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK); // Yellow
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK); // Blue
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK); // Magenta
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK); // Cyan
}

char* centeredString(char *str) {
	int strLen = strlen(str);
	int spaces = (appStatus->wx - strLen) / 2;
	char *centeredStr = (char *) g_malloc(sizeof(char) * appStatus->wx);
	if (centeredStr == NULL) { // Handle allocation error
		return NULL;
	}
	for (int i = 0; i < spaces; i++) {
		centeredStr[i] = ' ';
	}
	for (int i = spaces; i < spaces + strLen; i++) {
		centeredStr[i] = str[i - spaces];
	}
	for (int i = spaces + strLen; i < appStatus->wx; i++) {
		centeredStr[i] = ' ';
	}
	return centeredStr;
}

void freeCenteredString(char *str) {
	g_free(str);
}

void showLoadingScreen() {
	clear();
	attron(COLOR_PAIR(COLOR_GREEN));
	attron(A_BOLD);
	attron(A_REVERSE);
	char *title = centeredString("A carregar...");
	printw("%s", title);
	freeCenteredString(title);
	attroff(A_REVERSE);
	attron(COLOR_PAIR(COLOR_YELLOW));
	printw("\n");
	char *textinfo = centeredString("Pressione 'q' para sair.");
	printw("%s", textinfo);
	freeCenteredString(textinfo);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_YELLOW));
	refresh();
}

void loadDataset(char *path, CATALOG *c) {
	appStatus->isLoading = 1;
	showLoadingScreen();
	GHashTable *users = parse_users(path, DEFAULT_OUTPUT_DIR);
	GHashTable *reservations = parse_reservations(path, DEFAULT_OUTPUT_DIR, users);
	GHashTable *flights = parse_flights(path, DEFAULT_OUTPUT_DIR);
	GHashTable *passengers = parse_passengers(path, DEFAULT_OUTPUT_DIR, users, flights);
	c->users = users;
	c->reservations = reservations;
	c->flights = flights;
	c->passengers = passengers;
	appStatus->isLoading = 0;
	appStatus->isDatasetLoaded = 1;
}

/**
 * @brief Shows a custom ascii text title screen.
 */
void showTitleScreen() {
	attron(COLOR_PAIR(COLOR_GREEN));
	attron(A_BOLD);
	attron(A_REVERSE);
	char *title = centeredString("Bem-vindo ao BDM - Booking Dataset Manager!");
	printw("%s", title);
	freeCenteredString(title);
	attroff(A_REVERSE);
	if (appStatus->wy >= 20) {
		printw("\n");
		char *strart_1 = centeredString("BBBBBBBBBBBBBBBBB   DDDDDDDDDDDDD        MMMMMMMM               MMMMMMMM");
		printw("%s", strart_1);
		char *strart_2 = centeredString("B::::::::::::::::B  D::::::::::::DDD     M:::::::M             M:::::::M");
		printw("%s", strart_2);
		char *strart_3 = centeredString("B::::::BBBBBB:::::B D:::::::::::::::DD   M::::::::M           M::::::::M");
		printw("%s", strart_3);
		char *strart_4 = centeredString("BB:::::B     B:::::BDDD:::::DDDDD:::::D  M:::::::::M         M:::::::::M");
		printw("%s", strart_4);
		char *strart_5 = centeredString("  B::::B     B:::::B  D:::::D    D:::::D M::::::::::M       M::::::::::M");
		printw("%s", strart_5);
		char *strart_6 = centeredString("  B::::B     B:::::B  D:::::D     D:::::DM:::::::::::M     M:::::::::::M");
		printw("%s", strart_6);
		char *strart_7 = centeredString("  B::::BBBBBB:::::B   D:::::D     D:::::DM:::::::M::::M   M::::M:::::::M");
		printw("%s", strart_7);
		char *strart_8 = centeredString("  B:::::::::::::BB    D:::::D     D:::::DM::::::M M::::M M::::M M::::::M");
		printw("%s", strart_8);
		char *strart_9 = centeredString("  B::::BBBBBB:::::B   D:::::D     D:::::DM::::::M  M::::M::::M  M::::::M");
		printw("%s", strart_9);
		char *strart_10 = centeredString("  B::::B     B:::::B  D:::::D     D:::::DM::::::M   M:::::::M   M::::::M");
		printw("%s", strart_10);
		char *strart_11 = centeredString("  B::::B     B:::::B  D:::::D     D:::::DM::::::M    M:::::M    M::::::M");
		printw("%s", strart_11);
		char *strart_12 = centeredString("  B::::B     B:::::B  D:::::D    D:::::D M::::::M     MMMMM     M::::::M");
		printw("%s", strart_12);
		char *strart_13 = centeredString("BB:::::BBBBBB::::::BDDD:::::DDDDD:::::D  M::::::M               M::::::M");
		printw("%s", strart_13);
		char *strart_14 = centeredString("B:::::::::::::::::B D:::::::::::::::DD   M::::::M               M::::::M");
		printw("%s", strart_14);
		char *strart_15 = centeredString("B::::::::::::::::B  D::::::::::::DDD     M::::::M               M::::::M");
		printw("%s", strart_15);
		char *strart_16 = centeredString("BBBBBBBBBBBBBBBBB   DDDDDDDDDDDDD        MMMMMMMM               MMMMMMMM");
		printw("%s", strart_16);
		freeCenteredString(strart_1);
		freeCenteredString(strart_2);
		freeCenteredString(strart_3);
		freeCenteredString(strart_4);
		freeCenteredString(strart_5);
		freeCenteredString(strart_6);
		freeCenteredString(strart_7);
		freeCenteredString(strart_8);
		freeCenteredString(strart_9);
		freeCenteredString(strart_10);
		freeCenteredString(strart_11);
		freeCenteredString(strart_12);
		freeCenteredString(strart_13);
		freeCenteredString(strart_14);
		freeCenteredString(strart_15);
		freeCenteredString(strart_16);
	}
	attroff(COLOR_PAIR(COLOR_GREEN));
	attron(COLOR_PAIR(COLOR_YELLOW));
	printw("\n");
	char *textinfo_1 = centeredString("Pressione qualquer tecla para continuar.");
	printw("%s", textinfo_1);
	freeCenteredString(textinfo_1);
	char *textinfo_2 = centeredString("Pressione 'q' para sair.");
	printw("%s", textinfo_2);
	freeCenteredString(textinfo_2);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_YELLOW));
}

void askDatasetPath() {
	clear();
	attron(A_BOLD);
	attron(COLOR_PAIR(COLOR_GREEN));
	attron(A_REVERSE);
	char *title = centeredString("Initialize dataset:");
	printw("%s", title);
	freeCenteredString(title);
	attroff(A_REVERSE);
	attroff(COLOR_PAIR(COLOR_GREEN));
	printw("\n");
	printw("Insira o caminho para o dataset: (Default: '%s') ", appStatus->datasetPath);
	attroff(A_BOLD);
	attron(COLOR_PAIR(COLOR_YELLOW));
	refresh();
	echo();
	char *path = g_malloc(sizeof(char) * 100);
	getstr(path);
	// if the path is empty, use the default path
	if (path != NULL && strlen(path) != 0 && strcmp(path, "") != 0 && strcmp(path, "\n") != 0 && strcmp(path, "\0") != 0) {
		appStatus->datasetPath = path;
	}
	attroff(COLOR_PAIR(COLOR_YELLOW));
	noecho();
}

void showMenu() {
	appStatus->isInMenu = 1;
	clear();
	attron(COLOR_PAIR(COLOR_GREEN));
	attron(A_BOLD);
	attron(A_REVERSE);
	char *title = centeredString("Menu:");
	printw("%s", title);
	freeCenteredString(title);
	attroff(A_REVERSE);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_GREEN));
	printw("\n");
	char *subtitle = centeredString("Selecione uma query a executar:");
	printw("%s", subtitle);
	freeCenteredString(subtitle);
	attron(A_BOLD);
	for (int i = 0; i < NUM_QUERIES; i++) {
		char *option_text = (char *) g_malloc(sizeof(char) * 20);
		sprintf(option_text, "%d - Query %d", i, i + 1);
		char *option = centeredString(option_text);
		printw("%s", option);
		freeCenteredString(option);
		g_free(option_text);
	}
	attroff(A_BOLD);
	printw("\n");
	attron(COLOR_PAIR(COLOR_YELLOW));
	attron(A_BOLD);
	char *textinfo = centeredString("Pressione 'q' para sair.");
	printw("%s", textinfo);
	freeCenteredString(textinfo);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_YELLOW));
}

// askForQArgs first asks if we want the query formatted or not (adds F to the query id)
// then asks for the query args and retuns them as a string (command), like "1F agr1 arg2 arg3" or "1 arg1 arg2 arg3" (1 is the query id passed as parameter)
char* askForQArgs(int query_id) {
	clear();
	attron(COLOR_PAIR(COLOR_GREEN));
	attron(A_BOLD);
	attron(A_REVERSE);
	char *title = centeredString("Setup query:");
	printw("%s", title);
	freeCenteredString(title);
	attroff(A_REVERSE);
	attroff(COLOR_PAIR(COLOR_GREEN));
	printw("\n");
	refresh();
	echo();
	char *command = g_malloc(sizeof(char) * 100);
	sprintf(command, "%d", query_id);
	char *format_flag = g_malloc(sizeof(char) * 2);
	attron(A_BOLD);
	printw("Insira o formato do output? Y/N (Default: 'N') ");
	attroff(A_BOLD);
	attron(COLOR_PAIR(COLOR_YELLOW));
	getstr(format_flag);
	if (format_flag != NULL && strlen(format_flag) != 0 && strcmp(format_flag, "") != 0 && strcmp(format_flag, "\n") != 0 && strcmp(format_flag, "\0") != 0) {
		if (strcmp(format_flag, "y") == 0 || strcmp(format_flag, "Y") == 0) {
			strcat(command, "F");
		}
	}
	g_free(format_flag);
	attroff(COLOR_PAIR(COLOR_YELLOW));
	char *args = g_malloc(sizeof(char) * 100);
	attron(A_BOLD);
	printw("Insira os argumentos da query: ");
	attroff(A_BOLD);
	attron(COLOR_PAIR(COLOR_YELLOW));
	getstr(args);
	if (args != NULL && strlen(args) != 0 && strcmp(args, "") != 0 && strcmp(args, "\n") != 0 && strcmp(args, "\0") != 0) {
		strcat(command, " ");
		strcat(command, args);
	}
	g_free(args);
	attroff(COLOR_PAIR(COLOR_YELLOW));
	noecho();
	return command;
}

void print_result(GList *result, int page_size, int *pages) {
	GHashTable *paginated_result = g_hash_table_new(g_str_hash, g_str_equal);
	int current_page = 1; // this is not used as pagination, its used as key to split n lines in the page
	int lines_in_page = 0;
	int current_line = 0;
	for (GList *l = result; l != NULL; l = l->next) {
		char *line = (char *) l->data;
		int line_len = strlen(line);
		// if line is empty we dont need to count it
		if (line_len == 0 || strcmp(line, "") == 0 || strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0) {
			continue;
		}
		int line_lines = 1;
		for (int j = 0; j < line_len; j++) {
			if (line[j] == '\n') {
				line_lines++;
			}
		}
		// now check if line_lines+1 (the '\n's in a line and the line itself) fits in the page
		if (lines_in_page + line_lines > page_size) {
			// it doesnt fit in the page, so we increment the current page and add the rest to the saved lines
			current_page++;
			lines_in_page = line_lines;
		} else {
			// it fits in the page, so we can print it and add the lines to the lines in page
			lines_in_page += line_lines;
			// we can add the line to the hash table
			// if current_page is not in the hash table, we need to create a new key, if it is we concatenate the line to the value
			char* key = g_strdup_printf("%d", current_page);
			char* value = (char *) g_hash_table_lookup(paginated_result, key);
			if (value == NULL) {
				// the key doesnt exist, so we create a new key with the line
				g_hash_table_insert(paginated_result, key, line);
			} else {
				// the key exists, so we concatenate the line to the value
				char *new_value = g_strdup_printf("%s%s", value, line);
				g_hash_table_insert(paginated_result, key, new_value);
			}
		}
		current_line++;
	}
	// in the end as we have all pages completed, the total number of pages is the number of keys in the hash table
	*pages = current_page;
	// now we can print the lines
	for (GList *l = g_hash_table_get_keys(paginated_result); l != NULL; l = l->next) {
		char *key = (char *) l->data;
		if (atoi(key) == appStatus->page) {
			char *value = (char *) g_hash_table_lookup(paginated_result, key);
			printw("%s", value);
		}
	}
	// free memory
	g_hash_table_destroy(paginated_result);
}

void showQResult(GList *result) {
	appStatus->isInPagination = 1;
	clear();
	attron(COLOR_PAIR(COLOR_GREEN));
	attron(A_BOLD);
	attron(A_REVERSE);
	char *title = centeredString("Resultados:");
	printw("%s", title);
	freeCenteredString(title);
	attroff(A_REVERSE);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_GREEN));
	printw("\n");
	attron(A_BOLD);
	print_result(result, appStatus->wy-4, &(appStatus->pages));
	attroff(A_BOLD);
	printw("\n");
	// on the bottom of the page we can add a pagination
	attron(COLOR_PAIR(COLOR_WHITE));
	attron(A_BOLD);
	attron(A_REVERSE);
	char *footer_text = (char *) g_malloc(sizeof(char) * 20);
	sprintf(footer_text, "< Página %d de %d >", appStatus->page, appStatus->pages);
	char *footer = centeredString(footer_text);
	printw("%s", footer);
	freeCenteredString(footer);
	g_free(footer_text);
	attroff(A_REVERSE);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_WHITE));
	// also add a text to exit
	attron(COLOR_PAIR(COLOR_YELLOW));
	attron(A_BOLD);
	char *textinfo = centeredString("Pressione 'q' para voltar.");
	printw("%s", textinfo);
	freeCenteredString(textinfo);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(COLOR_YELLOW));
}

void execute_query(CATALOG *c, int query) {
	appStatus->isInMenu = 0; // TODO: we can comment this and execute multiple queries in a row
	char *command = askForQArgs(query);
	//printw("Command: %s\n", command);
	// we cant write the query results to a file because it crashes ncruses
	// so we will create a list that stores every line of the result (maybe we can use a queue instead of a list)
	//GList *result = NULL; // making this a global variable for now
	char* result_command[3];
	command_interpreter(command, result_command);
	/*printf("Query ID: %s\n", result_command[0]);
	printf("Format flag: %s\n", result_command[1]);
	printf("Args: %s\n", result_command[2]);*/
	g_free(command);
	int query_id = query;
	int format_flag = (result_command[1] != NULL && strcmp(result_command[1], "") != 0);
	// split args by space
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
	// execute queries
	switch (query_id) {
		case 1:
			char* res_1 = query_1(c, format_flag, args, args_size);
			result = g_list_append(result, res_1);
			break;
		case 2:
			GList *res_2 = NULL;
            query_2(c, format_flag, args, args_size, &res_2);
			result = res_2;
			break;
		case 3:
			char* res_3 = query_1(c, format_flag, args, args_size);
			result = g_list_append(result, res_3);
			break;
		case 4:
			GList *res_4 = NULL;
			query_4(c, format_flag, args, args_size, &res_4);
			result = res_4;
			break;
		case 5:
			GList *res_5 = NULL;
			query_5(c, format_flag, args, args_size, &res_5);
			result = res_5;
			break;
		case 6:
			GList *res_6 = NULL;
			query_6(c, format_flag, args, args_size, &res_6);
			result = res_6;
			break;
		case 7:
			GList *res_7 = NULL;
			query_7(c, format_flag, args, args_size, &res_7);
			result = res_7;
			break;
		case 8:
			char* res_8 = query_8(c, format_flag, args, args_size);
			result = g_list_append(result, res_8);
			break;
		case 9:
			GList *res_9 = NULL;
			query_9(c, format_flag, args, args_size, &res_9);
			result = res_9;
			break;
		case 10:
			GList *res_10 = NULL;
			query_10(c, format_flag, args, args_size, &res_10);
			result = res_10;
			break;
		default:
			printw("Query ID inválido!\n");
			break;
	}
	// print results
	if (result != NULL) {
		// if the result is empty we dont print anything
		if (g_list_length(result) <= 1 && (strcmp((char *) g_list_nth_data(result, 0), "") == 0 || strcmp((char *) g_list_nth_data(result, 0), "\n") == 0 || strcmp((char *) g_list_nth_data(result, 0), "\0") == 0)) {
			// add a string to the list to print
			result = g_list_append(result, "A query não retornou resultados!");
		}
	} else {
		result = g_list_append(result, "A query não retornou resultados!");
	}
	// lets create a new page to print the results (with pagination)
	showQResult(result);
	// free memory
	g_free(args);
	free_command(result_command);
}

void execute_main_app(CATALOG *c) {
	(void) c;
	if (appStatus->isDatasetLoaded == 1) {
		showMenu();
	}
}

void initialize_app_status() {
	appStatus = (APPSTATUS *) g_malloc(sizeof(APPSTATUS));
	appStatus->isInTitleScreen = 1;
	appStatus->isDatasetLoaded = 0;
	appStatus->isLoading = 0;
	appStatus->isInMenu = 0;
	appStatus->isInPagination = 0;
	appStatus->datasetPath = "dataset/data";
	appStatus->pages = 1;
	appStatus->page = 1;
	appStatus->wx = 0;
	appStatus->wy = 0;
}

/**
 * @brief Executes the iteractive mode.
 */
void iteractiveMode() {
	initialize_app_status();
    initscr(); // Inicializar ncurses
    noecho(); // Não mostrar o input do utilizador
    cbreak(); // Não esperar pelo enter
    keypad(stdscr, TRUE); // Permitir usar as setas
	curs_set(0); // Esconder o cursor
	start_color(); // Inicializar as cores
	initialize_colors();
	getmaxyx(stdscr, appStatus->wy, appStatus->wx); // Window size
    int ch;
    showTitleScreen();
	CATALOG *c = newCatalog(NULL, NULL, NULL, NULL);
    while (1) {
        ch = getch();
        if ((ch == 'q' && appStatus->isInTitleScreen) || (ch == 'q' && appStatus->isLoading) || (ch == 'q' && appStatus->isInMenu)) {
            break;
        } else if ((ch == 'q' && appStatus->isInPagination)) {
			clear();
			appStatus->isLoading = 0;
			appStatus->isInMenu = 1;
			appStatus->isInPagination = 0;
			// for pagination we need to reset the page and result
			appStatus->page = 1;
			result = NULL;
			execute_main_app(c);
		} else if (appStatus->isInTitleScreen) {
			clear();
			appStatus->isInTitleScreen = 0;
			if (appStatus->isDatasetLoaded == 0) {
				askDatasetPath();
				loadDataset(appStatus->datasetPath, c);
				execute_main_app(c);
			}
		} else if (appStatus->isInMenu) {
			// if ch between 0 and NUM_QUERIES-1
			// as we have 10 queries we can simplify and do, if 0 execute query 1 and so on
			if (ch >= '0' && ch <= '9') {
				execute_query(c, ch - '0' + 1);
			}
		} else if (appStatus->isInPagination) {
			if (ch == KEY_LEFT) {
				if (appStatus->page > 1) {
					appStatus->page--;
					showQResult(result);
				}
			} else if (ch == KEY_RIGHT) {
				if (appStatus->page < appStatus->pages) {
					appStatus->page++;
					showQResult(result);
				}
			}
		}
        //printw("Caractere lido: %c\n", ch);
        refresh();
    }
	g_free(appStatus);
    endwin(); // Encerrar a biblioteca ncurses
}