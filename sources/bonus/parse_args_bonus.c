#include <unistd.h>
#include <string.h>     // memset, strerror
#include <netdb.h>      // getaddrinfo, freeaddrinfo, addrinfo
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>      
#include <stdlib.h>     
#include <errno.h>

#include "ft_traceroute_bonus.h"
#include "ft_traceroute_structs_bonus.h"

/**
 * @brief Extrae las flags y el host de los argumentos introducidos, evalúa y ejecuta según sea necesario.
 * 
 * @param argc número de argumentos de main
 * @param argv valor de los argumentos de main
 * @param opts estructura de opciones para dar valor según sea el caso
 */

void parse_args(int argc, char *argv[], t_traceroute_options *opts){

    if (argc < 2){
        print_help();
        exit(EXIT_SUCCESS);
    }

    int i = 1;
    int hostcont = 0;

    while (argv[i]) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-'){
                if (!strcmp(argv[i], "--help")){
                        print_help();
                        exit(EXIT_SUCCESS);
                }
                if (!strcmp(argv[i], "--version")){
                        print_version();
                        exit(EXIT_SUCCESS);
                }
                error_exit(EXIT_FAILURE, 0, "Unknown option: %s", argv[i]);   
            } else {
                for (int j = 1; argv[i][j]; j++){
                    switch (argv[i][j]) {
                        case 'V': 
                            print_version();
                            exit(EXIT_SUCCESS);
                        case 'd': 
                            opts->debug=1;
                            break;
                        case '?': 
                            print_help(); 
                            exit(EXIT_SUCCESS);
                        default: 
                            error_exit(EXIT_FAILURE, 0, "Unknown option: -%c", argv[i][j]);
                    }
                }
            }
            
        }
        else {
            hostcont++;
            opts->target.hostname = argv[i]; 
        }
        i++;
    }

    if (hostcont == 0) {
        error_exit(EXIT_FAILURE, 0, "Specify \"host\" missing argument");
    }
    if (hostcont != 1) {
        error_exit(EXIT_FAILURE, 0,"only one destination allowed");
    }
}

/**
 * @brief Valida el host introducido por el usuario y lo introudce en target. devuelve 0 si todo bien
 * 
 * @param opts 
 * @param t_out 
 * @return int 
 */
int resolve_target(t_traceroute_options *opts){
        
    struct addrinfo hints;
	struct addrinfo *result;
	int ret;

	// Preparar estructura de filtro
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // Solo IPv4
	hints.ai_socktype = SOCK_RAW;    // 
	hints.ai_protocol = IPPROTO_ICMP;

	// Resolver
	ret = getaddrinfo(opts->target.hostname, NULL, &hints, &result);
	if (ret != 0) {
		error_exit(EXIT_FAILURE, 0, "%s: %s", opts->target, gai_strerror(ret));
	}

	// Extraer sockaddr_in
	struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
	opts->target.addr = *addr;

	// Convertir IP a string
	if (!inet_ntop(AF_INET, &addr->sin_addr, opts->target.ip_str, INET_ADDRSTRLEN)) {
		freeaddrinfo(result);
		error_exit(EXIT_FAILURE, errno, "inet_ntop");
	}

	freeaddrinfo(result);
	return (0);
}
