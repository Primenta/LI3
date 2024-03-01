/**
 * @file query_2.c
 * @brief Implementation of query 2.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/** REMODELACAO **/

/*
query_2: Listar os voos ou reservas de um utilizador, se o segundo argumento for flights ou reservations,
respetivamente, ordenados por data (da mais recente para a mais antiga)
Caso não seja fornecido um segundo argumento, apresentar voos e reservas, juntamente com o tipo (flight ou reservation).
Para os voos, date = schedule_departure_date (contudo, deverá ser descartada a componente das
horas/minutos/segundos no output), enquanto que para as reservas, date = begin_date.
Em caso de empate, ordenar pelo identificador (de forma crescente). Tal como na Q1, utilizadores com
account_status = “inactive” deverão ser ignorados.
Input and Output:
> 2 <ID> [flights|reservations]
id;date[;type]
*/
// Function to compare two users data for sorting (without format_flag)
/**
 * @brief Function to compare two users data for sorting (without format_flag).
 * 
 * @param a The first user data.
 * @param b The second user data.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_userdata(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *user_a = (char *)a;
    char *user_b = (char *)b;

    //printf("Comparing %s with %s\n", user_a, user_b);

    // split string into tokens using g_strsplit
    char **user_a_tokens = g_strsplit(user_a, ";", 0);
    char **user_b_tokens = g_strsplit(user_b, ";", 0);

    // get the date and id from the tokens
    char *user_a_date = user_a_tokens[1];
    char *user_b_date = user_b_tokens[1];

    char *user_a_id = user_a_tokens[0];
    char *user_b_id = user_b_tokens[0];

    // compare the date
    int date_cmp = date_comparator_wt(user_a_date, user_b_date);
    if (date_cmp == 0) {
        // if theres a tie, compare the id
        comparator = -strcmp(user_a_id, user_b_id);
    } else {
        comparator = date_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(user_a_tokens);
    g_strfreev(user_b_tokens);

    return comparator;
}

// Function to compare two users data for sorting (with format_flag)
/**
 * @brief Function to compare two users data for sorting (with format_flag).
 * 
 * @param a The first user data.
 * @param b The second user data.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_userdata_format(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *user_a = (char *)a;
    char *user_b = (char *)b;

    //printf("Comparing %s with %s\n", user_a, user_b);

    // split string into tokens using g_strsplit
    char **user_a_tokens = g_strsplit(user_a, "\n", 0);
    char **user_b_tokens = g_strsplit(user_b, "\n", 0);

    // get the date and id from the tokens
    char **user_a_date_tokens = g_strsplit(user_a_tokens[1], ": ", 0);
    char **user_b_date_tokens = g_strsplit(user_b_tokens[1], ": ", 0);

    char *user_a_date = user_a_date_tokens[1];
    char *user_b_date = user_b_date_tokens[1];

    char **user_a_id_tokens = g_strsplit(user_a_tokens[0], ": ", 0);
    char **user_b_id_tokens = g_strsplit(user_b_tokens[0], ": ", 0);

    char *user_a_id = user_a_id_tokens[1];
    char *user_b_id = user_b_id_tokens[1];

    // compare the date
    int date_cmp = date_comparator_wt(user_a_date, user_b_date);
    if (date_cmp == 0) {
        // if theres a tie, compare the id
        comparator = -strcmp(user_a_id, user_b_id);
    } else {
        comparator = date_cmp;
    }

    // reverse the order of the comparison to sort in descending order
    comparator *= -1;

    // free the tokens
    g_strfreev(user_a_tokens);
    g_strfreev(user_b_tokens);
    g_strfreev(user_a_date_tokens);
    g_strfreev(user_b_date_tokens);
    g_strfreev(user_a_id_tokens);
    g_strfreev(user_b_id_tokens);

    return comparator;
}

/**
 * @brief Returns the information of the flights or reservations of a user, if the second argument is flights or reservations, respectively, ordered by date (from most recent to oldest).
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param resList The result list.
 */
void query_2(CATALOG *c, int format_flag, char **args, int args_size, GList **resList) {
    *resList = NULL;

    // create two lists, one for flights and one for reservations
    GList *flightsList = NULL;
    GList *reservationsList = NULL;

    if (args_size < 1) {
        return; // Handle missing arguments
    }

    if (args_size == 1) { // we have the user_id and need to get the information for flights and reservations (and add ;flights or ;reservations where we found that data)
        // get the user
        USER *user = g_hash_table_lookup(getUsers(c), args[0]);
        // if we dont find the user we return
        if (user == NULL) return;
        // if the user is inactive we return
        if (isActive(get_user_info(user, 11)) == 0) return;
        // get the flights and reservations
        int number_of_flights = count_number_flights(getPassengers(c), args[0]);
        char** flights = get_user_flights(getPassengers(c), args[0]);
        int number_of_reservations = count_number_reservations(getReservations(c), args[0]);
        char** reservations = get_user_reservations(getReservations(c), args[0]);
        // iterate through the flights and reservations and add them to the list
        for (int i = 0; i < number_of_flights; i++) {
            // add the flight and date to the list
            FLIGHT *flight = g_hash_table_lookup(getFlights(c), flights[i]);
            if (flight == NULL) continue;
            char *str = NULL;
            char* formated_date = get_flight_info(flight, 6);
            // remove the time from the date
            char *formated_date_token = strtok(formated_date, " ");
            if (format_flag) {
                char *str_format = "id: %s\ndate: %s\ntype: flight\n\n";
                int len = snprintf(NULL, 0, str_format, get_flight_id(flight), formated_date_token);
                str = g_malloc(len + 1);
                snprintf(str, len + 1, str_format, get_flight_id(flight), formated_date_token);
            } else {
                char *str_format = "%s;%s;flight\n";
                int len = snprintf(NULL, 0, str_format, get_flight_id(flight), formated_date_token);
                str = g_malloc(len + 1);
                snprintf(str, len + 1, str_format, get_flight_id(flight), formated_date_token);
            }
            flightsList = g_list_append(flightsList, str);
            g_free(flights[i]);
            // free the copy of the string
            g_free(formated_date);
        }
        for (int i = 0; i < number_of_reservations; i++) {
            // add the reservation and date to the list
            RESERVATION *reservation = g_hash_table_lookup(getReservations(c), reservations[i]);
            if (reservation == NULL) continue;
            char *str = NULL;
            if (format_flag) {
                char *str_format = "id: %s\ndate: %s\ntype: reservation\n\n";
                int len = snprintf(NULL, 0, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7);
                str = g_malloc(len + 1);
                snprintf(str, len + 1, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
            } else {
                char *str_format = "%s;%s;reservation\n";
                int len = snprintf(NULL, 0, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
                str = g_malloc(len + 1);
                snprintf(str, len + 1, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
            }
            reservationsList = g_list_append(reservationsList, str);
            g_free(reservations[i]);
        }
        g_free(flights);
        g_free(reservations);

        // add to the list the flights and reservations
        *resList = g_list_concat(flightsList, reservationsList);

        // after having all the data we sort the list
        if (format_flag) {
            // if theres a format flag we sort but the "--- %d ---\n" must be added to the string after sorting them so "--- 1 ---" if always the first line and so on, but the data inside is sorted
            *resList = g_list_sort(*resList, (GCompareFunc)compare_userdata_format);
            // after sort we add the "--- %d ---\n" to the string
            GList *iterList = *resList;
            int number_of_results = 0;
            while (iterList != NULL) {
                char *userStr = iterList->data;
                char *str = "--- %d ---\n%s";
                int len = snprintf(NULL, 0, str, number_of_results + 1, userStr);
                char *userStrFormatted = g_malloc(len + 1);
                // if it is the last line, we remove the \n
                if (iterList->next == NULL) {
                    snprintf(userStrFormatted, len + 1, str, number_of_results + 1, userStr);
                    userStrFormatted[len - 1] = '\0';
                } else {
                    snprintf(userStrFormatted, len + 1, str, number_of_results + 1, userStr);
                }
                g_free(userStr);
                iterList->data = userStrFormatted;
                iterList = g_list_next(iterList);
                number_of_results++;
            }
        } else {
            *resList = g_list_sort(*resList, (GCompareFunc)compare_userdata);
        }
    } else { // we have 2 args, the user_id and the hash table where we want to take the data from (only show the data)
        if (strcmp(args[1], "flights") == 0) {
            int number_of_flights = count_number_flights(getPassengers(c), args[0]);
            char** flights = get_user_flights(getPassengers(c), args[0]);
            for (int i = 0; i < number_of_flights; i++) {
                // add the flight and date to the list
                FLIGHT *flight = g_hash_table_lookup(getFlights(c), flights[i]);
                if (flight == NULL) continue;
                char *str = NULL;
                //char* formated_date = strtok(flight->schedule_departure_date, " ");
                // create a copy of the string so we dont modify the original
                char *formated_date = get_flight_info(flight, 6);
                // remove the time from the date
                char *formated_date_token = strtok(formated_date, " ");
                if (format_flag) {
                    char *str_format = "id: %s\ndate: %s\n\n";
                    int len = snprintf(NULL, 0, str_format, get_flight_id(flight), formated_date_token);
                    str = g_malloc(len + 1);
                    snprintf(str, len + 1, str_format, get_flight_id(flight), formated_date_token);
                } else {
                    char *str_format = "%s;%s\n";
                    int len = snprintf(NULL, 0, str_format, get_flight_id(flight), formated_date_token);
                    str = g_malloc(len + 1);
                    snprintf(str, len + 1, str_format, get_flight_id(flight), formated_date_token);
                }
                flightsList = g_list_append(flightsList, str);
                g_free(flights[i]);
                // free the copy of the string
                g_free(formated_date);
            }
            g_free(flights);
        } else if (strcmp(args[1], "reservations") == 0) {
            int number_of_reservations = count_number_reservations(getReservations(c), args[0]);
            char** reservations = get_user_reservations(getReservations(c), args[0]);
            for (int i = 0; i < number_of_reservations; i++) {
                // add the reservation and date to the list
                RESERVATION *reservation = g_hash_table_lookup(getReservations(c), reservations[i]);
                if (reservation == NULL) continue;
                char *str = NULL;
                if (format_flag) {
                    char *str_format = "id: %s\ndate: %s\n\n";
                    int len = snprintf(NULL, 0, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
                    str = g_malloc(len + 1);
                    snprintf(str, len + 1, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
                } else {
                    char *str_format = "%s;%s\n";
                    int len = snprintf(NULL, 0, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
                    str = g_malloc(len + 1);
                    snprintf(str, len + 1, str_format, get_reservation_id(reservation), get_reservation_info(reservation, 7));
                }
                reservationsList = g_list_append(reservationsList, str);
                g_free(reservations[i]);
            }
            g_free(reservations);
        } else {
            return; // Handle invalid arguments
        }

        // add to the list the flights and reservations
        *resList = g_list_concat(flightsList, reservationsList);

        // after having all the data we sort the list
        if (format_flag) {
            // if theres a format flag we sort but the "--- %d ---\n" must be added to the string after sorting them so "--- 1 ---" if always the first line and so on, but the data inside is sorted
            *resList = g_list_sort(*resList, (GCompareFunc)compare_userdata_format);
            // after sort we add the "--- %d ---\n" to the string
            GList *iterList = *resList;
            int number_of_results = 0;
            while (iterList != NULL) {
                char *userStr = iterList->data;
                char *str = "--- %d ---\n%s";
                int len = snprintf(NULL, 0, str, number_of_results + 1, userStr);
                char *userStrFormatted = g_malloc(len + 1);
                // if it is the last line, we remove the \n
                if (iterList->next == NULL) {
                    snprintf(userStrFormatted, len + 1, str, number_of_results + 1, userStr);
                    userStrFormatted[len - 1] = '\0';
                } else {
                    snprintf(userStrFormatted, len + 1, str, number_of_results + 1, userStr);
                }
                g_free(userStr);
                iterList->data = userStrFormatted;
                iterList = g_list_next(iterList);
                number_of_results++;
            }
        } else {
            *resList = g_list_sort(*resList, (GCompareFunc)compare_userdata);
        }
    }
}