/**
 * @file structs.h
 * @brief Header file for the data types used in the dataset manager system.
 */
#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_LINE_SIZE 1024
#define MAX_PATH_LENGTH 256

/* Data Types */
typedef struct UserStruct USER;
typedef struct FlightStruct FLIGHT;
typedef struct FlightSeatsStruct FLIGHT_SEATS;
typedef struct ReservationStruct RESERVATION;

/* User */
USER* create_user();
void destroy_user(USER* user);
void set_user_id(USER* user, const char* id);
const char* get_user_id(const USER* user);
void update_user_info(USER* user, const int information_type, const char* information);
const char* get_user_info(const USER* user, const int information_type);

/* Flight */
FLIGHT* create_flight();
void destroy_flight(FLIGHT* flight);
void set_flight_id(FLIGHT* flight, const char* id);
const char* get_flight_id(const FLIGHT* flight);
void update_flight_info(FLIGHT* flight, const int information_type, const char* information);
const char* get_flight_info(const FLIGHT* flight, const int information_type);

/* Flight Seats */
FLIGHT_SEATS* create_flight_seats();
void destroy_flight_seats(FLIGHT_SEATS* flight_seats);
void set_flight_seats_id(FLIGHT_SEATS* flight_seats, const char* id);
const char* get_flight_seats_id(const FLIGHT_SEATS* flight_seats);
void update_flight_seats_passengers(FLIGHT_SEATS* seats, const char** passenger_list) ;
const char** get_flight_seats_passengers(const FLIGHT_SEATS* flight_seats);
void update_flight_seats_total_passengers(FLIGHT_SEATS* flight_seats, const int total_passengers);
int get_flight_seats_total_passengers(const FLIGHT_SEATS* flight_seats);
void add_passenger_to_flight_seats(FLIGHT_SEATS* flight_seats, const char* passenger);

/* Reservation */
RESERVATION* create_reservation();
void destroy_reservation(RESERVATION* reservation);
void set_reservation_id(RESERVATION* reservation, const char* id);
const char* get_reservation_id(const RESERVATION* reservation);
void update_reservation_info(RESERVATION* reservation, const int information_type, const char* information);
const char* get_reservation_info(const RESERVATION* reservation, const int information_type);

#endif