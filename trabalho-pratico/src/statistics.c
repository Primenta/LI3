/**
 * @file statistics.c
 * @brief Source file for statistical calculations to be used in the queries result calculations.
 */
#include "structs.h"
#include "statistics.h"
#include "catalog.h"
#include "utils.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Calculates the number of nights between two dates.
 *      The dates must be in the format YYYY/MM/DD.
 * @param begin_date The begin date.
 * @param end_date The end date.
 * @return int
 */
int calculate_nights(const char* begin_date, const char* end_date) {
    int number_of_nights = 0;

    int begin_year, begin_month, begin_day;
    sscanf(begin_date, "%4d/%2d/%2d", &begin_year, &begin_month, &begin_day);
    int end_year, end_month, end_day;
    sscanf(end_date, "%4d/%2d/%2d", &end_year, &end_month, &end_day);

    // Assuming each month has 30 days for simplicity
    int begin_days = begin_year * 365 + begin_month * 30 + begin_day;
    int end_days = end_year * 365 + end_month * 30 + end_day;

    // Calculate the difference in days
    number_of_nights = end_days - begin_days;

    return number_of_nights;
}

// Função auxiliar para extrair o ano da data de nascimento
/**
 * @brief Extracts the birth year from a date.
 * 
 * @param birth_date The birth date.
 * @return int 
 */
static int extract_birth_year(const char *birth_date) {
    int year;
    // usa-se sscanf porque birth_date etá definido como char
    sscanf(birth_date, "%4d", &year);
    return year;
}

/**
 * @brief Calculates the age of a person.
 * 
 * @param birth_date The birth date.
 * @return int 
 */
int calculate_age(const char *birth_date) {
    const int current_year = 2023; // se could use time.h to get the current year but since we are testing a fixed dataset, we can use a fixed value

    // Extrair o ano de nascimento do formato da data
    int birth_year = extract_birth_year(birth_date);

    // Calcular a idade
    int age = current_year - birth_year;

    return age;
}

// count_number_flights we go to passengers hash table and count the number of passengers with the same user_id
/*
remembering that the passengers hash table has the following structure:
    typedef struct {
    char* flight_id;
    char** passengers; // an array of user ids
    int total_passengers;
} FLIGHT_SEATS;

key: flight_id
value: FLIGHT_SEATS

and we want to count the number of passengers with the same user_id, so we need to iterate over the passengers array and count the number of times the user_id appears
*/
/**
 * @brief Counts the number of flights a user has (given a user_id).
 * 
 * @param passengers The passengers hash table.
 * @param user_id The user id.
 * @return int
*/
int count_number_flights(GHashTable *passengers, const char *user_id) {
    int count = 0;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, passengers);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        FLIGHT_SEATS *flight_seats = (FLIGHT_SEATS *) value;
        for (int i = 0; i < flight_seats->total_passengers; i++) {
            if (strcmp(flight_seats->passengers[i], user_id) == 0) {
                count++;
            }
        }
    }
    return count;
}

// get_user_flights we go to passengers hash table and get all the flights that have the same user_id
/**
 * @brief Get the user flights object (given a user_id).
 * 
 * @param passengers The passengers hash table.
 * @param user_id The user id.
 * @return char** 
 */
char** get_user_flights(GHashTable *passengers, const char *user_id) {
    char** flights = NULL;
    int flights_count = 0;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, passengers);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        FLIGHT_SEATS *flight_seats = (FLIGHT_SEATS *) value;

        for (int i = 0; i < flight_seats->total_passengers; i++) {
            if (strcmp(flight_seats->passengers[i], user_id) == 0) {
                flights = g_realloc(flights, sizeof(char*) * (flights_count + 1));
                flights[flights_count] = g_strdup(flight_seats->flight_id);
                flights_count++;
            }
        }
    }

    // Null-terminate the array
    flights = g_realloc(flights, sizeof(char*) * (flights_count + 1));
    flights[flights_count] = NULL;

    return flights;
}

// count_number_reservations we go to reservations hash table and count the number of reservations with the same user_id
/**
 * @brief Counts the number of reservations a user has (given a user_id).
 * 
 * @param reservations The reservations hash table.
 * @param user_id The user id.
 * @return int 
 */
int count_number_reservations(GHashTable *reservations, const char *user_id) {
    int count = 0;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, reservations);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        RESERVATION *reservation = (RESERVATION *) value;
        if (strcmp(reservation->user_id, user_id) == 0) {
            count++;
        }
    }
    return count;
}

// get_user_reservations we go to reservations hash table and get all the reservations that have the same user_id
/**
 * @brief Get the user reservations object (given a user_id).
 * 
 * @param reservations The reservations hash table.
 * @param user_id The user id.
 * @return char** 
 */
char** get_user_reservations(GHashTable *reservations, const char *user_id) {
    char** reservations_array = NULL;
    int reservations_count = 0;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, reservations);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        RESERVATION *reservation = (RESERVATION *) value;
        if (strcmp(reservation->user_id, user_id) == 0) {
            reservations_array = g_realloc(reservations_array, sizeof(char*) * (reservations_count + 1));
            reservations_array[reservations_count] = g_strdup(reservation->id);
            reservations_count++;
        }
    }

    // Null-terminate the array
    reservations_array = g_realloc(reservations_array, sizeof(char*) * (reservations_count + 1));
    reservations_array[reservations_count] = NULL;

    return reservations_array;
}

// calculate_total_spent we go to reservations hash table and sum the total price of the reservations with the same user_id
// reservation_price = price_per_night * number_of_nights + (price_per_night * number_of_nights) / 100 * city_tax
// total_spent (for a user) = sum(reservation_price)
/**
 * @brief Calculates the total amount of money a user has spent (given a user_id).
 * 
 * @param reservations The reservations hash table.
 * @param user_id The user id.
 * @return double 
 */
double calculate_total_spent(GHashTable *reservations, const char *user_id) {
    // first we go to a line that has the same user_id
    // next we check how many nights the user stayed in the hotel (end_date - begin_date)
    // next we check the price_per_night and multiply by the number of nights
    double total_spent = 0;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, reservations);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        RESERVATION *reservation = (RESERVATION *) value;
        if (strcmp(reservation->user_id, user_id) == 0) {
            // calculate the number of nights
            int number_of_nights = calculate_nights(reservation->begin_date, reservation->end_date);
            // calculate the total spent
            double price_per_night = atof(reservation->price_per_night);
            double city_tax = atof(reservation->city_tax);
            /*printf("number_of_nights: %d\n", number_of_nights);
            printf("price_per_night: %f\n", price_per_night);
            printf("city_tax: %s\n", reservation->city_tax);*/
            total_spent += price_per_night * number_of_nights + (price_per_night * number_of_nights) / 100 * city_tax;
        }
    }
    return total_spent;
}

// calculate_total_price returns just the total price of a reservation
/**
 * @brief Calculates the total price of a reservation.
 * 
 * @param price_per_night The price per night.
 * @param number_of_nights The number of nights.
 * @param city_tax The city tax.
 * @return double 
 */
double calculate_total_price(const char *price_per_night, int number_of_nights, int city_tax) {
    double total_price = 0;
    double price_per_night_double = atof(price_per_night);
    total_price = price_per_night_double * number_of_nights + (price_per_night_double * number_of_nights) / 100 * city_tax;
    return total_price;
}

// calculate the delay between the schedule_departure_date and the real_departure_date of a flight
// remember that the real_departure_date is in the format: YYYY/MM/DD HH:MM:SS
/**
 * @brief Calculates the delay between the schedule_departure_date and the real_departure_date of a flight.
 *    The dates must be in the format YYYY/MM/DD HH:MM:SS.
 * @param schedule_departure_date The schedule departure date.
 * @param real_departure_date The real departure date.
 * @return int 
 */
int calculate_delay(const char *schedule_departure_date, const char *real_departure_date) {
    int delay = 0;
    // first we need to extract the year, month, day, hour, minute and second from the real_departure_date
    int real_year, real_month, real_day, real_hour, real_minute, real_second;
    sscanf(real_departure_date, "%4d/%2d/%2d %2d:%2d:%2d", &real_year, &real_month, &real_day, &real_hour, &real_minute, &real_second);

    // next we need to extract the year, month, day, hour, minute and second from the schedule_departure_date
    int schedule_year, schedule_month, schedule_day, schedule_hour, schedule_minute, schedule_second;
    sscanf(schedule_departure_date, "%4d/%2d/%2d %2d:%2d:%2d", &schedule_year, &schedule_month, &schedule_day, &schedule_hour, &schedule_minute, &schedule_second);

    // if the real_hour is greater than the schedule_hour, we have a delay
    if (real_hour > schedule_hour) {
        delay = (real_hour - schedule_hour) * 60;
    }

    // if the real_minute is greater than the schedule_minute, we have a delay
    if (real_minute > schedule_minute) {
        delay += real_minute - schedule_minute;
    }

    // if the real_second is greater than the schedule_second, we have a delay
    if (real_second > schedule_second) {
        delay += real_second - schedule_second;
    }

    return (delay*60); // we return the delay in seconds
}

// date_comparator_wt (without time) is a comparator function used to qsort, it compares two dates and returns: -1 if date1 < date2, 0 if date1 == date2, 1 if date1 > date2
/**
 * @brief Compares two dates and returns: -1 if date1 < date2, 0 if date1 == date2, 1 if date1 > date2.
 * 
 * @param date1 The first date.
 * @param date2 The second date.
 * @return int 
 */
int date_comparator_wt(const char *date1, const char *date2) {
    // first we need to extract the year, month and day from the date1
    int year1, month1, day1;
    sscanf(date1, "%4d/%2d/%2d", &year1, &month1, &day1);

    // next we need to extract the year, month and day from the date2
    int year2, month2, day2;
    sscanf(date2, "%4d/%2d/%2d", &year2, &month2, &day2);

    // if year1 < year2, we return -1
    if (year1 < year2) {
        return -1;
    }

    // if year1 > year2, we return 1
    if (year1 > year2) {
        return 1;
    }

    // if year1 == year2, we need to check the month
    if (year1 == year2) {
        // if month1 < month2, we return -1
        if (month1 < month2) {
            return -1;
        }

        // if month1 > month2, we return 1
        if (month1 > month2) {
            return 1;
        }

        // if month1 == month2, we need to check the day
        if (month1 == month2) {
            // if day1 < day2, we return -1
            if (day1 < day2) {
                return -1;
            }

            // if day1 > day2, we return 1
            if (day1 > day2) {
                return 1;
            }

            // if day1 == day2, we return 0
            if (day1 == day2) {
                return 0;
            }
        }
    }
    return 0;
}

// date_comparator (with time) is a comparator function used to qsort, it compares two dates and returns: -1 if date1 < date2, 0 if date1 == date2, 1 if date1 > date2
/**
 * @brief Compares two dates and returns: -1 if date1 < date2, 0 if date1 == date2, 1 if date1 > date2.
 * 
 * @param date1 The first date.
 * @param date2 The second date.
 * @return int 
 */
int date_comparator(const char *date1, const char *date2) {
    // first we need to extract the year, month, day, hour, minute and second from the date1
    int year1, month1, day1, hour1, minute1, second1;
    sscanf(date1, "%4d/%2d/%2d %2d:%2d:%2d", &year1, &month1, &day1, &hour1, &minute1, &second1);

    // next we need to extract the year, month, day, hour, minute and second from the date2
    int year2, month2, day2, hour2, minute2, second2;
    sscanf(date2, "%4d/%2d/%2d %2d:%2d:%2d", &year2, &month2, &day2, &hour2, &minute2, &second2);

    // if year1 < year2, we return -1
    if (year1 < year2) {
        return -1;
    }

    // if year1 > year2, we return 1
    if (year1 > year2) {
        return 1;
    }

    // if year1 == year2, we need to check the month
    if (year1 == year2) {
        // if month1 < month2, we return -1
        if (month1 < month2) {
            return -1;
        }

        // if month1 > month2, we return 1
        if (month1 > month2) {
            return 1;
        }

        // if month1 == month2, we need to check the day
        if (month1 == month2) {
            // if day1 < day2, we return -1
            if (day1 < day2) {
                return -1;
            }

            // if day1 > day2, we return 1
            if (day1 > day2) {
                return 1;
            }

            // if day1 == day2, we need to check the hour
            if (day1 == day2) {
                // if hour1 < hour2, we return -1
                if (hour1 < hour2) {
                    return -1;
                }

                // if hour1 > hour2, we return 1
                if (hour1 > hour2) {
                    return 1;
                }

                // if hour1 == hour2, we need to check the minute
                if (hour1 == hour2) {
                    // if minute1 < minute2, we return -1
                    if (minute1 < minute2) {
                        return -1;
                    }

                    // if minute1 > minute2, we return 1
                    if (minute1 > minute2) {
                        return 1;
                    }

                    // if minute1 == minute2, we need to check the second
                    if (minute1 == minute2) {
                        // if second1 < second2, we return -1
                        if (second1 < second2) {
                            return -1;
                        }

                        // if second1 > second2, we return 1
                        if (second1 > second2) {
                            return 1;
                        }

                        // if second1 == second2, we return 0
                        if (second1 == second2) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}