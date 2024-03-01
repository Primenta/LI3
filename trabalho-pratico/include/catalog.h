/**
 * @file catalog.h
 * @brief Header file for the catalog data type.
 */
#ifndef CATALOG_H
#define CATALOG_H

#include <glib.h>

typedef struct catalog {
	GHashTable *users;
	GHashTable *passengers;
	GHashTable *flights;
    GHashTable *reservations;
} CATALOG;

void print_catalog(CATALOG *c);
void free_catalog(CATALOG *c);
CATALOG *newCatalog(GHashTable *users, GHashTable *passengers, GHashTable *flights, GHashTable *reservations);

#endif