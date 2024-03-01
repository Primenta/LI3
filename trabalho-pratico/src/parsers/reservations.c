/**
 * @file reservations.c
 * @brief Implementation of the reservations parser.
*/
#include "structs.h"
#include "parsers/reservations.h"
#include "parsers/users.h"
#include "validation.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATASET_NAME "reservations.csv"
#define ERRORS_DATASET_NAME "reservations_errors.csv"

// Function to free memory allocated
/**
 * @brief Frees the memory allocated for a reservation.
 * 
 * @param data The data to be freed.
*/
void free_reservation(gpointer data) {
    RESERVATION *reservation = (RESERVATION *)data;
    g_free(reservation->id);
    g_free(reservation->user_id);
    g_free(reservation->hotel_id);
    g_free(reservation->hotel_name);
    g_free(reservation->hotel_stars);
    g_free(reservation->city_tax);
    g_free(reservation->address);
    g_free(reservation->begin_date);
    g_free(reservation->end_date);
    g_free(reservation->price_per_night);
    g_free(reservation->includes_breakfast);
    //g_free(reservation->room_details);
    g_free(reservation->rating);
    //g_free(reservation->comment);
    g_free(reservation);
}

// Function to print User struct stored in the hash table
/**
 * @brief Prints a reservation.
 * 
 * @param key The key of the hash table.
 * @param value The value of the hash table (not used).
 * @param data The data of the hash table (not used).
*/
void print_hash_reservation(gpointer key, gpointer value, gpointer data) {
    (void)key;
    (void)data;
    RESERVATION *reservation = (RESERVATION *) value;
    printf("Reservation: %s\n", reservation->id);
    printf("\tUser ID: %s\n", reservation->user_id);
    printf("\tHotel ID: %s\n", reservation->hotel_id);
    printf("\tHotel Name: %s\n", reservation->hotel_name);
    printf("\tHotel Stars: %s\n", reservation->hotel_stars);
    printf("\tCity Tax: %s\n", reservation->city_tax);
    printf("\tAddress: %s\n", reservation->address);
    printf("\tBegin Date: %s\n", reservation->begin_date);
    printf("\tEnd Date: %s\n", reservation->end_date);
    printf("\tPrice Per Night: %s\n", reservation->price_per_night);
    printf("\tIncludes Breakfast: %s\n", reservation->includes_breakfast);
    //printf("\tRoom Details: %s\n", reservation->room_details);
    printf("\tRating: %s\n", reservation->rating);
    //printf("\tComment: %s\n", reservation->comment);
}

// Function to parse a CSV file and populate a GHashTable with User structs
/**
 * @brief Parses a CSV file and populates a GHashTable with reservations.
 * 
 * @param datasetDir The directory of the dataset.
 * @param outputDir The directory of the output.
 * @param users The hash table of users.
 * @return GHashTable* The hash table of reservations.
*/
GHashTable* parse_reservations(const char* datasetDir, const char* outputDir, GHashTable* users) {
    GHashTable* reservations = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_reservation);

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
        RESERVATION* reservation = g_new0(RESERVATION, 1);

        if (tokens) {
            reservation->id = g_strdup(tokens[0]);
            reservation->user_id = g_strdup(tokens[1]);
            reservation->hotel_id = g_strdup(tokens[2]);
            reservation->hotel_name = g_strdup(tokens[3]);
            reservation->hotel_stars = g_strdup(tokens[4]);
            reservation->city_tax = g_strdup(tokens[5]);
            reservation->address = g_strdup(tokens[6]);
            reservation->begin_date = g_strdup(tokens[7]);
            reservation->end_date = g_strdup(tokens[8]);
            reservation->price_per_night = g_strdup(tokens[9]);
            reservation->includes_breakfast = g_strdup(tokens[10]);
            //reservation->room_details = g_strdup(tokens[11]);
            reservation->rating = g_strdup(tokens[12]);
            //reservation->comment = g_strdup(tokens[13]);

            // if validations fail
            if (isValidUser(users, reservation->user_id) != 1
            || compareDates(reservation->begin_date, reservation->end_date) != 1
            || validateRating(reservation->rating) != 1
            || validatePrice(reservation->price_per_night) != 1
            || validateBreakfast(reservation->includes_breakfast) != 1
            || validateTax(reservation->city_tax) != 1
            || validateStars(reservation->hotel_stars) != 1
            || validateFieldSize(reservation->id) != 1 || validateFieldSize(reservation->user_id) != 1 || validateFieldSize(reservation->hotel_id) != 1 || validateFieldSize(reservation->hotel_name) != 1 || validateFieldSize(reservation->address) != 1) {
                // add to errors file
                register_error_line(error_registery, line);
                free_reservation(reservation);
            } else {
                g_hash_table_insert(reservations, reservation->id, reservation);
            }
            
            g_strfreev(tokens); // Free the array of strings
        } else {
            // Handle the case where g_strsplit fails
            fprintf(stderr, "Error splitting line: %s\n", line);
            g_free(reservations); // Free the partially allocated USER struct
            continue;
        }
    }

    // close error registery
    close_error_registery(error_registery);

    fclose(file);

    return reservations;
}

// Function to free the memory used by the hash table and its contents
/**
 * @brief Frees the memory used by the hash table and its contents.
 * 
 * @param reservations The hash table of reservations.
*/
void free_reservations(GHashTable* reservations) {
    g_hash_table_destroy(reservations);
}