/**
 * @file flights.c
 * @brief Implementation of the flights parser.
*/
#include "structs.h"
#include "parsers/flights.h"
#include "validation.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATASET_NAME "flights.csv"
#define ERRORS_DATASET_NAME "flights_errors.csv"

// Function to free memory allocated for a User struct
/**
 * @brief Frees the memory allocated for a flight.
 * 
 * @param data The data to be freed.
*/
void free_flight(gpointer data) {
    FLIGHT *flight = (FLIGHT *)data;
    g_free(flight->id);
    g_free(flight->airline);
    g_free(flight->plane_model);
    g_free(flight->total_seats);
    g_free(flight->origin);
    g_free(flight->destination);
    g_free(flight->schedule_departure_date);
    g_free(flight->schedule_arrival_date);
    g_free(flight->real_departure_date);
    g_free(flight->real_arrival_date);
    g_free(flight->pilot);
    g_free(flight->copilot);
    //g_free(flight->notes);
    g_free(flight);
}

// Function to print User struct stored in the hash table
/**
 * @brief Prints a flight.
 * 
 * @param key The key of the hash table.
 * @param value The value of the hash table (not used).
 * @param data The data of the hash table (not used).
*/
void print_hash_flight(gpointer key, gpointer value, gpointer data) {
    (void)key;
    (void)data;
    FLIGHT *flight = (FLIGHT *)value;
    printf("Flight ID: %s\n", flight->id);
    printf("\tAirline: %s\n", flight->airline);
    printf("\tPlane Model: %s\n", flight->plane_model);
    printf("\tTotal Seats: %s\n", flight->total_seats);
    printf("\tOrigin: %s\n", flight->origin);
    printf("\tDestination: %s\n", flight->destination);
    printf("\tSchedule Departure Date: %s\n", flight->schedule_departure_date);
    printf("\tSchedule Arrival Date: %s\n", flight->schedule_arrival_date);
    printf("\tReal Departure Date: %s\n", flight->real_departure_date);
    printf("\tReal Arrival Date: %s\n", flight->real_arrival_date);
    printf("\tPilot: %s\n", flight->pilot);
    printf("\tCopilot: %s\n", flight->copilot);
    //printf("\tNotes: %s\n", flight->notes);
}

// Function to parse a CSV file and populate a GHashTable with User structs
/**
 * @brief Parses a CSV file and populates a GHashTable with flights.
 * 
 * @param datasetDir The directory of the dataset.
 * @param outputDir The directory of the output.
 * @return GHashTable* The hash table with the flights.
*/
GHashTable* parse_flights(const char* datasetDir, const char* outputDir) {
    GHashTable* flights = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_flight);

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
        FLIGHT* flight = g_new0(FLIGHT, 1);

        if (tokens) {
            flight->id = g_strdup(tokens[0]);
            flight->airline = g_strdup(tokens[1]);
            flight->plane_model = g_strdup(tokens[2]);
            flight->total_seats = g_strdup(tokens[3]);
            flight->origin = g_strdup(tokens[4]);
            flight->destination = g_strdup(tokens[5]);
            flight->schedule_departure_date = g_strdup(tokens[6]);
            flight->schedule_arrival_date = g_strdup(tokens[7]);
            flight->real_departure_date = g_strdup(tokens[8]);
            flight->real_arrival_date = g_strdup(tokens[9]);
            flight->pilot = g_strdup(tokens[10]);
            flight->copilot = g_strdup(tokens[11]);
            //flight->notes = g_strdup(tokens[12]);
            
            /*int numberOfPassengers = 0;
            if (flight->id != NULL) {
                numberOfPassengers = get_flight_passengers(flight->id, datasetDir);
            }*/

            // if validations fail
            if (validateTrip(flight->origin, flight->destination) != 1
            || compareDates(flight->schedule_departure_date, flight->schedule_arrival_date) != 1
            || compareDates(flight->real_departure_date, flight->real_arrival_date) != 1
            || isInt(flight->total_seats) != 1
            //|| validateSeats(atoi(flight->total_seats), numberOfPassengers) != 1
            || validateFieldSize(flight->id) != 1 || validateFieldSize(flight->airline) != 1 || validateFieldSize(flight->plane_model) != 1 || validateFieldSize(flight->pilot) != 1 || validateFieldSize(flight->copilot) != 1) {
                // add to errors file
                register_error_line(error_registery, line);
                free_flight(flight);
            } else {
                g_hash_table_insert(flights, flight->id, flight);
            }
            
            g_strfreev(tokens); // Free the array of strings
        } else {
            // Handle the case where g_strsplit fails
            fprintf(stderr, "Error splitting line: %s\n", line);
            g_free(flight); // Free the partially allocated USER struct
            continue;
        }
    }

    // close error registery
    close_error_registery(error_registery);

    fclose(file);

    return flights;
}

// Function to free the memory used by the hash table and its contents
/**
 * @brief Frees the memory used by the hash table and its contents.
 * 
 * @param flights The hash table of flights. @see parse_flights
*/
void free_flights(GHashTable* flights) {
    g_hash_table_destroy(flights);
}

/**
 * @brief Gets the number of passengers in a flight (if it exists its valid).
 * 
 * @param flights The hash table of flights. @see parse_flights
 * @param flight_id The flight id.
 * 
 * @return The number of passengers in a flight.
*/
int isFlightValid(GHashTable *flights, char* flight_id) {
    FLIGHT *flight = g_hash_table_lookup(flights, flight_id);
    if (flight == NULL) {
        return 0;
    }
    return 1;
}