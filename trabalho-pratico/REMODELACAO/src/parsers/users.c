/**
 * @file users.c
 * @brief Implementation of the users parser.
*/
#include "structs.h"
#include "parsers/users.h"
#include "validation.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATASET_NAME "users.csv"
#define ERRORS_DATASET_NAME "users_errors.csv"

/** REMODELACAO **/
// Structs
struct UserStruct {
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
};

// Getters/Setters
USER* create_user() {
    return malloc(sizeof(USER));
    // we can also if optionaly set the fields to NULL or empty and then return the user (but we can't test it since we are not using it in the main program)
}

void destroy_user(USER* user) {
    free(user);
}

// como o id é algo único este tem uma importância maior (dado ser possivel a sua utilização para obter informações rapidamente), por isso temos um setter e um getter únicos deste
void set_user_id(USER* user, const char* id) {
    if (user && id) {
        if (user->id) {
            free(user->id);
        }
        user->id = strdup(id);
    }
}

const char* get_user_id(const USER* user) {
    return user ? user->id : NULL;
}
// Since we have a lot of fields, we will only implement the id getter and setter and give the program the ability to update and get the other fields by index (opção mais eficiente e rápida para escrita de código escolhida pelo grupo)
// we could also do it one by one if needed or if we wanted to do more specific validations or tasks for each field
// IMPORTANT: since we are valitating the fields in the parser, we don't need to validate them here, becasue is will only add them to the hash table if they are valid
void update_user_info(USER* user, const int information_type, const char* information) {
    if (user) {
        switch (information_type) {
            case 1:
                if (user->name) {
                    free(user->name);
                }
                user->name = strdup(information);
                break;
            case 2:
                if (user->email) {
                    free(user->email);
                }
                user->email = strdup(information);
                break;
            case 3:
                if (user->phone_number) {
                    free(user->phone_number);
                }
                user->phone_number = strdup(information);
                break;
            case 4:
                if (user->birth_date) {
                    free(user->birth_date);
                }
                user->birth_date = strdup(information);
                break;
            case 5:
                if (user->sex) {
                    free(user->sex);
                }
                user->sex = strdup(information);
                break;
            case 6:
                if (user->passport) {
                    free(user->passport);
                }
                user->passport = strdup(information);
                break;
            case 7:
                if (user->country_code) {
                    free(user->country_code);
                }
                user->country_code = strdup(information);
                break;
            case 8:
                if (user->address) {
                    free(user->address);
                }
                user->address = strdup(information);
                break;
            case 9:
                if (user->account_creation) {
                    free(user->account_creation);
                }
                user->account_creation = strdup(information);
                break;
            case 10:
                if (user->pay_method) {
                    free(user->pay_method);
                }
                user->pay_method = strdup(information);
                break;
            case 11:
                if (user->account_status) {
                    free(user->account_status);
                }
                user->account_status = strdup(information);
                break;
            default:
                break;
        }
    }
}

const char* get_user_info(const USER* user, const int information_type) {
    if (user) {
        switch (information_type) {
            case 1:
                return user->name;
            case 2:
                return user->email;
            case 3:
                return user->phone_number;
            case 4:
                return user->birth_date;
            case 5:
                return user->sex;
            case 6:
                return user->passport;
            case 7:
                return user->country_code;
            case 8:
                return user->address;
            case 9:
                return user->account_creation;
            case 10:
                return user->pay_method;
            case 11:
                return user->account_status;
            default:
                return NULL;
        }
    }
    return NULL;
}

// Function to free memory allocated for a User struct
/**
 * @brief Frees the memory allocated for a user.
 * 
 * @param data The data to be freed.
*/
void free_user(gpointer data) {
    USER *user = (USER *)data;
    if (user != NULL) {
        destroy_user(user);
    }
}

// Function to print User struct stored in the hash table
/**
 * @brief Prints a user.
 * 
 * @param key The key of the hash table.
 * @param value The value of the hash table (not used).
 * @param data The data of the hash table (not used).
*/
void print_hash_user(gpointer key, gpointer value, gpointer data) {
    (void)key;
    (void)data;
    USER *user = (USER *)value;
    //const char *user_id = (const char *)key; // since we are using the user_id as key bu we can also use what we wrote
    printf("User: %s\n", get_user_id(user));
    printf("\tName: %s\n", get_user_info(user, 1));
    printf("\tEmail: %s\n", get_user_info(user, 2));
    printf("\tPhone number: %s\n", get_user_info(user, 3));
    printf("\tBirth date: %s\n", get_user_info(user, 4));
    printf("\tSex: %s\n", get_user_info(user, 5));
    printf("\tPassport: %s\n", get_user_info(user, 6));
    printf("\tCountry code: %s\n", get_user_info(user, 7));
    printf("\tAddress: %s\n", get_user_info(user, 8));
    printf("\tAccount creation: %s\n", get_user_info(user, 9));
    printf("\tPay method: %s\n", get_user_info(user, 10));
    printf("\tAccount status: %s\n", get_user_info(user, 11));
}

// Function to parse a CSV file and populate a GHashTable with User structs
/**
 * @brief Parses a CSV file and populates a GHashTable with User structs.
 * 
 * @param datasetDir The directory of the dataset.
 * @param outputDir The directory of the output.
 * @return GHashTable* The hash table with the users.
*/
GHashTable* parse_users(const char* datasetDir, const char* outputDir) { 
    GHashTable* users = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_user);

    char datasetPath[MAX_PATH_LENGTH];
    // if datasetDir ends in /, remove it
    if (datasetDir[strlen(datasetDir) - 1] == '/') {
        snprintf(datasetPath, MAX_PATH_LENGTH, "%s%s", datasetDir, DATASET_NAME);
    } else {
        snprintf(datasetPath, MAX_PATH_LENGTH, "%s/%s", datasetDir, DATASET_NAME);
    }
    FILE* file = fopen(datasetPath, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_SIZE];
    // Read header line (assuming the first line is the header)
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        exit(EXIT_FAILURE);
    }

    // initialize error registery
    FILE *error_registery = initialize_error_registery(outputDir, ERRORS_DATASET_NAME);

    // add header to errors file
    line[strcspn(line, "\n")] = '\0';
    register_error_line(error_registery, line);

    int line_count = 0;
    while (fgets(line, sizeof(line), file)) {
        line_count++;
        // Remove newline character from the end
        line[strcspn(line, "\n")] = '\0';

        gchar** tokens = g_strsplit(line, ";", -1);
        USER* user = create_user();

        if (tokens) {
            set_user_id(user, tokens[0]);
            set_user_info(user, 1, tokens[1]); // name
            set_user_info(user, 2, tokens[2]); // email
            set_user_info(user, 3, tokens[3]); // phone_number
            set_user_info(user, 4, tokens[4]); // birth_date
            set_user_info(user, 5, tokens[5]); // sex
            set_user_info(user, 6, tokens[6]); // passport
            set_user_info(user, 7, tokens[7]); // country_code
            set_user_info(user, 8, tokens[8]); // address
            set_user_info(user, 9, tokens[9]); // account_creation
            set_user_info(user, 10, tokens[10]); // pay_method
            set_user_info(user, 11, tokens[11]); // account_status

            // if validations fail
            if (validateDate(user->birth_date) != 1
            || validateDate(user->account_creation) != 1
            || validateEmail(user->email) != 1
            || validateCCode(user->country_code) != 1
            || validateStatus(user->account_status) != 1
            || compareDates(user->birth_date, user->account_creation) != 1
            || validateFieldSize(user->id) != 1 || validateFieldSize(user->name) != 1 || validateFieldSize(user->phone_number) != 1 || validateFieldSize(user->sex) != 1 || validateFieldSize(user->passport) != 1 || validateFieldSize(user->address) != 1 || validateFieldSize(user->pay_method) != 1) {
                //add to errors file later
                //register_error_line(ERRORS_DATASET_NAME, outputDir, line);
                register_error_line(error_registery, line);
                free_user(user);
            } else {
                g_hash_table_insert(users, user->id, user);
            }
            
            g_strfreev(tokens); // Free the array of strings
        } else {
            // Handle the case where g_strsplit fails
            fprintf(stderr, "Error splitting line: %s\n", line);
            free_user(user);
            continue;
        }
    }

    // close error registery
    close_error_registery(error_registery);

    fclose(file);

    return users;
}

// Function to free the memory used by the hash table and its contents
/**
 * @brief Frees the memory used by the hash table and its contents.
 * 
 * @param users The hash table to be freed.
*/
void free_users(GHashTable* users) {
    g_hash_table_destroy(users);
}

// Function to get a user from the hash table
/**
 * @brief Gets a user from the hash table.
 * 
 * @param users The hash table of users.
 * @param user_id The user id.
 * 
 * @return USER* The user. @see struct USER
*/
USER *get_user(GHashTable *users, const char *user_id) {
    return (USER *) g_hash_table_lookup(users, user_id);
}

// isValidUser is used in other dataset verifications, if the user is not valid it means other data in other datasets is not valid
// so we can just skip the other datasets
// if it exists, it is valid (it passed validation for the users dataset to the hash table)
/**
 * @brief Checks if a user is valid.
 * 
 * @param users The hash table of users.
 * @param user_id The user id.
 * 
 * @return int 1 if valid, 0 if not.
*/
int isValidUser(GHashTable *users, const char *user_id) {
    return g_hash_table_contains(users, user_id);
}