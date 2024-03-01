/**
 * @file query_5.c
 * @brief Implementation of query 5.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/*
query_5: Listar os voos com origem num dado aeroporto, entre duas datas, ordenados por data de partida
estimada (da mais recente para a mais antiga). Um voo está entre <begin_date> e <end_date> caso a
sua respetiva data estimada de partida esteja entre <begin_date> e <end_date> (ambos inclusivos).
Caso dois voos tenham a mesma data, o identificador do voo deverá ser usado como critério de
desempate (de forma crescente).
*/
// Function to compare two flights departure date for sorting (without format_flag)
/**
 * @brief Function to compare two flights departure date for sorting (without format_flag).
 * 
 * @param a The first flight departure date.
 * @param b The second flight departure date.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_flightdeparture(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *flight_a = (char *)a;
    char *flight_b = (char *)b;

    //printf("Comparing %s with %s\n", flight_a, flight_b);

    // split string into tokens using g_strsplit
    char **flight_a_tokens = g_strsplit(flight_a, ";", 0);
    char **flight_b_tokens = g_strsplit(flight_b, ";", 0);

    // get the schedule_departure_date and id from the tokens
    char *flight_a_schedule_departure_date = flight_a_tokens[1];
    char *flight_b_schedule_departure_date = flight_b_tokens[1];

    char *flight_a_id = flight_a_tokens[0];
    char *flight_b_id = flight_b_tokens[0];

    // compare the schedule_departure_date
    int schedule_departure_date_cmp = date_comparator(flight_a_schedule_departure_date, flight_b_schedule_departure_date);
    if (schedule_departure_date_cmp == 0) {
        // if theres a tie, compare the id
        comparator = -strcmp(flight_a_id, flight_b_id);
    } else {
        comparator = schedule_departure_date_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(flight_a_tokens);
    g_strfreev(flight_b_tokens);

    return comparator;
}

// Function to compare two flights departure date for sorting (with format_flag)
/**
 * @brief Function to compare two flights departure date for sorting (with format_flag).
 * 
 * @param a The first flight departure date.
 * @param b The second flight departure date.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_flightdeparture_format(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *flight_a = (char *)a;
    char *flight_b = (char *)b;

    //printf("Comparing %s with %s\n", flight_a, flight_b);

    // split string into tokens using g_strsplit
    char **flight_a_tokens = g_strsplit(flight_a, "\n", 0);
    char **flight_b_tokens = g_strsplit(flight_b, "\n", 0);

    // get the schedule_departure_date and id from the tokens
    char **flight_a_schedule_departure_date_tokens = g_strsplit(flight_a_tokens[1], ": ", 0);
    char **flight_b_schedule_departure_date_tokens = g_strsplit(flight_b_tokens[1], ": ", 0);

    char *flight_a_schedule_departure_date = flight_a_schedule_departure_date_tokens[1];
    char *flight_b_schedule_departure_date = flight_b_schedule_departure_date_tokens[1];

    char **flight_a_id_tokens = g_strsplit(flight_a_tokens[0], ": ", 0);
    char **flight_b_id_tokens = g_strsplit(flight_b_tokens[0], ": ", 0);

    char *flight_a_id = flight_a_id_tokens[1];
    char *flight_b_id = flight_b_id_tokens[1];

    // compare the schedule_departure_date
    int schedule_departure_date_cmp = date_comparator(flight_a_schedule_departure_date, flight_b_schedule_departure_date);
    if (schedule_departure_date_cmp == 0) {
        // if theres a tie, compare the id
        comparator = -strcmp(flight_a_id, flight_b_id);
    } else {
        comparator = schedule_departure_date_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(flight_a_tokens);
    g_strfreev(flight_b_tokens);
    g_strfreev(flight_a_schedule_departure_date_tokens);
    g_strfreev(flight_b_schedule_departure_date_tokens);
    g_strfreev(flight_a_id_tokens);
    g_strfreev(flight_b_id_tokens);

    return comparator;
}

/**
 * @brief Returns the flights with origin in a given airport, between two dates, ordered by estimated departure date (from most recent to oldest). A flight is between <begin_date> and <end_date> if its respective estimated departure date is between <begin_date> and <end_date> (both inclusive). If two flights have the same date, the flight identifier should be used as a tiebreaker (in ascending order).
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param resList The result list.
 */
void query_5(CATALOG *c, int format_flag, char **args, int args_size, GList **resList) {
    *resList = NULL;

    if (args_size < 3) {
        return;
    }

    char *airport_name = args[0];
    char *begin_date = args[1];
    char *end_date = args[2];

    // iterate through the flights hash table
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, c->flights);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        FLIGHT *flight = (FLIGHT *)value;
        // check if the flight is from the airport_name
        // but both strings to uppercase so we dont have to worry about case sensitivity
        char *airport_name_upper = toupper_str(airport_name);
        char *flight_origin_upper = toupper_str(flight->origin);
        char *flight_destination_upper = toupper_str(flight->destination);
        if (strcmp(flight_origin_upper, airport_name_upper) == 0) {
            // check if the flight is between begin_date and end_date
            if (date_comparator_wt(flight->schedule_departure_date, begin_date) >= 0 && date_comparator_wt(flight->schedule_arrival_date, end_date) <= 0) {
                char* flightStr = NULL;

                if (format_flag) { // Format the output
                    char *str = "id: %s\nschedule_departure_date: %s\ndestination: %s\nairline: %s\nplane_model: %s\n\n";
                    int len = snprintf(NULL, 0, str, flight->id, flight->schedule_departure_date, flight_destination_upper, flight->airline, flight->plane_model);
                    flightStr = g_malloc(len + 1);
                    snprintf(flightStr, len + 1, str, flight->id, flight->schedule_departure_date, flight_destination_upper, flight->airline, flight->plane_model);
                } else {
                    char *str = "%s;%s;%s;%s;%s\n";
                    int len = snprintf(NULL, 0, str, flight->id, flight->schedule_departure_date, flight_destination_upper, flight->airline, flight->plane_model);
                    flightStr = g_malloc(len + 1);
                    snprintf(flightStr, len + 1, str, flight->id, flight->schedule_departure_date, flight_destination_upper, flight->airline, flight->plane_model);
                }

                *resList = g_list_append(*resList, flightStr);
            }
        }
        // free the strings (because we used toupper_str and it uses strdup)
        g_free(airport_name_upper);
        g_free(flight_origin_upper);
        g_free(flight_destination_upper);
    }

    // Sort the list based on schedule_departure_date and id
    if (format_flag) {
        // if theres a format flag we sort but the "--- %d ---\n" must be added to the string after sorting them so "--- 1 ---" if always the first line and so on, but the data inside is sorted
        *resList = g_list_sort(*resList, (GCompareFunc)compare_flightdeparture_format);
        // after sort we add the "--- %d ---\n" to the string
        GList *iterList = *resList;
        int number_of_results = 0;
        while (iterList != NULL) {
            char *flightStr = iterList->data;
            char *str = "--- %d ---\n%s";
            int len = snprintf(NULL, 0, str, number_of_results + 1, flightStr);
            char *flightStrFormatted = g_malloc(len + 1);
            // if it is the last line, we remove the \n
            if (iterList->next == NULL) {
                snprintf(flightStrFormatted, len + 1, str, number_of_results + 1, flightStr);
                flightStrFormatted[len - 1] = '\0';
            } else {
                snprintf(flightStrFormatted, len + 1, str, number_of_results + 1, flightStr);
            }
            g_free(flightStr);
            iterList->data = flightStrFormatted;
            iterList = g_list_next(iterList);
            number_of_results++;
        }
    } else {
        *resList = g_list_sort(*resList, (GCompareFunc)compare_flightdeparture);
    }
}