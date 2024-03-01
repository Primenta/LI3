/**
 * @file passengers.h
 * @brief Header file for the passengers parser.
*/
#ifndef PASSENGERS_H
#define PASSENGERS_H

#include <glib.h>

void free_flight_seats(gpointer data);
void print_hash_passenger(gpointer key, gpointer value, gpointer data);
GHashTable* parse_passengers(const char* datasetDir, const char* outputDir, GHashTable* users, GHashTable* flights);
void free_passengers(GHashTable* passengers);
int get_flight_passengers(GHashTable *passengers, char* flight_id);

#endif