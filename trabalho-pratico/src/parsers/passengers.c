/**
 * @file passengers.c
 * @brief Implementation of the passengers parser.
*/
#include "structs.h"
#include "parsers/passengers.h"
#include "parsers/users.h"
#include "parsers/flights.h"
#include "validation.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATASET_NAME "passengers.csv"
#define ERRORS_DATASET_NAME "passengers_errors.csv"

/*
typedef struct flight_seats {
    char* flight_id;
    char** passengers; // an array of user ids
    int total_passengers;
} FLIGHT_SEATS;*/
// remember this time its a little different, we need to add the user id to the passengers array if the flight_id already exists in the hash table

// Function to free memory allocated for a User struct
/**
 * @brief Frees the memory allocated for a flight.
 * 
 * @param data The data to be freed.
*/
void free_flight_seats(gpointer data) {
    FLIGHT_SEATS *flight_seats = (FLIGHT_SEATS *)data;
    g_free(flight_seats->flight_id);
    for (int i = 0; i < flight_seats->total_passengers; i++) {
        g_free(flight_seats->passengers[i]);
    }
    g_free(flight_seats->passengers);
    g_free(flight_seats);
}

// Function to print User struct stored in the hash table
/**
 * @brief Prints a flight.
 * 
 * @param key The key of the hash table.
 * @param value The value of the hash table (not used).
 * @param data The data of the hash table (not used).
*/
void print_hash_passenger(gpointer key, gpointer value, gpointer data) {
    (void)key;
    (void)data;
    FLIGHT_SEATS *flight_seats = (FLIGHT_SEATS *)value;
    printf("Flight ID: %s\n", flight_seats->flight_id);
    for (int i = 0; i < flight_seats->total_passengers; i++) {
        printf("\t\tPassenger ID: %s\n", flight_seats->passengers[i]);
    }
    printf("\tTotal Passengers: %d\n", flight_seats->total_passengers);
}

// Function to parse a CSV file and populate a GHashTable with User structs
/**
 * @brief Parses a CSV file and populates a GHashTable with flight seats.
 * 
 * @param datasetDir The path to the dataset directory.
 * @param outputDir The path to the output directory.
 * @param users The hash table of users. @see parse_users
 * @param flights The hash table of flights. @see parse_flights
 * @return The hash table of flight seats (passengers).
*/
GHashTable* parse_passengers(const char* datasetDir, const char* outputDir, GHashTable* users, GHashTable* flights) {
    GHashTable* passengers = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_flight_seats);

    char datasetPath[MAX_PATH_LENGTH];
    // if datasetDir ends in /, remove it
    if (datasetDir[strlen(datasetDir) - 1] == '/') {
        snprintf(datasetPath, MAX_PATH_LENGTH, "%s%s", datasetDir, DATASET_NAME);
    } else {
        snprintf(datasetPath, MAX_PATH_LENGTH, "%s/%s", datasetDir, DATASET_NAME);
    }
    FILE* file = fopen(datasetPath, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_SIZE];
    // Read header line (assuming the first line is the header)
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        exit(EXIT_FAILURE);
    }

    // initialize error registery
    FILE *error_registery = initialize_error_registery(outputDir, ERRORS_DATASET_NAME);

    // add header to errors file
    line[strcspn(line, "\n")] = '\0';
    register_error_line(error_registery, line);

    int line_count = 0;
    while (fgets(line, sizeof(line), file)) {
        line_count++;
        // Remove newline character from the end
        line[strcspn(line, "\n")] = '\0';

        gchar** tokens = g_strsplit(line, ";", -1);
        FLIGHT_SEATS *flight_seats = g_new(FLIGHT_SEATS, 1);

        if (tokens) {
            flight_seats->flight_id = g_strdup(tokens[0]);
            flight_seats->passengers = g_new(char*, 1);
            flight_seats->passengers[0] = g_strdup(tokens[1]);
            flight_seats->total_passengers = 1;

            // if validations fail
            if (validateFieldSize(flight_seats->flight_id) != 1 || isValidUser(users, flight_seats->passengers[0]) != 1 || isFlightValid(flights, flight_seats->flight_id) != 1) {
                // add to errors file
                register_error_line(error_registery, line);
                free_flight_seats(flight_seats);
            } else {
                // if flight_id already exists in the hash table add it to the passengers array
                // else add it to the hash table
                FLIGHT_SEATS *flight_seats_in_hash_table = g_hash_table_lookup(passengers, flight_seats->flight_id);
                if (flight_seats_in_hash_table != NULL) {
                    flight_seats_in_hash_table->total_passengers++;
                    flight_seats_in_hash_table->passengers = g_realloc(flight_seats_in_hash_table->passengers, flight_seats_in_hash_table->total_passengers * sizeof(char*));
                    flight_seats_in_hash_table->passengers[flight_seats_in_hash_table->total_passengers - 1] = g_strdup(tokens[1]);
                    free_flight_seats(flight_seats);
                } else {
                    g_hash_table_insert(passengers, flight_seats->flight_id, flight_seats);
                }
            }
            
            g_strfreev(tokens); // Free the array of strings
        } else {
            // Handle the case where g_strsplit fails
            fprintf(stderr, "Error splitting line: %s\n", line);
            g_free(passengers); // Free the partially allocated USER struct
            continue;
        }
    }

    // close error registery
    close_error_registery(error_registery);

    fclose(file);

    return passengers;
}

// Function to free the memory used by the hash table and its contents
/**
 * @brief Frees the memory used by the hash table and its contents.
 * 
 * @param passengers The hash table of passengers. @see parse_passengers
*/
void free_passengers(GHashTable* passengers) {
    g_hash_table_destroy(passengers);
}

/**
 * @brief Gets the number of passengers in a flight.
 * 
 * @param passengers The hash table of passengers. @see parse_passengers
 * @param flight_id The flight id.
 * 
 * @return The number of passengers in a flight. If the flight doesn't exist, returns 0.
*/
int get_flight_passengers(GHashTable *passengers, char* flight_id) {
    FLIGHT_SEATS *flight_seats = g_hash_table_lookup(passengers, flight_id);
    if (flight_seats == NULL) {
        return 0;
    }
    return flight_seats->total_passengers;
}