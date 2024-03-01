/**
 * @file catalog.c
 * @brief Implementation of the catalog data type (saves all the data from the parsed CSV files from the hash tables).
 */
#include "catalog.h"
#include "structs.h"
#include "validation.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** REMODELACAO **/
struct CATALOG {
	GHashTable *users;
	GHashTable *passengers;
	GHashTable *flights;
    GHashTable *reservations;
};

// Getters/Setters
GHashTable *getUsers(CATALOG *c) {
    return c->users;
}

GHashTable *getPassengers(CATALOG *c) {
    return c->passengers;
}

GHashTable *getFlights(CATALOG *c) {
    return c->flights;
}

GHashTable *getReservations(CATALOG *c) {
    return c->reservations;
}

void setUsers(CATALOG *c, GHashTable *users) {
    c->users = users;
}

void setPassengers(CATALOG *c, GHashTable *passengers) {
    c->passengers = passengers;
}

void setFlights(CATALOG *c, GHashTable *flights) {
    c->flights = flights;
}

void setReservations(CATALOG *c, GHashTable *reservations) {
    c->reservations = reservations;
}

/**
 * @brief Prints a user from the hash table.
 * 
 * @param key The key of the user.
 * @param value The value of the user.
 * @param user_data The user data.
 */
void print_catalog(CATALOG *c) {
    if (c != NULL) {
        printf("Users:\n");
        g_hash_table_foreach(c->users, print_hash_user, NULL);
        printf("Passengers:\n");
        g_hash_table_foreach(c->passengers, print_hash_passenger, NULL);
        printf("Flights:\n");
        g_hash_table_foreach(c->flights, print_hash_flight, NULL);
        printf("Reservations:\n");
        g_hash_table_foreach(c->reservations, print_hash_reservation, NULL);
    }
}

/**
 * @brief Frees the memory allocated for the catalog.
 * 
 * @param c The catalog.
 */
void free_catalog(CATALOG *c) {
    if (c != NULL) {
        g_hash_table_destroy(c->users);
        g_hash_table_destroy(c->passengers);
        g_hash_table_destroy(c->flights);
        g_hash_table_destroy(c->reservations);
        g_free(c);
    }
}

/**
 * @brief Creates a new catalog.
 * 
 * @param users The users hash table.
 * @param passengers The passengers hash table.
 * @param flights The flights hash table.
 * @param reservations The reservations hash table.
 * @return CATALOG* The new catalog. @see struct CATALOG
 */
CATALOG *newCatalog(GHashTable *users, GHashTable *passengers, GHashTable *flights, GHashTable *reservations) {
    CATALOG *c = g_malloc(sizeof(CATALOG));
    c->users = users;
    c->passengers = passengers;
    c->flights = flights;
    c->reservations = reservations;
    return c;
}