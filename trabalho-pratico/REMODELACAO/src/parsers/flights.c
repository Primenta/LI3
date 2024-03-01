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

/** REMODELACAO **/
// Structs
struct FlightStruct {
    char *id;
    char* airline;
    char* plane_model;
    char *total_seats;
    char* origin;
    char* destination;
    char* schedule_departure_date;
    char* schedule_arrival_date;
    char* real_departure_date;
    char* real_arrival_date;
    char* pilot;
    char* copilot;
    //char* notes;
};

// Getters/Setters
FLIGHT* create_flight() {
    return malloc(sizeof(FLIGHT));
}

void destroy_flight(FLIGHT* flight) {
    free(flight);
}

// como o id é algo único este tem uma importância maior (dado ser possivel a sua utilização para obter informações rapidamente), por isso temos um setter e um getter únicos deste
void set_flight_id(FLIGHT* flight, char* id) {
    if (flight && id) {
        if (flight->id) {
            free(flight->id);
        }
        flight->id = strdup(id);
    }
}

const char* get_flight_id(FLIGHT* flight) {
    return flight ? flight->id : NULL;
}

// (ver users.c para mais informações)
void update_flight_info(FLIGHT* flight, const int information_type, const char* information) {
    if (flight && information) {
        switch (information_type) {
            case 1:
                if (flight->airline) {
                    free(flight->airline);
                }
                flight->airline = strdup(information);
                break;
            case 2:
                if (flight->plane_model) {
                    free(flight->plane_model);
                }
                flight->plane_model = strdup(information);
                break;
            case 3:
                if (flight->total_seats) {
                    free(flight->total_seats);
                }
                flight->total_seats = strdup(information);
                break;
            case 4:
                if (flight->origin) {
                    free(flight->origin);
                }
                flight->origin = strdup(information);
                break;
            case 5:
                if (flight->destination) {
                    free(flight->destination);
                }
                flight->destination = strdup(information);
                break;
            case 6:
                if (flight->schedule_departure_date) {
                    free(flight->schedule_departure_date);
                }
                flight->schedule_departure_date = strdup(information);
                break;
            case 7:
                if (flight->schedule_arrival_date) {
                    free(flight->schedule_arrival_date);
                }
                flight->schedule_arrival_date = strdup(information);
                break;
            case 8:
                if (flight->real_departure_date) {
                    free(flight->real_departure_date);
                }
                flight->real_departure_date = strdup(information);
                break;
            case 9:
                if (flight->real_arrival_date) {
                    free(flight->real_arrival_date);
                }
                flight->real_arrival_date = strdup(information);
                break;
            case 10:
                if (flight->pilot) {
                    free(flight->pilot);
                }
                flight->pilot = strdup(information);
                break;
            case 11:
                if (flight->copilot) {
                    free(flight->copilot);
                }
                flight->copilot = strdup(information);
                break;
            /*case 12:
                if (flight->notes) {
                    free(flight->notes);
                }
                flight->notes = strdup(information);
                break;*/
            default:
                break;
        }
    }
}

const char* get_flight_info(FLIGHT* flight, const int information_type) {
    if (flight) {
        switch (information_type) {
            case 1:
                return flight->airline;
            case 2:
                return flight->plane_model;
            case 3:
                return flight->total_seats;
            case 4:
                return flight->origin;
            case 5:
                return flight->destination;
            case 6:
                return flight->schedule_departure_date;
            case 7:
                return flight->schedule_arrival_date;
            case 8:
                return flight->real_departure_date;
            case 9:
                return flight->real_arrival_date;
            case 10:
                return flight->pilot;
            case 11:
                return flight->copilot;
            /*case 12:
                return flight->notes;*/
            default:
                return NULL;
        }
    }
    return NULL;
}

/**
 * @brief Frees the memory allocated for a flight.
 * 
 * @param data The data to be freed.
*/
void free_flight(gpointer data) {
    FLIGHT *flight = (FLIGHT *)data;
    if (flight != NULL) {
        destroy_flight(flight);
    }
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
    printf("Flight ID: %s\n", get_flight_id(flight));
    printf("\tAirline: %s\n", get_flight_info(flight, 1));
    printf("\tPlane Model: %s\n", get_flight_info(flight, 2));
    printf("\tTotal Seats: %s\n", get_flight_info(flight, 3));
    printf("\tOrigin: %s\n", get_flight_info(flight, 4));
    printf("\tDestination: %s\n", get_flight_info(flight, 5));
    printf("\tSchedule Departure Date: %s\n", get_flight_info(flight, 6));
    printf("\tSchedule Arrival Date: %s\n", get_flight_info(flight, 7));
    printf("\tReal Departure Date: %s\n", get_flight_info(flight, 8));
    printf("\tReal Arrival Date: %s\n", get_flight_info(flight, 9));
    printf("\tPilot: %s\n", get_flight_info(flight, 10));
    printf("\tCopilot: %s\n", get_flight_info(flight, 11));
    //printf("\tNotes: %s\n", get_flight_info(flight, 12));
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
        FLIGHT* flight = create_flight();

        if (tokens) {
            set_flight_id(flight, tokens[0]);
            set_flight_info(flight, 1, tokens[1]); // Airline
            set_flight_info(flight, 2, tokens[2]); // Plane Model
            set_flight_info(flight, 3, tokens[3]); // Total Seats
            set_flight_info(flight, 4, tokens[4]); // Origin
            set_flight_info(flight, 5, tokens[5]); // Destination
            set_flight_info(flight, 6, tokens[6]); // Schedule Departure Date
            set_flight_info(flight, 7, tokens[7]); // Schedule Arrival Date
            set_flight_info(flight, 8, tokens[8]); // Real Departure Date
            set_flight_info(flight, 9, tokens[9]); // Real Arrival Date
            set_flight_info(flight, 10, tokens[10]); // Pilot
            set_flight_info(flight, 11, tokens[11]); // Copilot
            //set_flight_info(flight, 12, tokens[12]); // Notes
            
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