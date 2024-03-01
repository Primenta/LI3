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

// Function to free memory allocated for a User struct
/**
 * @brief Frees the memory allocated for a user.
 * 
 * @param data The data to be freed.
*/
void free_user(gpointer data) {
    USER *user = (USER *)data;
    g_free(user->id);
    g_free(user->name);
    g_free(user->email);
    g_free(user->phone_number);
    g_free(user->birth_date);
    g_free(user->sex);
    g_free(user->passport);
    g_free(user->country_code);
    g_free(user->address);
    g_free(user->account_creation);
    g_free(user->pay_method);
    g_free(user->account_status);
    g_free(user);
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
    printf("User: %s\n", user->id);
    printf("\tName: %s\n", user->name);
    printf("\tEmail: %s\n", user->email);
    printf("\tPhone number: %s\n", user->phone_number);
    printf("\tBirth date: %s\n", user->birth_date);
    printf("\tSex: %s\n", user->sex);
    printf("\tPassport: %s\n", user->passport);
    printf("\tCountry code: %s\n", user->country_code);
    printf("\tAddress: %s\n", user->address);
    printf("\tAccount creation: %s\n", user->account_creation);
    printf("\tPay method: %s\n", user->pay_method);
    printf("\tAccount status: %s\n", user->account_status);
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
        USER* user = g_new0(USER, 1);

        if (tokens) {
            user->id = g_strdup(tokens[0]);
            user->name = g_strdup(tokens[1]);
            user->email = g_strdup(tokens[2]);
            user->phone_number = g_strdup(tokens[3]);
            user->birth_date = g_strdup(tokens[4]);
            user->sex = g_strdup(tokens[5]);
            user->passport = g_strdup(tokens[6]);
            user->country_code = g_strdup(tokens[7]);
            user->address = g_strdup(tokens[8]);
            user->account_creation = g_strdup(tokens[9]);
            user->pay_method = g_strdup(tokens[10]);
            user->account_status = g_strdup(tokens[11]);

            // Handle the case where user->id is empty
            /*if (strlen(user->id) == 0 || user->id == NULL) {
                //add to errors file later
                //register_error_line(ERRORS_DATASET_NAME, outputDir, line);
                //printf("Empty id on line %d; Line: %s\n", line_count, line);
                // add line anyways but set id "PROGRAM_ERROR_(linenumber)"
                char error_id[32];
                snprintf(error_id, 32, "PROGRAM_ERROR_%d", line_count);
                g_free(user->id);
                user->id = g_strdup(error_id);
                //register_error_line(error_registery, line);
            }*/

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
            g_free(user); // Free the partially allocated USER struct
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