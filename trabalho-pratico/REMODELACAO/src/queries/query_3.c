/**
 * @file query_3.c
 * @brief Implementation of query 3.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/** REMODELACAO **/

/*
query_3: Apresentar a classificação média de um hotel, a partir do seu identificador.
*/
/**
 * @brief Returns the average rating of a hotel, from its identifier.
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @return char* The result string.
 */
char* query_3(CATALOG *c, int format_flag, char **args, int args_size) {
    char* result = g_malloc(sizeof(char) * MAX_RES_SIZE);
    if (result == NULL) return NULL;
    result[0] = '\0';

    int number_of_results = 0;
    double average_rating = 0;
    int count = 0;

    if (args_size == 1) {
        // get the reservation->hotel_id
        // we cant use g_hash_table_lookup because we dont have the reservation id we need reservation->hotel_id
        // so we have to iterate through the hash table if the reservation->hote_id == args[0] we add reservation->stars to the average
        // in the end we divide the average by the number of lines that we found with the same hotel_id (count)
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, getReservations(c));

        while (g_hash_table_iter_next(&iter, &key, &value)) {
            RESERVATION *reservation = (RESERVATION *)value;
            if (strcmp(get_reservation_info(reservation, 2), args[0]) == 0) {
                average_rating += atof(get_reservation_info(reservation, 11));
                count++;
            }
        }
        if (count == 0) return result;
        number_of_results++;

        average_rating /= count;

        if (format_flag) {
            sprintf(result, "--- %d ---\nrating: %.3f\n", number_of_results, average_rating);
        } else {
            sprintf(result, "%.3f\n", average_rating);
        }
    }
    return result;
}