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
        USER *user = g_hash_table_lookup(c->users, args[0]);
        // se não encontrarmos na hash table de users, vamos procurar na hash table de flights
        if (user == NULL) {
            FLIGHT *flight = g_hash_table_lookup(c->flights, args[0]);
            // se não encontrarmos na hash table de flights, vamos procurar na hash table de reservations
            if (flight == NULL) {
                RESERVATION *reservation = g_hash_table_lookup(c->reservations, args[0]);
                // se não encontrarmos na hash table de reservations, retornamos uma string vazia
                if (reservation == NULL) {
                    return result;
                } else {
                    // retornamos a informação da reserva "reservation";
                    // "hotel_id;hotel_name;hotel_stars;begin_date;end_date;includes_breakfast;nights;total_price"
                    int nights = calculate_nights(reservation->begin_date, reservation->end_date);
                    double total_price = calculate_total_price(reservation->price_per_night, nights, atoi(reservation->city_tax));
                    // include_breakfast first letter to uppercase
                    reservation->includes_breakfast[0] = toupper(reservation->includes_breakfast[0]);
                    if (format_flag) {
                        sprintf(result, "--- %d ---\nhotel_id: %s\nhotel_name: %s\nhotel_stars: %s\nbegin_date: %s\nend_date: %s\nincludes_breakfast: %s\nnights: %d\ntotal_price: %.3f\n", number_of_register, reservation->hotel_id, reservation->hotel_name, reservation->hotel_stars, reservation->begin_date, reservation->end_date, reservation->includes_breakfast, nights, total_price);
                    } else { // if theres no format flag, we return the string as it is
                        sprintf(result, "%s;%s;%s;%s;%s;%s;%d;%.3f\n", reservation->hotel_id, reservation->hotel_name, reservation->hotel_stars, reservation->begin_date, reservation->end_date, reservation->includes_breakfast, nights, total_price);
                    }
                    number_of_register++;
                }
            } else {
                // retornamos a informação do voo "flight";
                // "airline;plane_model;origin;destination;schedule_departure_date;schedule_arrival_date;passengers;delay"
                int flight_passengers = get_flight_passengers(c->passengers, flight->id);
                int delay = calculate_delay(flight->schedule_departure_date, flight->real_departure_date);
                if (format_flag) {
                    sprintf(result, "--- %d ---\nairline: %s\nplane_model: %s\norigin: %s\ndestination: %s\nschedule_departure_date: %s\nschedule_arrival_date: %s\npassengers: %d\ndelay: %d\n", number_of_register, flight->airline, flight->plane_model, flight->origin, flight->destination, flight->schedule_departure_date, flight->schedule_arrival_date, flight_passengers, delay);
                } else { // if theres no format flag, we return the string as it is
                    sprintf(result, "%s;%s;%s;%s;%s;%s;%d;%d\n", flight->airline, flight->plane_model, flight->origin, flight->destination, flight->schedule_departure_date, flight->schedule_arrival_date, flight_passengers, delay);
                }
                number_of_register++;
            }
        } else {
            // "Não deverão ser retornadas informações para utilizadores com account_status = “inactive”"
            if (isActive(user->account_status) == 0) return result;
            // retornamos a informação do user "user";
            // "user_id;sex;age;country_code;passport;number_of_flights;number_of_reservations;total_spent"
            int age = calculate_age(user->birth_date);
            // for number_of_flights we go to passengers hash table and count the number of passengers with the same user_id
            int number_of_flights = count_number_flights(c->passengers, user->id);
            // for number_of_reservations we go to reservations hash table and count the number of reservations with the same user_id
            int number_of_reservations = count_number_reservations(c->reservations, user->id);
            // for total_spent we go to reservations hash table and sum the total price of the reservations with the same user_id
            double total_spent = calculate_total_spent(c->reservations, user->id);
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
                sprintf(result, "--- %d ---\nname: %s\nsex: %s\nage: %d\ncountry_code: %s\npassport: %s\nnumber_of_flights: %d\nnumber_of_reservations: %d\ntotal_spent: %.3f\n", number_of_register, user->name, user->sex, age, user->country_code, user->passport, number_of_flights, number_of_reservations, total_spent);
            } else { // if theres no format flag, we return the string as it is
                sprintf(result, "%s;%s;%d;%s;%s;%d;%d;%.3f\n", user->name, user->sex, age, user->country_code, user->passport, number_of_flights, number_of_reservations, total_spent);
            }
            number_of_register++;
        }
    }
    return result;
}