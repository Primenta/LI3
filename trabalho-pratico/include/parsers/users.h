/**
 * @file users.h
 * @brief Header file for the users parser.
*/
#ifndef USERS_H
#define USERS_H

#include <glib.h>

void free_user(gpointer data);
void print_hash_user(gpointer key, gpointer value, gpointer data);
GHashTable* parse_users(const char* datasetDir, const char* outputDir);
void free_users(GHashTable* users);
int isValidUser(GHashTable *users, const char *user_id);

#endif