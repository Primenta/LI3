/**
 * @file statistics.h
 * @brief Header file for the statistics module.
 */
#ifndef STATISTICS_H
#define STATISTICS_H

#include <glib.h>

int calculate_nights(const char* begin_date, const char* end_date);
int calculate_age(const char *birth_date);
int count_number_flights(GHashTable *passengers, const char *user_id);
char** get_user_flights(GHashTable *passengers, const char *user_id);
int count_number_reservations(GHashTable *reservations, const char *user_id);
char** get_user_reservations(GHashTable *reservations, const char *user_id);
double calculate_total_spent(GHashTable *reservations, const char *user_id);
double calculate_total_price(const char *price_per_night, int number_of_nights, int city_tax);
int calculate_delay(const char *schedule_departure_date, const char *real_departure_date);
int date_comparator_wt(const char *date1, const char *date2);
int date_comparator(const char *date1, const char *date2);

#endif