/**
 * @file query_8.c
 * @brief Implementation of query 8.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/** REMODELACAO **/

/*
query_8: Apresentar a receita total de um hotel entre duas datas (inclusive), a partir do seu identificador.
As receitas de um hotel devem considerar apenas o preço por noite (price_per_night) de todas as
reservas com noites entre as duas datas. E.g., caso um hotel tenha apenas uma reserva de 100€/noite
de 2023/10/01 a 2023/10/10, e quisermos saber as receitas entre 2023/10/01 a 2023/10/02, deverá ser
retornado 200€ (duas noites). Por outro lado, caso a reserva seja entre 2023/10/01 a 2023/09/02,
deverá ser retornado 100€ (uma noite).
*/
// calculate_nights_i is the same as calculate_nights but it counts the nights between two dates (inclusive)
/**
 * @brief Returns the number of nights between two dates (inclusive).
 * 
 * @param begin_date The begin date.
 * @param end_date The end date.
 * @return int The number of nights.
 */
int calculate_nights_i(char *begin_date, char *end_date) {
    return calculate_nights(begin_date, end_date) + 1;
}
// calculate_total_price_wt its the same as calculate_total_price but without the city_tax
/**
 * @brief Returns the total price of a reservation.
 * 
 * @param price_per_night The price per night.
 * @param nights The number of nights.
 * @return int The total price.
 */
int calculate_total_price_wt(char *price_per_night, int nights) {
    return atoi(price_per_night) * nights;
}

/**
 * @brief Returns the total revenue of a hotel between two dates (inclusive), from its identifier.
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @return char* The result string.
 */
char* query_8(CATALOG *c, int format_flag, char **args, int args_size) {
    char* result = g_malloc(sizeof(char) * MAX_RES_SIZE);
    if (result == NULL) return NULL;
    result[0] = '\0';

    int number_of_results = 0;
    int total_revenue = 0;

    if (args_size == 3) {
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
                // get all the reservations between the two dates (inclusive)
                if (date_comparator_wt(args[1], get_reservation_info(reservation, 7)) >= 0 && date_comparator_wt(args[2], get_reservation_info(reservation, 8)) <= 0) {
                    int nights = calculate_nights_i(args[1], args[2]);
                    total_revenue += calculate_total_price_wt(get_reservation_info(reservation, 9), nights);
                }
                if (date_comparator_wt(args[1], get_reservation_info(reservation, 7)) <= 0 && date_comparator_wt(args[2], get_reservation_info(reservation, 8)) >= 0) {
                    int reservation_nights = calculate_nights_i(get_reservation_info(reservation, 7), get_reservation_info(reservation, 8))-1; // we do -1 because if args[1] or args[2] are before or after the get_reservation_info(reservation, 7) or get_reservation_info(reservation, 8) we dont want to count that night
                    int nights = calculate_nights_i(args[1], args[2])-1;
                    if (reservation_nights >= nights) {
                        total_revenue += calculate_total_price_wt(get_reservation_info(reservation, 9), nights);
                    } else {
                        total_revenue += calculate_total_price_wt(get_reservation_info(reservation, 9), reservation_nights);
                    }
                }
            }
        }
        number_of_results++;

        if (format_flag) {
            sprintf(result, "--- %d ---\nrevenue: %d\n", number_of_results, total_revenue);
        } else {
            sprintf(result, "%d\n", total_revenue);
        }
    }
    return result;
}