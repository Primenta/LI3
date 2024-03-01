/**
 * @file query_10.c
 * @brief Implementation of query 10.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

/*
query_10: Apresentar várias métricas gerais da aplicação. As métricas consideradas são: número de novos utilizadores registados (de acordo com o campo account_creation); número de voos (de acordo
com o campo schedule_departure_date); número de passageiros; número de passageiros únicos; e número de reservas (de acordo com o campo begin_date). Caso a query seja executada sem argumentos, apresentar os dados agregados por ano, para todos os anos que a aplicação tem registo.
Caso a query seja executada com um argumento, year, apresentar os dados desse ano agregados
por mês. Finalmente, caso a query seja executada com dois argumentos, year e month, apresentar
os dados desse ano e mês agregados por dia. O output deverá ser ordenado de forma crescente
consoante o ano/mês/dia.
*/
// Function to add a value to a list (given a unique id), if the id already exists in the list, it adds the value to the existing value in the wantedPos, if 1 it add to the first column, etc, the column 0 is the uid "year;number_of_users;number_of_flights;number_of_passengers;number_of_unique_passengers;number_of_reservations"
void add_to_ymetrics(GList **metrics, const char *uid, int val, int wantedPos) {
    GList *iterList = *metrics;
    int found = 0;

    while (iterList != NULL) {
        char *uid_val = iterList->data;
        char **uid_val_tokens = g_strsplit(uid_val, ";", 0);
        char *existing_uid = uid_val_tokens[0];
        int number_of_users = atoi(uid_val_tokens[1]);
        int number_of_flights = atoi(uid_val_tokens[2]);
        int number_of_passengers = atoi(uid_val_tokens[3]);
        int number_of_unique_passengers = atoi(uid_val_tokens[4]);
        int number_of_reservations = atoi(uid_val_tokens[5]);

        if (strcmp(existing_uid, uid) == 0) {
            // If the uid already exists in the list, add the val to the count
            switch (wantedPos) {
                case 1:
                    number_of_users += val;
                    break;
                case 2:
                    number_of_flights += val;
                    break;
                case 3:
                    number_of_passengers += val;
                    break;
                case 4:
                    number_of_unique_passengers += val;
                    break;
                case 5:
                    number_of_reservations += val;
                    break;
                default:
                    break;
            }
            g_free(uid_val);
            iterList->data = g_strdup_printf("%s;%d;%d;%d;%d;%d", uid, number_of_users, number_of_flights, number_of_passengers, number_of_unique_passengers, number_of_reservations);
            found = 1;
            g_strfreev(uid_val_tokens);
            break;
        }

        g_strfreev(uid_val_tokens);

        iterList = g_list_next(iterList);
    }

    if (found == 0) {
        // If not, add the uid to the list
        switch (wantedPos) {
            case 1:
                *metrics = g_list_append(*metrics, g_strdup_printf("%s;%d;0;0;0;0", uid, val));
                break;
            case 2:
                *metrics = g_list_append(*metrics, g_strdup_printf("%s;0;%d;0;0;0", uid, val));
                break;
            case 3:
                *metrics = g_list_append(*metrics, g_strdup_printf("%s;0;0;%d;0;0", uid, val));
                break;
            case 4:
                *metrics = g_list_append(*metrics, g_strdup_printf("%s;0;0;0;%d;0", uid, val));
                break;
            case 5:
                *metrics = g_list_append(*metrics, g_strdup_printf("%s;0;0;0;0;%d", uid, val));
                break;
            default:
                break;
        }

        // Free the memory for the existing metric data
        if (iterList != NULL) {
            g_free(iterList->data);
        }
    }
}

// compare_list_dates picks the first element in "year;number_of_passengers;..." (picks the year), and sorts them by crescent order
static gint compare_list_dates(gconstpointer a, gconstpointer b) {
    int comparator = 0;

    char *date_a = (char *)a;
    char *date_b = (char *)b;

    //printf("Comparing %s with %s\n", date_a, date_b);

    // split string into tokens using g_strsplit
    char **date_a_tokens = g_strsplit(date_a, ";", 0);
    char **date_b_tokens = g_strsplit(date_b, ";", 0);

    // get the year from the tokens
    char *date_a_year = date_a_tokens[0];
    char *date_b_year = date_b_tokens[0];

    // compare the year
    int year_cmp = atoi(date_a_year) - atoi(date_b_year);
    if (year_cmp == 0) {
        // if theres a tie, compare the month
        comparator = atoi(date_a_year) - atoi(date_b_year);
    } else {
        comparator = year_cmp;
    }

    // free the tokens
    g_strfreev(date_a_tokens);
    g_strfreev(date_b_tokens);

    return comparator;
}


/**
 * @brief Returns general metrics of the application.
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param resList The result list.
 */
void query_10(CATALOG *c, int format_flag, char **args, int args_size, GList **resList) {
    *resList = NULL;

    // get the year and month from the args
    /*int year = 0;
    int month = 0;
    if (args_size == 1) {
        year = atoi(args[0]);
    } else if (args_size == 2) {
        year = atoi(args[0]);
        month = atoi(args[1]);
    }*/
    (void) args;

    // if the year is not given, we get the information for all years
    if (args_size == 0) {
        // list of years and their metrics
        GList *years_metrics = NULL;

        // Iterate through the users in the catalog
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, c->users);

        while (g_hash_table_iter_next(&iter, &key, &value)) {
            USER *user = (USER *)value;

            // get the year from the account_creation
            char *account_creation_copy = g_strdup(user->account_creation);
            char *year_str = strtok(account_creation_copy, "/");

            //printf("found %s user with account_creation %s\n", user->name, year_str);

            // add the user to the list
            add_to_ymetrics(&years_metrics, year_str, 1, 1);

            // free the string
            g_free(account_creation_copy);
        }

        // get the biggest year in years_metrics
        int biggest_year = 0;
        GList *iterList2 = years_metrics;
        while (iterList2 != NULL) {
            char *year_metrics = iterList2->data;
            char **year_metrics_tokens = g_strsplit(year_metrics, ";", 0);
            char *year = year_metrics_tokens[0];
            int year_int = atoi(year);
            if (year_int > biggest_year) {
                biggest_year = year_int;
            }
            g_strfreev(year_metrics_tokens);
            iterList2 = g_list_next(iterList2);
        }

        // from the biggest year to the current year, we add the year to the list if it doesnt exist
        for (int i = biggest_year; i <= 2023; i++) {
            char *year_str = g_strdup_printf("%d", i);
            add_to_ymetrics(&years_metrics, year_str, 0, 1);
            g_free(year_str);
        }

        // after the first iteration, we have the years in the list, now we need to add the other metrics
        // Iterate through the flights in the catalog
        GHashTableIter iter2;
        gpointer key2, value2;
        g_hash_table_iter_init(&iter2, c->flights);

        while (g_hash_table_iter_next(&iter2, &key2, &value2)) {
            FLIGHT *flight = (FLIGHT *)value2;

            // get the year from the schedule_departure_date
            char *schedule_departure_date_copy = g_strdup(flight->schedule_departure_date);
            char *year_str = strtok(schedule_departure_date_copy, "/");
            // add the flight to the list
            add_to_ymetrics(&years_metrics, year_str, 1, 2);

            // free the string
            g_free(schedule_departure_date_copy);
        }

        // Iterate through the reservations in the catalog
        GHashTableIter iter3;
        gpointer key3, value3;
        g_hash_table_iter_init(&iter3, c->reservations);

        while (g_hash_table_iter_next(&iter3, &key3, &value3)) {
            RESERVATION *reservation = (RESERVATION *)value3;

            // get the year from the begin_date
            char *begin_date_copy = g_strdup(reservation->begin_date);
            char *year_str = strtok(begin_date_copy, "/");
            // add the reservation to the list
            add_to_ymetrics(&years_metrics, year_str, 1, 5);

            // free the string
            g_free(begin_date_copy);
        }

        // Iterate through the passengers
        GHashTableIter iter4;
        gpointer key4, value4;
        g_hash_table_iter_init(&iter4, c->passengers);

        while (g_hash_table_iter_next(&iter4, &key4, &value4)) {
            FLIGHT_SEATS *flight_seats = (FLIGHT_SEATS *)value4;

            // go through the flights and get the passengers
            GHashTableIter iter5;
            gpointer key5, value5;
            g_hash_table_iter_init(&iter5, c->flights);

            while (g_hash_table_iter_next(&iter5, &key5, &value5)) {
                FLIGHT *flight = (FLIGHT *)value5;
                if (strcmp(flight->id, flight_seats->flight_id) == 0) {
                    // get the year from the schedule_departure_date
                    char *schedule_departure_date_copy = g_strdup(flight->schedule_departure_date);
                    char *year_str = strtok(schedule_departure_date_copy, "/");
                    // add the passengers to the list
                    add_to_ymetrics(&years_metrics, year_str, flight_seats->total_passengers, 3);

                    // free the string
                    g_free(schedule_departure_date_copy);
                    break;
                }

                // get the unique passengers
                // List to store the unique passengers
                /*GList *unique_passengers = NULL;
                char **passengers = flight_seats->passengers;

                GList *iterList = unique_passengers;
                int found = 0;

                // Iterate through all passenger IDs in the flight_seats
                for (int i = 0; i < flight_seats->total_passengers; i++) {
                    char *passenger_id = passengers[i];

                    // Reset found flag for each passenger ID
                    found = 0;

                    // Iterate through the existing unique_passengers list
                    while (iterList != NULL) {
                        char *existing_passenger_id = iterList->data;

                        // Compare the current passenger_id with the existing ones
                        if (strcmp(existing_passenger_id, passenger_id) == 0) {
                            found = 1;
                            break;
                        }

                        iterList = g_list_next(iterList);
                    }

                    // If not found, add the passenger_id to the unique_passengers list
                    if (found == 0) {
                        unique_passengers = g_list_append(unique_passengers, g_strdup(passenger_id));
                    }

                    // Reset iterList for the next iteration
                    iterList = unique_passengers;
                }

                // get the year from the schedule_departure_date
                char *schedule_departure_date_copy = g_strdup(flight->schedule_departure_date);
                char *year_str = strtok(schedule_departure_date_copy, "/");

                // add the unique passengers to the list
                add_to_ymetrics(&years_metrics, year_str, g_list_length(unique_passengers), 4);*/
            }
        }

        // Sort the list based on year
        years_metrics = g_list_sort(years_metrics, (GCompareFunc)compare_list_dates);

        // add the years_metrics to the resList
        GList *iterList = years_metrics;
        int number_of_results = 0;
        while (iterList != NULL) {
            char *year_metrics = iterList->data;
            char **year_metrics_tokens = g_strsplit(year_metrics, ";", 0);
            char *year = year_metrics_tokens[0];
            char *number_of_users = year_metrics_tokens[1];
            char *number_of_flights = year_metrics_tokens[2];
            char *number_of_passengers = year_metrics_tokens[3];
            char *number_of_unique_passengers = year_metrics_tokens[4];
            char *number_of_reservations = year_metrics_tokens[5];

            char* yearStr = NULL;

            if (format_flag) {
                char *str = "--- %d ---\nyear: %s\nusers: %s\nflights: %s\npassengers: %s\nunique_passengers: %s\nreservations: %s\n\n";
                int len = snprintf(NULL, 0, str, number_of_results + 1, year, number_of_users, number_of_flights, number_of_passengers, number_of_unique_passengers, number_of_reservations);
                yearStr = g_malloc(len + 1);
                snprintf(yearStr, len + 1, str, number_of_results + 1, year, number_of_users, number_of_flights, number_of_passengers, number_of_unique_passengers, number_of_reservations);
                // if it is the last line, we remove the \n
                if (iterList->next == NULL) {
                    yearStr[len - 1] = '\0';
                }
            } else {
                char *str = "%s;%s;%s;%s;%s;%s\n";
                int len = snprintf(NULL, 0, str, year, number_of_users, number_of_flights, number_of_passengers, number_of_unique_passengers, number_of_reservations);
                yearStr = g_malloc(len + 1);
                snprintf(yearStr, len + 1, str, year, number_of_users, number_of_flights, number_of_passengers, number_of_unique_passengers, number_of_reservations);
            }

            *resList = g_list_append(*resList, yearStr);

            g_strfreev(year_metrics_tokens);
            iterList = g_list_next(iterList);
            number_of_results++;
        }

        // free the list
        g_list_free_full(years_metrics, g_free);

    // TODO: implement the other cases
    /*} else if (args_size == 1) {
    } else if (args_size == 2) {*/
    } else {
        return;
    }
}