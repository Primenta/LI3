/**
 * @file query_6.c
 * @brief Implementation of query 6.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/** REMODELACAO **/

/*
query_6: Listar o top N aeroportos com mais passageiros, para um dado ano. Deverão ser contabilizados
os voos com a data estimada de partida nesse ano. Caso dois aeroportos tenham o mesmo valor,
deverá ser usado o nome do aeroporto como critério de desempate (de forma crescente).
*/
// Function to compare two airports passenger count for sorting
/**
 * @brief Function to compare two airports passenger count for sorting.
 * 
 * @param a The first airport passenger count.
 * @param b The second airport passenger count.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_airport_passengers(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *airport_passengers_a = (char *)a;
    char *airport_passengers_b = (char *)b;

    //printf("Comparing %s with %s\n", airport_passengers_a, airport_passengers_b);

    // split string into tokens using g_strsplit
    char **airport_passengers_a_tokens = g_strsplit(airport_passengers_a, ";", 0);
    char **airport_passengers_b_tokens = g_strsplit(airport_passengers_b, ";", 0);

    // get the airport_passengers from the tokens
    char *airport_passengers_a_airport = airport_passengers_a_tokens[0];
    char *airport_passengers_b_airport = airport_passengers_b_tokens[0];

    int airport_passengers_a_passengers = atoi(airport_passengers_a_tokens[1]);
    int airport_passengers_b_passengers = atoi(airport_passengers_b_tokens[1]);

    // compare the passengers
    int passengers_cmp = airport_passengers_a_passengers - airport_passengers_b_passengers;
    if (passengers_cmp == 0) {
        // if theres a tie, compare the airport
        comparator = -strcmp(airport_passengers_a_airport, airport_passengers_b_airport);
    } else {
        comparator = passengers_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(airport_passengers_a_tokens);
    g_strfreev(airport_passengers_b_tokens);

    return comparator;
}

// Helper function to update airport passengers list
/**
 * @brief Helper function to update airport passengers list.
 * 
 * @param airport_passengers_list The airport passengers list.
 * @param airport_name The airport name.
 * @param number_of_passengers The number of passengers.
 */
void update_airport_passengers(GList **airport_passengers_list, const char *airport_name, int number_of_passengers) {
    char *airport_name_formated = toupper_str(airport_name);

    GList *iterList = *airport_passengers_list;
    int found = 0;

    while (iterList != NULL) {
        char *airport_passengers = iterList->data;
        char **airport_passengers_tokens = g_strsplit(airport_passengers, ";", 0);
        char *existing_airport_name = airport_passengers_tokens[0];

        if (strcmp(existing_airport_name, airport_name_formated) == 0) {
            // If the airport already exists in the list, add the number_of_passengers to the count
            int airport_passengers_count = atoi(airport_passengers_tokens[1]);
            airport_passengers_count += number_of_passengers;
            g_free(airport_passengers);
            iterList->data = g_strdup_printf("%s;%d", airport_name_formated, airport_passengers_count);
            found = 1;
            g_strfreev(airport_passengers_tokens);
            break;
        }

        g_strfreev(airport_passengers_tokens);

        iterList = g_list_next(iterList);
    }

    if (found == 0) {
        // If not, add the airport to the list
        *airport_passengers_list = g_list_append(*airport_passengers_list, g_strdup_printf("%s;%d", airport_name_formated, number_of_passengers));
    }

    // Free the strings
    g_free(airport_name_formated);
}

/**
 * @brief Returns the top N airports with the most passengers, for a given year. Flights with the estimated departure date in that year should be counted. If two airports have the same value, the airport name should be used as a tiebreaker (in ascending order).
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param resList The result list.
 */
void query_6(CATALOG *c, int format_flag, char **args, int args_size, GList **resList) {
    *resList = NULL;

    if (args_size < 2) {
        return;
    }

    int year = atoi(args[0]);
    int N = atoi(args[1]);

    GHashTableIter iter;
    gpointer key, value;

    // variable to save the airport passenger count and the airport name (must be, per airport, so we need a list)
    GList *airport_passengers_list = NULL;

    g_hash_table_iter_init(&iter, getFlights(c));
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        FLIGHT *flight = (FLIGHT *)value;

        // get the year from the schedule_departure_date
        char *schedule_departure_date_copy = g_strdup(get_flight_info(flight, 6)); // use g_strdup instead of strdup because we are using g_free to free the list
        char *year_str = strtok(schedule_departure_date_copy, "/");
        int flight_year = atoi(year_str);
        g_free(schedule_departure_date_copy);

        if (flight_year == year) {
            // Use a helper function to avoid redundancy in handling departure and arrival airports
            int number_of_passengers = get_flight_passengers(getPassengers(c), get_flight_id(flight));
            update_airport_passengers(&airport_passengers_list, get_flight_info(flight, 4), number_of_passengers);
            update_airport_passengers(&airport_passengers_list, get_flight_info(flight, 5), number_of_passengers);
        }
    }

    // Sort the list based on number of passengers and airport name
    airport_passengers_list = g_list_sort(airport_passengers_list, (GCompareFunc)compare_airport_passengers);

    // Add the top N airports to the result list
    GList *iterList = airport_passengers_list;
    int number_of_results = 0;
    while (iterList != NULL && number_of_results < N) {
        char *airport_passengers = iterList->data;
        char **airport_passengers_tokens = g_strsplit(airport_passengers, ";", 0);
        char *airport_name = airport_passengers_tokens[0];
        int airport_passengers_count = atoi(airport_passengers_tokens[1]);

        char *str = NULL;
        if (format_flag) {
            char *str_format = "--- %d ---\nname: %s\npassengers: %d\n\n";
            int len = snprintf(NULL, 0, str_format, number_of_results + 1, airport_name, airport_passengers_count);
            str = g_malloc(len + 1);
            // if it is the last line, we remove the \n (here we cant do it like in other queries because we dont have the next element)
            // so we use N, if it is the last element we remove the \n
            if (number_of_results + 1 == N) {
                snprintf(str, len + 1, str_format, number_of_results + 1, airport_name, airport_passengers_count);
                str[len - 1] = '\0';
            } else {
                snprintf(str, len + 1, str_format, number_of_results + 1, airport_name, airport_passengers_count);
            }
        } else {
            char *str_format = "%s;%d\n";
            int len = snprintf(NULL, 0, str_format, airport_name, airport_passengers_count);
            str = g_malloc(len + 1);
            snprintf(str, len + 1, str_format, airport_name, airport_passengers_count);
        }

        *resList = g_list_append(*resList, str);

        g_strfreev(airport_passengers_tokens);

        iterList = g_list_next(iterList);
        number_of_results++;
    }

    // free airport_passengers_list
    g_list_free_full(airport_passengers_list, g_free);
}