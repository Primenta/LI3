/**
 * @file flights.h
 * @brief Header file for the flights parser.
*/
#ifndef FLIGHTS_H
#define FLIGHTS_H

#include <glib.h>

void free_flight(gpointer data);
void print_hash_flight(gpointer key, gpointer value, gpointer data);
GHashTable* parse_flights(const char* datasetDir, const char* outputDir);
void free_flights(GHashTable* users);
int isFlightValid(GHashTable *flights, char* flight_id);

#endif