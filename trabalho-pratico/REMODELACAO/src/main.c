/**
* @file main.c
* @brief Main program file for a dataset manager system.
*/
#include "batchMode.h"
#include "iteractiveMode.h"
#include "utils.h"
#include "parser.h"
#include "catalog.h"
#include "unitTesting.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#define OUTPUT_DIR "Resultados/"

// performance testing
#include <time.h>
#include <sys/resource.h>

/**
 * @brief Main function for the dataset manager system.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Returns 0 upon successful execution.
*/
int main(int argc, char *argv[]) {
	// start performance testing
	clock_t start, end;
	start = clock();
    if (argc > 1) { // Se tivermos argumentos, estamos em modo batch diretamente, programa-teste <caminho para o dataset com os CSVs, o ficheiro com os comandos a executar, e uma pasta com os ficheiros de output esperado>
		if (argc >= 3 && argc <= 4) {
			char *datasetDir = argv[1];
			char *inputFile = argv[2];
			char *outputDir = argv[3];
			int runninTests = 0;
			if (argc >= 3 && strstr(argv[0], "programa-testes") != NULL) { // or: argc > 3 && outputDir != NULL
				runninTests = 1;
			}
			clock_t parser_start, parser_end;
			parser_start = clock();
			GHashTable *users = parse_users(datasetDir, OUTPUT_DIR);
			parser_end = clock();
			if (runninTests) {
				double time_taken = ((double)parser_end - parser_start) / CLOCKS_PER_SEC;
				printf("Users parser executed in time: %fs\n", time_taken);
			}
			parser_start = clock();
			GHashTable *reservations = parse_reservations(datasetDir, OUTPUT_DIR, users);
			parser_end = clock();
			if (runninTests) {
				double time_taken = ((double)parser_end - parser_start) / CLOCKS_PER_SEC;
				printf("Reservations parser executed in time: %fs\n", time_taken);
			}
			parser_start = clock();
			GHashTable *flights = parse_flights(datasetDir, OUTPUT_DIR);
			parser_end = clock();
			if (runninTests) {
				double time_taken = ((double)parser_end - parser_start) / CLOCKS_PER_SEC;
				printf("Flights parser executed in time: %fs\n", time_taken);
			}
			parser_start = clock();
			GHashTable *passengers = parse_passengers(datasetDir, OUTPUT_DIR, users, flights);
			parser_end = clock();
			if (runninTests) {
				double time_taken = ((double)parser_end - parser_start) / CLOCKS_PER_SEC;
				printf("Passengers parser executed in time: %fs\n", time_taken);
			}
			CATALOG *c = newCatalog(users, passengers, flights, reservations);
			batchMode(inputFile, OUTPUT_DIR, c, runninTests);
			//g_hash_table_foreach(users, print_hash_user, NULL);
			//printf("Tamanho da hash table users: %u\n", g_hash_table_size(users));
			//free_users(users);
			//g_hash_table_foreach(reservations, print_hash_reservation, NULL);
			//printf("Tamanho da hash table reservations: %u\n", g_hash_table_size(reservations));
			//free_reservations(reservations);
			//g_hash_table_foreach(passengers, print_hash_passenger, NULL);
			//printf("Tamanho da hash table passengers: %u\n", g_hash_table_size(passengers));
			//free_passengers(passengers);
			//g_hash_table_foreach(flights, print_hash_flight, NULL);
			//printf("Tamanho da hash table flights: %u\n", g_hash_table_size(flights));
			//free_flights(flights);
			if (argc > 3 && outputDir != NULL && strstr(argv[0], "programa-testes") != NULL) { // or: argc > 3 && outputDir != NULL
				run_unit_tests(OUTPUT_DIR, outputDir);
			}
			free_catalog(c);
		} else {
			printf("Usage: programa-principal <caminho para o dataset com os CSVs> <ficheiro com os comandos a executar>\n");
			printf("       programa-testes <caminho para o dataset com os CSVs> <ficheiro com os comandos a executar> <pasta com os ficheiros de output esperado>\n");
			return 1;
		}
    } else {
		iteractiveMode();
	}
	// end performance testing
	if (argc >= 0 && strstr(argv[0], "programa-testes") != NULL) {
		struct rusage r_usage;
		getrusage(RUSAGE_SELF, &r_usage);
		printf("Memory usage: %ld KB\n", r_usage.ru_maxrss);
		end = clock();
		double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("CPU time used: %f\n", cpu_time_used);
	}
    return 0;
}