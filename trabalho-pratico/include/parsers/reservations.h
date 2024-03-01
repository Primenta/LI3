/**
 * @file reservations.h
 * @brief Header file for the reservations parser.
*/
#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include <glib.h>

void free_reservation(gpointer data);
void print_hash_reservation(gpointer key, gpointer value, gpointer data);
GHashTable* parse_reservations(const char* datasetDir, const char* outputDir, GHashTable* users);
void free_reservations(GHashTable* users);

#endif