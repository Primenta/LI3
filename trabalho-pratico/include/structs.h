/**
 * @file structs.h
 * @brief Header file for the data types used in the dataset manager system.
 */
#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_LINE_SIZE 1024
#define MAX_PATH_LENGTH 256

/* Data Types */
typedef struct User {
    char* id;
    char* name;
    char* email;
    char* phone_number;
    char* birth_date;
    char* sex;
    char* passport;
    char* country_code;
    char* address;
    char* account_creation;
    char* pay_method;
    char* account_status;
} USER;

typedef struct Flight {
    char *id;
    char* airline;
    char* plane_model;
    char *total_seats;
    char* origin;
    char* destination;
    char* schedule_departure_date;
    char* schedule_arrival_date;
    char* real_departure_date;
    char* real_arrival_date;
    char* pilot;
    char* copilot;
    //char* notes;
} FLIGHT;

/*typedef struct Passenger {
    char* flight_id;
    char* user_id;
} PASSENGER;*/

// lets replace it by FLIGHT_SEATS
typedef struct flight_seats {
    char* flight_id;
    char** passengers; // an array of user ids
    int total_passengers;
} FLIGHT_SEATS;

typedef struct Reservation {
    char* id; /* or make int but we need to remove the "Book00001" and get only the int*/
    char* user_id;
    char* hotel_id;
    char* hotel_name;
    char* hotel_stars;
    char* city_tax;
    char* address;
    char* begin_date;
    char* end_date;
    char* price_per_night;
    char* includes_breakfast;
    //char* room_details;
    char* rating;
    //char* comment;
} RESERVATION;

#endif