/**
 * @file catalog.h
 * @brief Header file for the catalog data type.
 */
#ifndef CATALOG_H
#define CATALOG_H

#include <glib.h>

typedef struct catalog CATALOG;

void print_catalog(CATALOG *c);
void free_catalog(CATALOG *c);
CATALOG *newCatalog(GHashTable *users, GHashTable *passengers, GHashTable *flights, GHashTable *reservations);
// Getters/Setters
GHashTable *getUsers(CATALOG *c);
GHashTable *getPassengers(CATALOG *c);
GHashTable *getFlights(CATALOG *c);
GHashTable *getReservations(CATALOG *c);
void setUsers(CATALOG *c, GHashTable *users);
void setPassengers(CATALOG *c, GHashTable *passengers);
void setFlights(CATALOG *c, GHashTable *flights);
void setReservations(CATALOG *c, GHashTable *reservations);

#endif