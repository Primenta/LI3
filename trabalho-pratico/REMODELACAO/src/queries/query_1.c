/**
 * @file query_1.c
 * @brief Implementation of query 1.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"

#include <ctype.h>

/** REMODELACAO **/

// query_1 retorna as informações do utilizador com o username "username" ou as informações do voo com o id "id" ou as informações da reserva com o id "id"
// JéssiTavares910 > retorna da tabela de hash o valor associado à chave "JéssiTavares910" as informações do utilizador com o username "JéssiTavares910" (ex: "Jéssica Tavares;F;63;PT;ZE466021;11;5;2365.200")
// F JéssiTavares910 (isto é, tem format_flags, format_flags = 1) > retorna:
/*ex:
--- 1 ---
name: Jéssica Tavares
sex: F
age: 63
country_code: PT
passport: ZE466021
number_of_flights: 11
number_of_reservations: 5
total_spent: 2365.200
*/
// onde "--- 1 ---" é o numero de resultados encontrados (neste caso, 1)
// 0000000029 > retorna as informacoes do voo com o id "0000000029"
// Book0000000048 > retorna as informacoes da reserva com o id "Book0000000048"
/**
 * @brief Returns the information of the user with the username "username" or the information of the flight with the id "id" or the information of the reservation with the id "id".
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @return char* The result string.
 */
char* query_1(CATALOG *c, int format_flag, char **args, int args_size) {
    char *result = g_malloc(sizeof(char) * MAX_RES_SIZE);
    if (result == NULL) {
        // Handle memory allocation failure
        return NULL;
    }
    // Number of register
    int number_of_register = 1;
    // Ensure that the result buffer is initialized before using it
    result[0] = '\0'; // Set the first character to null terminator
    if (args_size == 1) { // só temos o user_id, ou id do voo, ou id da reserva
        // vamos procurar na hash table de users
        USER *user = g_hash_table_lookup(getUsers(c), args[0]);
        // se não encontrarmos na hash table de users, vamos procurar na hash table de flights
        if (user == NULL) {
            FLIGHT *flight = g_hash_table_lookup(getFlights(c), args[0]);
            // se não encontrarmos na hash table de flights, vamos procurar na hash table de reservations
            if (flight == NULL) {
                RESERVATION *reservation = g_hash_table_lookup(getReservations(c), args[0]);
                // se não encontrarmos na hash table de reservations, retornamos uma string vazia
                if (reservation == NULL) {
                    return result;
                } else {
                    // retornamos a informação da reserva "reservation";
                    // "hotel_id;hotel_name;hotel_stars;begin_date;end_date;includes_breakfast;nights;total_price"
                    int nights = calculate_nights(get_reservation_info(reservation, 7), get_reservation_info(reservation, 8));
                    double total_price = calculate_total_price(get_reservation_info(reservation, 9), nights, atoi(get_reservation_info(reservation, 5)));
                    // include_breakfast first letter to uppercase
                    char* reservation_info = get_reservation_info(reservation, 10);
                    if (reservation_info != NULL) {
                        reservation_info[0] = toupper(reservation_info[0]);
                        update_reservation_info(reservation, 10, reservation_info);
                    }
                    if (format_flag) {
                        sprintf(result, "--- %d ---\nhotel_id: %s\nhotel_name: %s\nhotel_stars: %s\nbegin_date: %s\nend_date: %s\nincludes_breakfast: %s\nnights: %d\ntotal_price: %.3f\n", number_of_register,
                        get_reservation_info(reservation, 2), get_reservation_info(reservation, 3),
                        get_reservation_info(reservation, 4), get_reservation_info(reservation, 7), 
                        get_reservation_info(reservation, 8), get_reservation_info(reservation, 10), nights, total_price);
                    } else { // if theres no format flag, we return the string as it is
                        sprintf(result, "%s;%s;%s;%s;%s;%s;%d;%.3f\n", get_reservation_info(reservation, 2), get_reservation_info(reservation, 3),
                        get_reservation_info(reservation, 4), get_reservation_info(reservation, 7), 
                        get_reservation_info(reservation, 8), get_reservation_info(reservation, 10), nights, total_price);
                    }
                    number_of_register++;
                }
            } else {
                // retornamos a informação do voo "flight";
                // "airline;plane_model;origin;destination;schedule_departure_date;schedule_arrival_date;passengers;delay"
                int flight_passengers = get_flight_passengers(getPassengers(c), get_flight_id(flight));
                int delay = calculate_delay(get_flight_info(flight, 6), get_flight_info(flight, 8));
                if (format_flag) {
                    sprintf(result, "--- %d ---\nairline: %s\nplane_model: %s\norigin: %s\ndestination: %s\nschedule_departure_date: %s\nschedule_arrival_date: %s\npassengers: %d\ndelay: %d\n", number_of_register, get_flight_info(flight, 1),
                    get_flight_info(flight, 2), get_flight_info(flight, 4),
                    get_flight_info(flight, 5), get_flight_info(flight, 6),
                    get_flight_info(flight, 7), flight_passengers, delay);
                } else { // if theres no format flag, we return the string as it is
                    sprintf(result, "%s;%s;%s;%s;%s;%s;%d;%d\n", get_flight_info(flight, 1),
                    get_flight_info(flight, 2), get_flight_info(flight, 4),
                    get_flight_info(flight, 5), get_flight_info(flight, 6),
                    get_flight_info(flight, 7), flight_passengers, delay);
                }
                number_of_register++;
            }
        } else {
            // "Não deverão ser retornadas informações para utilizadores com account_status = “inactive”"
            if (isActive(get_user_info(user, 11)) == 0) return result;
            // retornamos a informação do user "user";
            // "user_id;sex;age;country_code;passport;number_of_flights;number_of_reservations;total_spent"
            int age = calculate_age(get_user_info(user, 4));
            // for number_of_flights we go to passengers hash table and count the number of passengers with the same user_id
            int number_of_flights = count_number_flights(getPassengers(c), get_user_id(user));
            // for number_of_reservations we go to reservations hash table and count the number of reservations with the same user_id
            int number_of_reservations = count_number_reservations(getReservations(c), get_user_id(user));
            // for total_spent we go to reservations hash table and sum the total price of the reservations with the same user_id
            double total_spent = calculate_total_spent(getReservations(c), get_user_id(user));
            /*printf("UserID: %s\n", user->id);
            printf("Age: %d\n", age);
            printf("Number of flights: %d\n", number_of_flights);
            printf("Number of reservations: %d\n", number_of_reservations);
            printf("Total spent: %f\n", total_spent);*/
            // lets format the string
            if (format_flag) {
                // if format flag is on we format the string like:
                /*--- 1 --- (1 if number_of_register)
                name: (value)
                sex: (value)
                age: (value)
                */
                sprintf(result, "--- %d ---\nname: %s\nsex: %s\nage: %d\ncountry_code: %s\npassport: %s\nnumber_of_flights: %d\nnumber_of_reservations: %d\ntotal_spent: %.3f\n", number_of_register, get_user_info(user, 1),
                get_user_info(user, 5), age, get_user_info(user, 7), get_user_info(user, 6),
                number_of_flights, number_of_reservations, total_spent);
            } else { // if theres no format flag, we return the string as it is
                sprintf(result, "%s;%s;%d;%s;%s;%d;%d;%.3f\n", get_user_info(user, 1),
                get_user_info(user, 5), age, get_user_info(user, 7), get_user_info(user, 6),
                number_of_flights, number_of_reservations, total_spent);
            }
            number_of_register++;
        }
    }
    return result;
}