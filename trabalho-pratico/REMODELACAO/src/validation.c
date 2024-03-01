/**
 * @file validation.c
 * @brief Some validation functions/steps for the input data (dataset files) for the parser.
 */
#include "validation.h"
#include "structs.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

/* função que recebe uma data e verifica se é válida segundo os seguintes pontos:
O formato deverá ser sempre do tipo nnnn/nn/nn, onde n é um número entre 0 e 9
(inclusivo). Exemplo de possíveis erros: 2023/1234, 09/10, 20230901, 09/01/2023,
2023|09|01, abcd/09/01, . . . ;
O mês deverá estar entre 1 e 12 (inclusivo) e o dia entre 1 e 31 (inclusivo). Ignorar a validação dos dias consoante o mês (e.g., datas como 2023/02/31 não surgirão). Exemplos
de erros: 2023/01/52, 2023/14/03, . . .
*/
/**
 * @brief Validates a date.
 * 
 * @param date The date to validate.
 * @return int 1 if the date is valid, 0 otherwise.
 */
int validateDate(char *date) {
    // check if the date is valid
    // lets start by checking if they have hours associated, then execute "validateDateWithTime" else they are just dates "validateDateWithoutTime"
    if (strchr(date, ' ') != NULL) {
        return validateDateWithTime(date);
    } else {
        return validateDateWithoutTime(date);
    }
    return 0;
}

/**
 * @brief Validates a date without time.
 * 
 * @param date The date to validate.
 * @return int 1 if the date is valid, 0 otherwise.
 */
int validateDateWithoutTime(char *date) {
    char *date_copy = g_strdup(date); // Create a mutable copy of the input string
    if (date_copy == NULL) {
        return 0; // Handle memory allocation error
    }

    char *year = strtok(date_copy, "/");
    char *month = strtok(NULL, "/");
    char *day = strtok(NULL, "/");
    
    if (year == NULL || month == NULL || day == NULL) {
        g_free(date_copy);
        return 0; // Handle bad date format
    }

    if (strlen(year) != 4 || strlen(month) != 2 || strlen(day) != 2) {
        g_free(date_copy);
        return 0; // Handle the case where the tokens don't have the right length
    }

    int year_int = atoi(year);
    int month_int = atoi(month);
    int day_int = atoi(day);

    if (year_int == 0 || month_int < 1 || month_int > 12 || day_int < 1 || day_int > 31) {
        g_free(date_copy);
        return 0; // Handle out-of-range values
    }

    g_free(date_copy);

    return 1;
}

/*
O formato deverá ser sempre do tipo nnnn/nn/nn nn:nn:nn, onde n é um número entre
0 e 9 (inclusivo);
– As mesmas regras das datas aplicam-se aqui;
– A hora deverá estar entre 0 e 23 (inclusivo), os minutos entre 0 e 59 (inclusivo), e os
segundos entre 0 e 59 (inclusivo). Exemplos de erro: 2023/10/01 32:23:05, 2023/10/01
12:95:05, 2023/10/01 12:23:75, . . . ;
*/
/**
 * @brief Validates a date with time.
 * 
 * @param date The date to validate.
 * @return int 1 if the date is valid, 0 otherwise.
 */
int validateDateWithTime(char* date) {
    char *date_copy = g_strdup(date); // Create a mutable copy of the input string
    if (date_copy == NULL) {
        return 0; // Handle memory allocation error
    }

    char *year = strtok(date_copy, "/");
    char *month = strtok(NULL, "/");
    char *day = strtok(NULL, " ");
    char *hour = strtok(NULL, ":");
    char *minute = strtok(NULL, ":");
    char *second = strtok(NULL, ":");

    if (year == NULL || month == NULL || day == NULL || hour == NULL || minute == NULL || second == NULL) {
        g_free(date_copy);
        return 0; // Handle bad date format
    }

    if (strlen(year) != 4 || strlen(month) != 2 || strlen(day) != 2 || strlen(hour) != 2 || strlen(minute) != 2 || strlen(second) != 2) {
        g_free(date_copy);
        return 0; // Handle the case where the tokens don't have the right length
    }

    int year_int = atoi(year);
    int month_int = atoi(month);
    int day_int = atoi(day);
    int hour_int = atoi(hour);
    int minute_int = atoi(minute);
    int second_int = atoi(second);

    if (year_int == 0 || month_int < 1 || month_int > 12 || day_int < 1 || day_int > 31 || hour_int < 0 || hour_int > 23 || minute_int < 0 || minute_int > 59 || second_int < 0 || second_int > 59) {
        g_free(date_copy);
        return 0; // Handle out-of-range values
    }

    g_free(date_copy);

    return 1;
}

/*
As datas/datas com tempo de fim não poderão ser superiores às datas de início. Nos utilizadores, o birth_date tem que vir antes de account_creation; Nos voos, o schedule_departure_date
tem que vir antes de schedule_arrival_date, e o real_departure_date tem que vir antes de
real_arrival_date; E nas reservas, o begin_date tem que vir antes do end_date;
*/
/**
 * @brief Compares two dates.
 * 
 * @param d1 The first date.
 * @param d2 The second date.
 * @return int 1 if d1 happens first than d2, -1 if d2 happens first than d1, 0 if they are the same.
 */
int compareDates(char* d1, char* d2) {
    // returns success if d1 happens first than d2
    // returns error if d2 happens first than d1
    // lets first check if the date is valid (prevent segmentation fault)
    if (validateDate(d1) == 0 || validateDate(d2) == 0) {
        return -1;
    }
    // lets start by checking if they have hours associated, then execute "compareDatesWithTime" else they are just dates "compareDatesWithoutTime"
    if (strchr(d1, ' ') != NULL && strchr(d2, ' ') != NULL) {
        return compareDatesWithTime(d1, d2);
    } else {
        return compareDatesWithoutTime(d1, d2);
    }
    return 0;
}

/**
 * @brief Compares two dates without time.
 * 
 * @param d1 The first date.
 * @param d2 The second date.
 * @return int 1 if d1 happens first than d2, -1 if d2 happens first than d1, 0 if they are the same.
 */
int compareDatesWithoutTime(char* d1, char* d2) {
    // returns success if d1 happens first than d2
    // returns error if d2 happens first than d1
    char *d1_copy = g_strdup(d1); // Create a mutable copy of the input string
    char *d2_copy = g_strdup(d2); // Create a mutable copy of the input string
    if (d1_copy == NULL || d2_copy == NULL) {
        return -1; // Handle memory allocation error
    }

    char *d1_year = strtok(d1_copy, "/");
    char *d1_month = strtok(NULL, "/");
    char *d1_day = strtok(NULL, "/");
    
    char *d2_year = strtok(d2_copy, "/");
    char *d2_month = strtok(NULL, "/");
    char *d2_day = strtok(NULL, "/");
    
    if (d1_year == NULL || d1_month == NULL || d1_day == NULL || d2_year == NULL || d2_month == NULL || d2_day == NULL) {
        g_free(d1_copy);
        g_free(d2_copy);
        return -1; // Handle bad date format
    }

    int d1_year_int = atoi(d1_year);
    int d1_month_int = atoi(d1_month);
    int d1_day_int = atoi(d1_day);

    int d2_year_int = atoi(d2_year);
    int d2_month_int = atoi(d2_month);
    int d2_day_int = atoi(d2_day);

    g_free(d1_copy);
    g_free(d2_copy);

    if (d1_year_int < d2_year_int) {
        return 1;
    } else if (d1_year_int > d2_year_int) {
        return -1;
    } else {
        if (d1_month_int < d2_month_int) {
            return 1;
        } else if (d1_month_int > d2_month_int) {
            return -1;
        } else {
            if (d1_day_int < d2_day_int) {
                return 1;
            } else if (d1_day_int > d2_day_int) {
                return -1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

/**
 * @brief Compares two dates with time.
 * 
 * @param d1 The first date.
 * @param d2 The second date.
 * @return int 1 if d1 happens first than d2, -1 if d2 happens first than d1, 0 if they are the same.
 */
int compareDatesWithTime(char* d1, char* d2) {
    char *d1_copy = g_strdup(d1); // Create a mutable copy of the input string
    char *d2_copy = g_strdup(d2); // Create a mutable copy of the input string
    if (d1_copy == NULL || d2_copy == NULL) {
        return -1; // Handle memory allocation error
    }

    char *d1_year = strtok(d1_copy, "/");
    char *d1_month = strtok(NULL, "/");
    char *d1_day = strtok(NULL, " ");
    char *d1_hour = strtok(NULL, ":");
    char *d1_minute = strtok(NULL, ":");
    char *d1_second = strtok(NULL, ":");

    char *d2_year = strtok(d2_copy, "/");
    char *d2_month = strtok(NULL, "/");
    char *d2_day = strtok(NULL, " ");
    char *d2_hour = strtok(NULL, ":");
    char *d2_minute = strtok(NULL, ":");
    char *d2_second = strtok(NULL, ":");

    if (d1_year == NULL || d1_month == NULL || d1_day == NULL || d1_hour == NULL || d1_minute == NULL || d1_second == NULL || d2_year == NULL || d2_month == NULL || d2_day == NULL || d2_hour == NULL || d2_minute == NULL || d2_second == NULL) {
        g_free(d1_copy);
        g_free(d2_copy);
        return -1; // Handle bad date format
    }

    int d1_year_int = atoi(d1_year);
    int d1_month_int = atoi(d1_month);
    int d1_day_int = atoi(d1_day);
    int d1_hour_int = atoi(d1_hour);
    int d1_minute_int = atoi(d1_minute);
    int d1_second_int = atoi(d1_second);

    int d2_year_int = atoi(d2_year);
    int d2_month_int = atoi(d2_month);
    int d2_day_int = atoi(d2_day);
    int d2_hour_int = atoi(d2_hour);
    int d2_minute_int = atoi(d2_minute);
    int d2_second_int = atoi(d2_second);

    g_free(d1_copy);
    g_free(d2_copy);

    if (d1_year_int < d2_year_int) {
        return 1;
    } else if (d1_year_int > d2_year_int) {
        return -1;
    } else {
        if (d1_month_int < d2_month_int) {
            return 1;
        } else if (d1_month_int > d2_month_int) {
            return -1;
        } else {
            if (d1_day_int < d2_day_int) {
                return 1;
            } else if (d1_day_int > d2_day_int) {
                return -1;
            } else {
                if (d1_hour_int < d2_hour_int) {
                    return 1;
                } else if (d1_hour_int > d2_hour_int) {
                    return -1;
                } else {
                    if (d1_minute_int < d2_minute_int) {
                        return 1;
                    } else if (d1_minute_int > d2_minute_int) {
                        return -1;
                    } else {
                        if (d1_second_int < d2_second_int) {
                            return 1;
                        } else if (d1_second_int > d2_second_int) {
                            return -1;
                        } else {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/*
O email de um utilizador tem que ter o seguinte formato: “<username>@<domain>.<TLD>”.
O <username> e o <domain> têm que ter pelo menos tamanho 1; O <TLD> tem que ter pelo
menos tamanho 2. Exemplo de erros: @email.com, john@.pt, john@email.a, john@email,pt,
john.email.pt, . . . ;*/
/**
 * @brief Validates an email.
 * 
 * @param email The email to validate.
 * @return int 1 if the email is valid, 0 otherwise.
 */
int validateEmail(char *email) {
    if (strchr(email, '@') == NULL || strchr(email, '.') == NULL) {
        return 0;
    } else {
        char *email_copy = g_strdup(email); // Create a mutable copy of the input string
        if (email_copy == NULL) {
            return 0; // Handle memory allocation error
        }

        char *username = strtok(email_copy, "@");
        char *domain = strtok(NULL, ".");
        char *tld = strtok(NULL, ".");

        if (username == NULL || domain == NULL || tld == NULL) {
            g_free(email_copy);
            return 0; // Handle bad email format
        }

        if (strlen(username) < 1 || strlen(domain) < 1 || strlen(tld) < 2) {
            g_free(email_copy);
            return 0; // Handle the case where the tokens don't have the right length
        }

        g_free(email_copy);

        return 1;
    }
    return 0;
}

/*
O country_code de um utilizador deverá ser formado por duas letras;*/
/**
 * @brief Validates a country code.
 * 
 * @param country_code The country code to validate.
 * @return int 1 if the country code is valid, 0 otherwise.
 */
int validateCCode(char* country_code) {
    if (strlen(country_code) != 2) {
        return 0;
    } else {
        return 1;
    }
}

/*
O account_status de um utilizador deverá ter o valor “active” ou “inactive”, sendo que diferentes combinações de maiúsculas e minúsculas também são válidas (e.g., “Active”, “aCtive”,
e “INACTIVE” também são válidos);*/
/**
 * @brief Validates an account status.
 * 
 * @param account_status The account status to validate.
 * @return int 1 if the account status is valid, 0 otherwise.
 */
int isActive(char* account_status) {
    // first lets convert the string to lowercase
    char *account_status_copy = g_strdup(account_status); // Create a mutable copy of the input string
    if (account_status_copy == NULL) {
        return 0; // Handle memory allocation error
    }

    for (int i = 0; account_status_copy[i]; i++) {
        account_status_copy[i] = tolower(account_status_copy[i]);
    }

    if (strcmp(account_status_copy, "active") == 0) {
        g_free(account_status_copy);
        return 1;
    } else {
        g_free(account_status_copy);
        return 0;
    }
    return 0;
}

/**
 * @brief Validates an account status.
 * 
 * @param account_status The account status to validate.
 * @return int 1 if the account status is valid, 0 otherwise.
 */
int validateStatus(char* account_status) {
    // first lets convert the string to lowercase
    char *account_status_copy = g_strdup(account_status); // Create a mutable copy of the input string
    if (account_status_copy == NULL) {
        return 0; // Handle memory allocation error
    }

    for (int i = 0; account_status_copy[i]; i++) {
        account_status_copy[i] = tolower(account_status_copy[i]);
    }

    if (strcmp(account_status_copy, "active") == 0 || strcmp(account_status_copy, "inactive") == 0) {
        g_free(account_status_copy);
        return 1;
    } else {
        g_free(account_status_copy);
        return 0;
    }
    return 0;
}

/*
O número de lugares de um voo (total_seats) não poderá ser inferior ao número de passageiros
nesse voo;*/
/**
 * @brief Validates the number of seats.
 * 
 * @param total_seats The total number of seats.
 * @param passengers The number of passengers.
 * @return int 1 if the number of seats is valid, 0 otherwise.
 */
int validateSeats(int total_seats, int passengers) {
    if (total_seats < passengers) {
        return 0;
    } else {
        return 1;
    }
}

/*
Os aeroportos de origem e destino de um voo tem que ser constituídos por 3 letras. Considerase que dois aeroportos são iguais quando são formados pela mesma sequência de letras,
mesmo que existam diferenças entre maiúsculas e minúsculas (e.g., OPO, opo, e opO são
considerados o mesmo aeroporto);*/
/**
 * @brief Validates an airport.
 * 
 * @param airport The airport to validate.
 * @return int 1 if the airport is valid, 0 otherwise.
 */
int validateAirport(char* airport) {
    if (strlen(airport) != 3) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Validates a trip.
 * 
 * @param airport_depart The departure airport.
 * @param airport_arr The arrival airport.
 * @return int 1 if the trip is valid, 0 otherwise.
 */
int validateTrip(char* airport_depart, char* airport_arr) {
    // check if the airport names are valid
    if (validateAirport(airport_depart) == 0 || validateAirport(airport_arr) == 0) {
        return 0;
    }
    // first we put both airports in lowercase
    char *airport_depart_copy = g_strdup(airport_depart); // Create a mutable copy of the input string
    char *airport_arr_copy = g_strdup(airport_arr); // Create a mutable copy of the input string
    if (airport_depart_copy == NULL || airport_arr_copy == NULL) {
        return 0; // Handle memory allocation error
    }

    for (int i = 0; airport_depart_copy[i]; i++) {
        airport_depart_copy[i] = tolower(airport_depart_copy[i]);
    }

    for (int i = 0; airport_arr_copy[i]; i++) {
        airport_arr_copy[i] = tolower(airport_arr_copy[i]);
    }

    if (strcmp(airport_depart_copy, airport_arr_copy) == 0) {
        g_free(airport_depart_copy);
        g_free(airport_arr_copy);
        return 0;
    } else {
        g_free(airport_depart_copy);
        g_free(airport_arr_copy);
        return 1;
    }
}

/*
O número de estrelas de um hotel (hotel_stars) tem que ser um valor inteiro entre 1 e 5,
inclusive. Exemplos de erros: 0, -3, 1.4, . . . ;*/
/**
 * @brief Validates the number of stars.
 * 
 * @param hotel_stars The number of stars.
 * @return int 1 if the number of stars is valid, 0 otherwise.
 */
int validateStars(const char *hotel_stars) {
    if (hotel_stars == NULL || strlen(hotel_stars) == 0) {
        return 0;
    }
    // string to double
    double hotel_stars_double = atof(hotel_stars);
    
    // Check if the conversion is successful
    if (hotel_stars_double == 0 && hotel_stars[0] != '0') {
        return 0; // Invalid conversion
    }

    // Check if it is an integer
    if (hotel_stars_double - (int)hotel_stars_double != 0) {
        return 0; // Not an integer
    }

    // Check if it is within the valid range
    if (hotel_stars_double < 1 || hotel_stars_double > 5) {
        return 0; // Out of range
    }

    return 1; // Valid hotel stars
}

/*
A percentagem de imposto da cidade de uma reserva (city_tax) tem que ser um valor inteiro
maior ou igual a zero; Exemplos de erros: -3, 1.4, . . . ;*/
/**
 * @brief Validates the city tax.
 * 
 * @param city_tax The city tax.
 * @return int 1 if the city tax is valid, 0 otherwise.
 */
int validateTax(const char *city_tax) {
    if (city_tax == NULL || strlen(city_tax) == 0) {
        return 0;
    }
    // string to double
    double city_tax_double = atof(city_tax);
    
    // Check if the conversion is successful
    if (city_tax_double == 0 && city_tax[0] != '0') {
        return 0; // Invalid conversion
    }

    // Check if it is an integer
    if (city_tax_double - (int)city_tax_double != 0) {
        return 0; // Not an integer
    }

    // Check if it is greater than or equal to zero
    if (city_tax_double < 0) {
        return 0; // Not greater than or equal to zero
    }

    return 1; // Valid city tax
}

/*
O preço por noite de uma reserva (price_per_night) tem que ser um valor inteiro maior que
0. Exemplo de erros: 0, -3, 1.4, . . . ;*/
/**
 * @brief Validates the price per night.
 * 
 * @param price_per_night The price per night.
 * @return int 1 if the price per night is valid, 0 otherwise.
 */
int validatePrice(const char *price_per_night) {
    if (price_per_night == NULL || strlen(price_per_night) == 0) {
        return 0;
    }
    // string to double
    double price_per_night_double = atof(price_per_night);
    
    // Check if the conversion is successful
    if (price_per_night_double == 0 && price_per_night[0] != '0') {
        return 0; // Invalid conversion
    }

    // Check if it is an integer
    if (price_per_night_double - (int)price_per_night_double != 0) {
        return 0; // Not an integer
    }

    // Check if it is greater than zero
    if (price_per_night_double <= 0) {
        return 0; // Not greater than zero
    }

    return 1; // Valid price
}

/*
O campo includes_breafast de uma reserva pode ser especificado das seguintes formas (com
diferentes combinações de maiúsculas e minúsculas): Para valores falsos, “f ”, “false”, “0”, e
“” (string vazia); Para valores verdadeiros, “t”, “true”, e “1”.*/
/**
 * @brief Validates the breakfast.
 * 
 * @param includes_breakfast The breakfast.
 * @return int 1 if the breakfast is valid, 0 otherwise.
 */
int validateBreakfast(char* includes_breakfast) {
    // first lets convert the string to lowercase
    char *includes_breakfast_copy = g_strdup(includes_breakfast); // Create a mutable copy of the input string
    if (includes_breakfast_copy == NULL) {
        return 0; // Handle memory allocation error
    }

    for (int i = 0; includes_breakfast_copy[i]; i++) {
        includes_breakfast_copy[i] = tolower(includes_breakfast_copy[i]);
    }

    if (strcmp(includes_breakfast_copy, "f") == 0 || strcmp(includes_breakfast_copy, "false") == 0 || strcmp(includes_breakfast_copy, "0") == 0 || strcmp(includes_breakfast_copy, "") == 0) {
        g_free(includes_breakfast_copy);
        return 1;
    } else if (strcmp(includes_breakfast_copy, "t") == 0 || strcmp(includes_breakfast_copy, "true") == 0 || strcmp(includes_breakfast_copy, "1") == 0) {
        g_free(includes_breakfast_copy);
        return 1;
    } else {
        g_free(includes_breakfast_copy);
        return 0;
    }
    return 0;
}

/*
As classificações de uma reserva (rating) têm que ter um valor inteiro entre 1 e 5, inclusive.
Exemplos de erros: 0, -3, 1.4, . . . . Opcionalmente, podem estar vazias caso o utilizador não
tenha classificado o hotel;*/
/**
 * @brief Validates the rating.
 * 
 * @param rating The rating.
 * @return int 1 if the rating is valid, 0 otherwise.
 */
int validateRating(const char* rating) {
    if (rating == NULL || strlen(rating) == 0) {
        return 0;
    }
    // string to double
    double rating_double = atoi(rating);

    // Check if the conversion is successful
    if (rating_double == 0 && rating[0] != '0') {
        return 0; // Invalid conversion
    }

    // Check if it is an integer
    if (rating_double - (int)rating_double != 0) {
        return 0; // Not an integer
    }

    // if is out of range return error
    if (rating_double < 1 || rating_double > 5) {
        return 0; // Out of range
    }

    return 1; // Valid rating
}

/*
Os seguintes restantes campos têm que ter tamanho superior a zero:
– Utilizador: id, name, phone_number, sex, passport, address, pay_method;
– Voo: id, airline, plane_model, pilot, copilot;
– Reserva: id, user_id, hotel_id, hotel_name, address.*/
/**
 * @brief Validates the field size.
 * 
 * @param field The field to validate.
 * @return int 1 if the field size is valid, 0 otherwise.
 */
int validateFieldSize(const char* field) {
    if (strlen(field) == 0) {
        return 0;
    } else {
        return 1;
    }
}

// isInt
/**
 * @brief Validates if a string is an integer.
 * 
 * @param str The string to validate.
 * @return int 1 if the string is an integer, 0 otherwise.
 */
int isInt(const char* str) {
    if (str == NULL || strlen(str) == 0) {
        return 0;
    }
    // string to double
    double str_double = atoi(str);

    // Check if the conversion is successful
    if (str_double == 0 && str[0] != '0') {
        return 0; // Invalid conversion
    }

    // Check if it is an integer
    if (str_double - (int)str_double != 0) {
        return 0; // Not an integer
    }

    return 1; // Valid rating
}