/**
 * @file queries.h
 * @brief Header file for the queries module.
*/
#ifndef QUERIES_H
#define QUERIES_H

#include "catalog.h"

#define MAX_RES_SIZE 1024

#include <glib.h>

char* query_1(CATALOG *c, int format_flag, char **args, int args_size);
void query_2(CATALOG *c, int format_flag, char **args, int args_size, GList **resList);
char* query_3(CATALOG *c, int format_flag, char **args, int args_size);
void query_4(CATALOG *c, int format_flag, char **args, int args_size, GList **reservationsList);
void query_5(CATALOG *c, int format_flag, char **args, int args_size, GList **resList);
void query_6(CATALOG *c, int format_flag, char **args, int args_size, GList **resList);
void query_7(CATALOG *c, int format_flag, char **args, int args_size, GList **resList);
char* query_8(CATALOG *c, int format_flag, char **args, int args_size);
void query_9(CATALOG *c, int format_flag, char **args, int args_size, GList **usersList);
void query_10(CATALOG *c, int format_flag, char **args, int args_size, GList **resList);

#endif