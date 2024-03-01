/**
 * @file validation.h
 * @brief Header file for the validation module.
*/
#ifndef VALIDATION_H
#define VALIDATION_H

int validateDate(char *date);
int validateDateWithoutTime(char *date);
int validateDateWithTime(char* date);
int compareDates(char* d1, char* d2);
int compareDatesWithoutTime(char* d1, char* d2);
int compareDatesWithTime(char* d1, char* d2);
int validateEmail(char *email);
int validateCCode(char* country_code);
int isActive(char* account_status);
int validateStatus(char* account_status);
int validateSeats(int total_seats, int passengers);
int validateAirport(char* airport);
int validateTrip(char* airport_depart, char* airport_arr);
int validateStars(const char* hotel_stars);
int validateTax(const char* city_tax);
int validatePrice(const char* price_per_night);
int validateBreakfast(char* includes_breakfast);
int validateRating(const char* rating);
int validateFieldSize(const char* field);
int isInt(const char* str);

#endif