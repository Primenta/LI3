/**
 * @file query_4.c
 * @brief Implementation of query 4.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/*
query_4: Listar as reservas de um hotel, ordenadas por data de início (da mais recente para a mais
antiga). Caso duas reservas tenham a mesma data, deve ser usado o identificador da reserva como
critério de desempate (de forma crescente).
Input and Output:
> 4 <hotel_id>
id;begin_date;end_date;user_id;rating;total_price
*/
// Function to compare two reservations for sorting (without format_flag)
/**
 * @brief Function to compare two reservations for sorting (without format_flag).
 * 
 * @param a The first reservation.
 * @param b The second reservation.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_reservations(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *reservation_a = (char *)a;
    char *reservation_b = (char *)b;

    //printf("Comparing %s with %s\n", reservation_a, reservation_b);

    // split string into tokens using g_strsplit
    char **reservation_a_tokens = g_strsplit(reservation_a, ";", 0);
    char **reservation_b_tokens = g_strsplit(reservation_b, ";", 0);

    // get the begin_date and id from the tokens
    char *reservation_a_begin_date = reservation_a_tokens[1];
    char *reservation_b_begin_date = reservation_b_tokens[1];

    char *reservation_a_id = reservation_a_tokens[0];
    char *reservation_b_id = reservation_b_tokens[0];

    // compare the begin_date
    int begin_date_cmp = strcmp(reservation_a_begin_date, reservation_b_begin_date);
    if (begin_date_cmp == 0) {
        // if theres a tie, compare the id
        comparator = -strcmp(reservation_a_id, reservation_b_id); // its reversed because thats how its given in the expected output
    } else {
        comparator = begin_date_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(reservation_a_tokens);
    g_strfreev(reservation_b_tokens);

    return comparator;
}

// Function to compare two reservations for sorting (with format_flag)
/**
 * @brief Function to compare two reservations for sorting (with format_flag).
 * 
 * @param a The first reservation.
 * @param b The second reservation.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_reservations_format(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *reservation_a = (char *)a;
    char *reservation_b = (char *)b;

    //printf("Comparing %s with %s\n", reservation_a, reservation_b);

    // split string into tokens using g_strsplit
    char **reservation_a_tokens = g_strsplit(reservation_a, "\n", 0);
    char **reservation_b_tokens = g_strsplit(reservation_b, "\n", 0);

    // get the begin_date and id from the tokens
    char **reservation_a_begin_date_tokens = g_strsplit(reservation_a_tokens[1], ": ", 0);
    char **reservation_b_begin_date_tokens = g_strsplit(reservation_b_tokens[1], ": ", 0);

    char *reservation_a_begin_date = reservation_a_begin_date_tokens[1];
    char *reservation_b_begin_date = reservation_b_begin_date_tokens[1];

    char **reservation_a_id_tokens = g_strsplit(reservation_a_tokens[0], ": ", 0);
    char **reservation_b_id_tokens = g_strsplit(reservation_b_tokens[0], ": ", 0);

    char *reservation_a_id = reservation_a_id_tokens[1];
    char *reservation_b_id = reservation_b_id_tokens[1];

    // compare the begin_date
    int begin_date_cmp = strcmp(reservation_a_begin_date, reservation_b_begin_date);
    if (begin_date_cmp == 0) {
        // if theres a tie, compare the id
        comparator = -strcmp(reservation_a_id, reservation_b_id); // its reversed because thats how its given in the expected output
    } else {
        comparator = begin_date_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(reservation_a_tokens);
    g_strfreev(reservation_b_tokens);
    g_strfreev(reservation_a_begin_date_tokens);
    g_strfreev(reservation_b_begin_date_tokens);
    g_strfreev(reservation_a_id_tokens);
    g_strfreev(reservation_b_id_tokens);

    return comparator;
}

/**
 * @brief Returns the reservations of a hotel, ordered by start date (from most recent to oldest). If two reservations have the same date, the reservation identifier should be used as a tiebreaker (in ascending order).
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param reservationsList The result list.
 */
void query_4(CATALOG *c, int format_flag, char **args, int args_size, GList **reservationsList) {
    *reservationsList = NULL;

    if (args_size < 1) {
        return; // Handle missing arguments
    }

    // iterate through the reservations hash table
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, c->reservations);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        RESERVATION *reservation = (RESERVATION *)value;
        // check if the reservation is from the hotel_id
        if (strcmp(reservation->hotel_id, args[0]) == 0) {
            double total_price = calculate_total_price(reservation->price_per_night, calculate_nights(reservation->begin_date, reservation->end_date), atoi(reservation->city_tax));
            char* reservationStr = NULL;

            if (format_flag) { // Format the output
                char *str = "id: %s\nbegin_date: %s\nend_date: %s\nuser_id: %s\nrating: %s\ntotal_price: %.3f\n\n";
                int len = snprintf(NULL, 0, str, reservation->id, reservation->begin_date, reservation->end_date, reservation->user_id, reservation->rating, total_price);
                reservationStr = g_malloc(len + 1);
                snprintf(reservationStr, len + 1, str, reservation->id, reservation->begin_date, reservation->end_date, reservation->user_id, reservation->rating, total_price);
            } else {
                char *str = "%s;%s;%s;%s;%s;%.3f\n";
                int len = snprintf(NULL, 0, str, reservation->id, reservation->begin_date, reservation->end_date, reservation->user_id, reservation->rating, total_price);
                reservationStr = g_malloc(len + 1);
                snprintf(reservationStr, len + 1, str, reservation->id, reservation->begin_date, reservation->end_date, reservation->user_id, reservation->rating, total_price);
            }

            *reservationsList = g_list_append(*reservationsList, reservationStr);
        }
    }

    // Sort the list based on begin_date and id
    if (format_flag) {
        // if theres a format flag we sort but the "--- %d ---\n" must be added to the string after sorting them so "--- 1 ---" if always the first line and so on, but the data inside is sorted
        *reservationsList = g_list_sort(*reservationsList, (GCompareFunc)compare_reservations_format);
        // after sort we add the "--- %d ---\n" to the string
        GList *iterList = *reservationsList;
        int number_of_results = 0;
        while (iterList != NULL) {
            char *reservationStr = iterList->data;
            char *str = "--- %d ---\n%s";
            int len = snprintf(NULL, 0, str, number_of_results + 1, reservationStr);
            char *reservationStrFormatted = g_malloc(len + 1);
            // if it is the last line, we remove the \n
            if (iterList->next == NULL) {
                snprintf(reservationStrFormatted, len + 1, str, number_of_results + 1, reservationStr);
                reservationStrFormatted[len - 1] = '\0';
            } else {
                snprintf(reservationStrFormatted, len + 1, str, number_of_results + 1, reservationStr);
            }
            g_free(reservationStr);
            iterList->data = reservationStrFormatted;
            iterList = g_list_next(iterList);
            number_of_results++;
        }
    } else {
        *reservationsList = g_list_sort(*reservationsList, (GCompareFunc)compare_reservations);
    }
}