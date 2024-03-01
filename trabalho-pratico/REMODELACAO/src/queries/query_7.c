/**
 * @file query_7.c
 * @brief Implementation of query 7.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"
#include <ctype.h>

/** REMODELACAO **/

/**
 * @brief Helper function to get the flight delay (in seconds).
 * 
 * @param flight The flight.
 * @return int The flight delay.
 */
int get_flight_delay(FLIGHT *flight) {
    // get the estimated_departure_date and departure_date
    char *estimated_departure_date_copy = g_strdup(get_flight_info(flight, 6));
    char *departure_date_copy = g_strdup(get_flight_info(flight, 8));

    // get the year, month, day, hour and minute from the estimated_departure_date
    char *estimated_departure_date_year_str = strtok(estimated_departure_date_copy, "/");
    char *estimated_departure_date_month_str = strtok(NULL, "/");
    char *estimated_departure_date_day_str = strtok(NULL, " ");
    char *estimated_departure_date_hour_str = strtok(NULL, ":");
    char *estimated_departure_date_minute_str = strtok(NULL, ":");
    int estimated_departure_date_year = atoi(estimated_departure_date_year_str);
    int estimated_departure_date_month = atoi(estimated_departure_date_month_str);
    int estimated_departure_date_day = atoi(estimated_departure_date_day_str);
    int estimated_departure_date_hour = atoi(estimated_departure_date_hour_str);
    int estimated_departure_date_minute = atoi(estimated_departure_date_minute_str);

    // get the year, month, day, hour and minute from the departure_date
    char *departure_date_year_str = strtok(departure_date_copy, "/");
    char *departure_date_month_str = strtok(NULL, "/");
    char *departure_date_day_str = strtok(NULL, " ");
    char *departure_date_hour_str = strtok(NULL, ":");
    char *departure_date_minute_str = strtok(NULL, ":");
    int departure_date_year = atoi(departure_date_year_str);
    int departure_date_month = atoi(departure_date_month_str);
    int departure_date_day = atoi(departure_date_day_str);
    int departure_date_hour = atoi(departure_date_hour_str);
    int departure_date_minute = atoi(departure_date_minute_str);

    // calculate the delay
    int delay = 0;
    delay += (departure_date_year - estimated_departure_date_year) * 365 * 24 * 60 * 60;
    delay += (departure_date_month - estimated_departure_date_month) * 30 * 24 * 60 * 60;
    delay += (departure_date_day - estimated_departure_date_day) * 24 * 60 * 60;
    delay += (departure_date_hour - estimated_departure_date_hour) * 60 * 60;
    delay += (departure_date_minute - estimated_departure_date_minute) * 60;

    // free the strings
    g_free(estimated_departure_date_copy);
    g_free(departure_date_copy);

    return delay;
}

/**
 * @brief Helper function to compare two integers.
 * 
 * @param a The first integer.
 * @param b The second integer.
 * @return int The result of the comparison.
 */
static gint int_compare(gconstpointer a, gconstpointer b) {
    int int_a = GPOINTER_TO_INT(a);
    int int_b = GPOINTER_TO_INT(b);

    if (int_a == int_b) {
        return 0;
    } else if (int_a < int_b) {
        return -1;
    } else {
        return 1;
    }
}

typedef struct {
    char *airport_origin;
    GList *delays;
} AIRPORT_DELAYS;

void free_airport_delays(gpointer data) {
    AIRPORT_DELAYS *airport_delays_tuple = (AIRPORT_DELAYS *)data;
    g_free(airport_delays_tuple->airport_origin);
    g_list_free(airport_delays_tuple->delays);
    g_free(airport_delays_tuple);
}

/**
 * @brief Helper function to compare two airports by their median of delays, if the medians are equal, the airport name is used as a tiebreaker (in ascending order).
 * 
 * @param a The first airport.
 * @param b The second airport.
 * @return int The result of the comparison.
 */
static gint airport_compare (gconstpointer a, gconstpointer b) {
    int res = 0;

    char *airport_a = (char *)a;
    char *airport_b = (char *)b;

    // using g_strsplit
    char **airport_a_tokens = g_strsplit(airport_a, ";", 0);
    char **airport_b_tokens = g_strsplit(airport_b, ";", 0);

    // get the airport and median from the tokens
    char *airport_a_airport = airport_a_tokens[0];
    char *airport_b_airport = airport_b_tokens[0];

    int airport_a_median = atoi(airport_a_tokens[1]);
    int airport_b_median = atoi(airport_b_tokens[1]);

    // compare the medians
    int median_cmp = airport_a_median - airport_b_median;
    if (median_cmp == 0) {
        // if theres a tie, compare the airport
        res = strcmp(airport_a_airport, airport_b_airport);
    } else {
        res = -median_cmp;
    }

    // free the tokens
    g_strfreev(airport_a_tokens);
    g_strfreev(airport_b_tokens);

    return res;
}

/**
 * @brief Returns the top N airports with the highest median of delays. Delays at an airport are calculated from the difference between the estimated date and the actual departure date, for flights originating at that airport. The delay value should be presented in seconds. If two airports have the same median, the airport name should be used as a tiebreaker (in ascending order).
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param resList The result list.
 */
void query_7(CATALOG *c, int format_flag, char **args, int args_size, GList **resList) {
    *resList = NULL;

    if (args_size < 1) {
        return;
    }

    int N = atoi(args[0]);

    // Create a list to store airport delays
    GList *airport_delays = NULL;

    // Iterate over the flights
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, getFlights(c));
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        FLIGHT *flight = (FLIGHT *)value;

        // Get the flight delay
        int flight_delay = get_flight_delay(flight);

        // Get the airport origin
        char *airport_origin = toupper_str(get_flight_info(flight, 4));

        // Check if the airport_origin is already in the list
        GList *airport_delays_iter = airport_delays;
        gboolean airport_origin_in_list = FALSE;

        while (airport_delays_iter != NULL) {
            AIRPORT_DELAYS *airport_delays_tuple = (AIRPORT_DELAYS *)airport_delays_iter->data;

            // Check if the airport_origin is the same
            if (strcmp(airport_delays_tuple->airport_origin, airport_origin) == 0) {
                // Add the delay to the list
                airport_delays_tuple->delays = g_list_append(airport_delays_tuple->delays, GINT_TO_POINTER(flight_delay));

                // Set the flag
                airport_origin_in_list = TRUE;

                // Free the memory for airport_origin, as we don't need it anymore
                g_free(airport_origin);

                break;
            }

            airport_delays_iter = g_list_next(airport_delays_iter);
        }

        // If the airport_origin is not in the list, create a new tuple and add it to the list
        if (!airport_origin_in_list) {
            AIRPORT_DELAYS *airport_delays_tuple = g_new(AIRPORT_DELAYS, 1);
            airport_delays_tuple->airport_origin = airport_origin;
            airport_delays_tuple->delays = g_list_append(NULL, GINT_TO_POINTER(flight_delay));

            // Add the tuple to the list
            airport_delays = g_list_append(airport_delays, airport_delays_tuple);
        }
    }

    // calculate the median and add to result list
    GList *airport_delays_iter = airport_delays;

    while (airport_delays_iter != NULL) {
        AIRPORT_DELAYS *airport_delays_tuple = (AIRPORT_DELAYS *)airport_delays_iter->data;

        // Sort the list of delays
        airport_delays_tuple->delays = g_list_sort(airport_delays_tuple->delays, int_compare);

        // Calculate the median
        int delays_size = g_list_length(airport_delays_tuple->delays);
        int median = 0;
        if (delays_size % 2 == 0) {
            int index1 = delays_size / 2 - 1;
            int index2 = delays_size / 2;
            int delay1 = GPOINTER_TO_INT(g_list_nth_data(airport_delays_tuple->delays, index1));
            int delay2 = GPOINTER_TO_INT(g_list_nth_data(airport_delays_tuple->delays, index2));
            median = (delay1 + delay2) / 2;
        } else {
            int index = delays_size / 2;
            median = GPOINTER_TO_INT(g_list_nth_data(airport_delays_tuple->delays, index));
        }

        // Create the string
        char *str = g_strdup_printf("%s;%d\n", airport_delays_tuple->airport_origin, median);

        // Add the string to the result list
        *resList = g_list_append(*resList, str);

        airport_delays_iter = g_list_next(airport_delays_iter);
    }

    // Sort the result list
    *resList = g_list_sort(*resList, (GCompareFunc)airport_compare);

    // Get the top N airports
    // iterate over the list and free the memory for the airports that are not in the top N
    GList *resList_iter = *resList;
    int i = 0;
    while (resList_iter != NULL) {
        if (i >= N) {
            char *str = (char *)resList_iter->data;
            g_free(str);
            resList_iter->data = NULL;
        }

        resList_iter = g_list_next(resList_iter);
        i++;
    }

    // Remove the NULL elements from the list
    *resList = g_list_remove_all(*resList, NULL);

    // if format flag if 0, we let the resList as it is, if not we need to format the result "--- 1 ---\nname:airpot_name\nmedian:median\n\n"
    int number_of_results = 1;
    if (format_flag) {
        // Create a new list to store the formatted results
        GList *formatted_resList = NULL;

        // Iterate over the result list
        GList *resList_iter = *resList;
        while (resList_iter != NULL) {
            char *str = (char *)resList_iter->data;

            // Split the string
            char **tokens = g_strsplit(str, ";", 0);

            // Get the airport and median from the tokens
            char *airport = tokens[0];
            char *median_str = tokens[1];

            // Create the formatted string
            char *formatted_str = g_strdup_printf("--- %d ---\nname: %s\nmedian: %s\n", number_of_results, airport, median_str);

            // Add the formatted string to the list
            formatted_resList = g_list_append(formatted_resList, formatted_str);

            // Free the memory for the tokens
            g_strfreev(tokens);

            // increment the number of results
            number_of_results++;

            // if it is the last element, we remove one line break
            if (resList_iter->next == NULL) {
                formatted_str[strlen(formatted_str) - 1] = '\0';
            }

            resList_iter = g_list_next(resList_iter);
        }

        // Free the memory for the result list
        g_list_free_full(*resList, g_free);

        // Set the result list to the formatted result list
        *resList = formatted_resList;
    }

    // Free the list of airport delays
    g_list_free_full(airport_delays, free_airport_delays);
}