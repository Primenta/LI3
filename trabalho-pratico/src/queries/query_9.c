/**
 * @file query_9.c
 * @brief Implementation of query 9.
*/
#include "queries.h"
#include "structs.h"
#include "parser.h"
#include "statistics.h"
#include "validation.h"
#include "utils.h"
#include <locale.h>

/*
query_9: Listar todos os utilizadores cujo nome começa com o prefixo passado por argumento, ordenados
por nome (de forma crescente). Caso dois utilizadores tenham o mesmo nome, deverá ser usado
o seu identificador como critério de desempate (de forma crescente). Utilizadores inativos não
deverão ser considerados pela pesquisa.
*/
// Function to compare two users name for sorting (without format_flag)
/**
 * @brief Function to compare two users name for sorting (without format_flag).
 * 
 * @param a The first user.
 * @param b The second user.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_username(gconstpointer a, gconstpointer b) {
    setlocale(LC_COLLATE, "en_US.UTF-8");
    int comparator = 0;

    char *user_a = (char *)a;
    char *user_b = (char *)b;

    //printf("Comparing %s with %s\n", user_a, user_b);

    // split string into tokens using g_strsplit
    char **user_a_tokens = g_strsplit(user_a, ";", 0);
    char **user_b_tokens = g_strsplit(user_b, ";", 0);

    // get the name and id from the tokens
    char *user_a_name = user_a_tokens[1];
    char *user_b_name = user_b_tokens[1];

    char *user_a_id = user_a_tokens[0];
    char *user_b_id = user_b_tokens[0];

    // compare the name
    int name_cmp = strcoll(user_a_name, user_b_name);
    if (name_cmp == 0) {
        // if theres a tie, compare the id
        comparator = strcoll(user_a_id, user_b_id);
    } else {
        comparator = name_cmp;
    }

    // free the tokens
    g_strfreev(user_a_tokens);
    g_strfreev(user_b_tokens);

    return comparator;
}


// Function to compare two users name for sorting (with format_flag)
/**
 * @brief Function to compare two users name for sorting (with format_flag).
 * 
 * @param a The first user.
 * @param b The second user.
 * @return gint The comparator (0 if equal, > 0 if a > b, < 0 if a < b).
*/
static gint compare_username_format(gconstpointer a, gconstpointer b) {
    setlocale(LC_COLLATE, "en_US.UTF-8");
    int comparator = 0;

    char *user_a = (char *)a;
    char *user_b = (char *)b;

    //printf("Comparing %s with %s\n", user_a, user_b);

    // split string into tokens using g_strsplit
    char **user_a_tokens = g_strsplit(user_a, "\n", 0);
    char **user_b_tokens = g_strsplit(user_b, "\n", 0);

    // get the name and id from the tokens
    char **user_a_name_tokens = g_strsplit(user_a_tokens[1], ": ", 0);
    char **user_b_name_tokens = g_strsplit(user_b_tokens[1], ": ", 0);

    char *user_a_name = user_a_name_tokens[1];
    char *user_b_name = user_b_name_tokens[1];

    char **user_a_id_tokens = g_strsplit(user_a_tokens[0], ": ", 0);
    char **user_b_id_tokens = g_strsplit(user_b_tokens[0], ": ", 0);

    char *user_a_id = user_a_id_tokens[1];
    char *user_b_id = user_b_id_tokens[1];

    // compare the name
    int name_cmp = strcoll(user_a_name, user_b_name);
    if (name_cmp == 0) {
        // if theres a tie, compare the id
        comparator = strcoll(user_a_id, user_b_id);
    } else {
        comparator = name_cmp;
    }

    // free the tokens
    g_strfreev(user_a_tokens);
    g_strfreev(user_b_tokens);
    g_strfreev(user_a_name_tokens);
    g_strfreev(user_b_name_tokens);
    g_strfreev(user_a_id_tokens);
    g_strfreev(user_b_id_tokens);

    return comparator;
}

/**
 * @brief Returns all users whose name starts with the prefix passed as an argument, sorted by name (in ascending order). If two users have the same name, their identifier should be used as a tiebreaker (in ascending order). Inactive users should not be considered in the search.
 * 
 * @param c The catalog. @see struct CATALOG
 * @param format_flag The format flag. @see command_interpreter
 * @param args The arguments. @see command_interpreter
 * @param args_size The size of the arguments. @see command_interpreter
 * @param usersList The result list.
 */
void query_9(CATALOG *c, int format_flag, char **args, int args_size, GList **usersList) {
    *usersList = NULL;

    if (args_size < 1) {
        return; // Handle missing arguments
    }

    // Iterate through the users in the catalog
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, c->users);

    char* prefix = args[0];

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        USER *user = (USER *)value;
        // Check if the user name starts with the prefix
        if (strncmp(user->name, prefix, strlen(prefix)) == 0) {
            // check if user is active
            if (isActive(user->account_status) == 0) continue;

            //printf("User found: %s for prefix %s\n", user->name, prefix);
            char* userStr = NULL;

            if (format_flag) {
                char *str = "id: %s\nname: %s\n\n";
                int len = snprintf(NULL, 0, str, user->id, user->name);
                userStr = g_malloc(len + 1);
                snprintf(userStr, len + 1, str, user->id, user->name);
            } else {
                char *str = "%s;%s\n";
                int len = snprintf(NULL, 0, str, user->id, user->name);
                userStr = g_malloc(len + 1);
                snprintf(userStr, len + 1, str, user->id, user->name);
            }

            *usersList = g_list_append(*usersList, userStr);
        }
    }

    // Sort the list based on name and id
    if (format_flag) {
        // if theres a format flag we sort but the "--- %d ---\n" must be added to the string after sorting them so "--- 1 ---" if always the first line and so on, but the data inside is sorted
        *usersList = g_list_sort(*usersList, (GCompareFunc)compare_username_format);
        // after sort we add the "--- %d ---\n" to the string
        GList *iterList = *usersList;
        int number_of_results = 0;
        while (iterList != NULL) {
            char *userStr = iterList->data;
            char *str = "--- %d ---\n%s";
            int len = snprintf(NULL, 0, str, number_of_results + 1, userStr);
            char *userStrFormatted = g_malloc(len + 1);
            // if it is the last line, we remove the \n
            if (iterList->next == NULL) {
                snprintf(userStrFormatted, len + 1, str, number_of_results + 1, userStr);
                userStrFormatted[len - 1] = '\0';
            } else {
                snprintf(userStrFormatted, len + 1, str, number_of_results + 1, userStr);
            }
            g_free(userStr);
            iterList->data = userStrFormatted;
            iterList = g_list_next(iterList);
            number_of_results++;
        }
    } else {
        *usersList = g_list_sort(*usersList, (GCompareFunc)compare_username);
    }
}