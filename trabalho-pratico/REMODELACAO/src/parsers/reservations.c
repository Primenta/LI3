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

/** REMODELACAO **/
// Structs
struct ReservationStruct {
    char* id;
    char* user_id;
    char* hotel_id;
    char* hotel_name;
    char* hotel_stars;
    char* city_tax;
    char* address;
    char* begin_date;
    char* end_date;
    char* price_per_night;
    char* includes_breakfast;
    //char* room_details;
    char* rating;
    //char* comment;
};

// Getters/Setters
RESERVATION* create_reservation() {
    return malloc(sizeof(RESERVATION));
}

void destroy_reservation(RESERVATION* reservation) {
    free(reservation);
}

// como o id é algo único este tem uma importância maior (dado ser possivel a sua utilização para obter informações rapidamente), por isso temos um setter e um getter únicos deste
void set_reservation_id(RESERVATION* reservation, const char* id) {
    if (reservation && id) {
        if (reservation->id) {
            free(reservation->id);
        }
        reservation->id = strdup(id);
    }
}

const char* get_reservation_id(RESERVATION* reservation) {
    return reservation ? reservation->id : NULL;
}

// (ver users.c para mais informações)
void update_reservation_info(RESERVATION* reservation, const int information_type, const char* information) {
    if (reservation && information) {
        switch (information_type) {
            case 1:
                if (reservation->user_id) {
                    free(reservation->user_id);
                }
                reservation->user_id = strdup(information);
                break;
            case 2:
                if (reservation->hotel_id) {
                    free(reservation->hotel_id);
                }
                reservation->hotel_id = strdup(information);
                break;
            case 3:
                if (reservation->hotel_name) {
                    free(reservation->hotel_name);
                }
                reservation->hotel_name = strdup(information);
                break;
            case 4:
                if (reservation->hotel_stars) {
                    free(reservation->hotel_stars);
                }
                reservation->hotel_stars = strdup(information);
                break;
            case 5:
                if (reservation->city_tax) {
                    free(reservation->city_tax);
                }
                reservation->city_tax = strdup(information);
                break;
            case 6:
                if (reservation->address) {
                    free(reservation->address);
                }
                reservation->address = strdup(information);
                break;
            case 7:
                if (reservation->begin_date) {
                    free(reservation->begin_date);
                }
                reservation->begin_date = strdup(information);
                break;
            case 8:
                if (reservation->end_date) {
                    free(reservation->end_date);
                }
                reservation->end_date = strdup(information);
                break;
            case 9:
                if (reservation->price_per_night) {
                    free(reservation->price_per_night);
                }
                reservation->price_per_night = strdup(information);
                break;
            case 10:
                if (reservation->includes_breakfast) {
                    free(reservation->includes_breakfast);
                }
                reservation->includes_breakfast = strdup(information);
                break;
            case 11:
                if (reservation->rating) {
                    free(reservation->rating);
                }
                reservation->rating = strdup(information);
                break;
        }
    }
}

const char* get_reservation_info(RESERVATION* reservation, const int information_type) {
    if (reservation) {
        switch (information_type) {
            case 1:
                return reservation->user_id;
            case 2:
                return reservation->hotel_id;
            case 3:
                return reservation->hotel_name;
            case 4:
                return reservation->hotel_stars;
            case 5:
                return reservation->city_tax;
            case 6:
                return reservation->address;
            case 7:
                return reservation->begin_date;
            case 8:
                return reservation->end_date;
            case 9:
                return reservation->price_per_night;
            case 10:
                return reservation->includes_breakfast;
            case 11:
                return reservation->rating;
        }
    }
    return NULL;
}

// Function to free memory allocated
/**
 * @brief Frees the memory allocated for a reservation.
 * 
 * @param data The data to be freed.
*/
void free_reservation(gpointer data) {
    RESERVATION *reservation = (RESERVATION *)data;
    if (reservation != NULL) {
        destroy_reservation(reservation);
    }
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
    printf("Reservation: %s\n", get_reservation_id(reservation));
    printf("\tUser ID: %s\n", get_reservation_info(reservation, 1));
    printf("\tHotel ID: %s\n", get_reservation_info(reservation, 2));
    printf("\tHotel Name: %s\n", get_reservation_info(reservation, 3));
    printf("\tHotel Stars: %s\n", get_reservation_info(reservation, 4));
    printf("\tCity Tax: %s\n", get_reservation_info(reservation, 5));
    printf("\tAddress: %s\n", get_reservation_info(reservation, 6));
    printf("\tBegin Date: %s\n", get_reservation_info(reservation, 7));
    printf("\tEnd Date: %s\n", get_reservation_info(reservation, 8));
    printf("\tPrice Per Night: %s\n", get_reservation_info(reservation, 9));
    printf("\tIncludes Breakfast: %s\n", get_reservation_info(reservation, 10));
    //printf("\tRoom Details: %s\n", reservation->room_details);
    printf("\tRating: %s\n", get_reservation_info(reservation, 11));
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
        RESERVATION* reservation = create_reservation();

        if (tokens) {
            set_reservation_id(reservation, tokens[0]);
            update_reservation_info(reservation, 1, tokens[1]);
            update_reservation_info(reservation, 2, tokens[2]);
            update_reservation_info(reservation, 3, tokens[3]);
            update_reservation_info(reservation, 4, tokens[4]);
            update_reservation_info(reservation, 5, tokens[5]);
            update_reservation_info(reservation, 6, tokens[6]);
            update_reservation_info(reservation, 7, tokens[7]);
            update_reservation_info(reservation, 8, tokens[8]);
            update_reservation_info(reservation, 9, tokens[9]);
            update_reservation_info(reservation, 10, tokens[10]);
            //reservation->room_details = g_strdup(tokens[11]);
            update_reservation_info(reservation, 11, tokens[12]);
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