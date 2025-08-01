#include <unistd.h>
#include <string.h>     // memset, strerror
#include <netdb.h>      // getaddrinfo, freeaddrinfo, addrinfo
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>      
#include <stdlib.h>     
#include <errno.h>

#include "ft_traceroute_bonus.h"
#include "ft_traceroute_definitions_bonus.h"
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

    int     i = 1;
    int     host_count = 0;
    int     pos_count = 0;
    char    flag;
    char    *val = NULL;
    
    opts->port = BASE_DST_PORT;
    opts->packet_len = WIRE_BYTES;

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
                        case 's':
                        case 't':
                        case 'p':
                        case 'i':
                        case 'c':
                            flag = argv[i][j];
                            // Caso "-c123" (sin espacio)
                            if (argv[i][j+1] != '\0') {
                                val = &argv[i][j+1];
                            } else {
                                // Caso "-c 123" (espacio separado)
                                if (i + 1 >=argc)
                                    error_exit(EXIT_FAILURE, 0, "Option -%c requires an argument", argv[i][j]);
                                val = argv[++i];
                            }
                            validate_flag_arg(val, flag, opts);
                            // Salimos del inner-loop para no reexaminar los dígitos de "123"
                            j = strlen(argv[i]) - 1;
                            break;
                        default: 
                            error_exit(EXIT_FAILURE, 0, "Unknown option: -%c", argv[i][j]);
                    }
                }
            }            
        }
        else {
            pos_count++;
            if (pos_count == 1) {
                host_count++;
                opts->target.hostname = argv[i];
            }
            else if (pos_count == 2) {
                // segundo posicional = packetlen
                validate_flag_arg(argv[i], 'x', opts);
                int plen = atoi(argv[i]);
                if ( plen > 65500) {
                    error_exit(EXIT_FAILURE, 0, "too big packetlen %d specified", plen); 
                }        
                opts->packet_len = plen;
            }
            else {
                error_exit(EXIT_FAILURE, 0, "Extra arg `%s' (position %d, argc %d)", argv[i], pos_count, argc);
            }
        }
        i++;
    }

    if (host_count == 0) {
        error_exit(EXIT_FAILURE, 0, "Specify \"host\" missing argument");
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

/**
 * @brief Función para validar el argumento de las flag que aceptan un número de arg como -c e -i. Salimos con error en caso negativo
 * 
 * @param value valor que recibimos del argumento
 * 
 */
void    validate_flag_arg(char *value, char flag, t_traceroute_options *opts){
    errno = 0;
    char *endptr;
    if (flag == 'p'){
        long port = strtol(value, &endptr, 10);
        if (*endptr != '\0') 
            error_exit(EXIT_FAILURE, 0,"invalid argument: '%s'", value);
        if ( port < 1 || port > 65535) 
            error_exit(EXIT_FAILURE, 0, "invalid argument: '%s': out of range: 1 <= value <= 65535", value);
        opts->port = (int)port;
    }
    if (flag == 'x') {
        long plen = strtol(value, &endptr, 10);
        if (*endptr != '\0') 
            error_exit(EXIT_FAILURE, 0,"invalid argument: '%s'", value);
        if ( plen > 65500) {
            error_exit(EXIT_FAILURE, 0, "too big packetlen %d specified", plen); 
        }        
        opts->packet_len = (int)plen;
        opts->packet_len_use = 1;
    }      
    /*if (flag == 'i'){
        double interval = strtod(value, &endptr);
        if (*endptr != '\0') 
            error_exit(EXIT_FAILURE, 0,"invalid argument: '%s'", value);
        if (errno == ERANGE || interval <=0.0)
            error_exit(EXIT_FAILURE, 0, "invalid argument: '%s': out of range: 1 <= value <= %ld", value, LONG_MAX);
        opts->interval = interval;
    }*/
    /*if (flag == 'p'){
        size_t hexlen = strlen(value);
        if (hexlen < 2 || hexlen > MAX_PATTERN_LEN*2 || hexlen%2 != 0)
            error_exit(EXIT_FAILURE, 0, "invalid argument: '%s'", value);
        opts->pattern_len = hexlen / 2; //de digitos a bytes
        for (size_t i = 0; i < opts->pattern_len; i++) {
            char byte_str[3] = {value[2*i], value[2*i+1], '\0'};
            long v = strtol(byte_str, &endptr, 16);
            if (*endptr != '\0')
                error_exit(EXIT_FAILURE, 0,"invalid argument: '%s'", value);
            opts->pattern[i] = (uint8_t)v;
        }
        opts->pattern_use = 1;
    }*/
    /*if (flag == 's'){
        long sz = strtol(value, &endptr, 10);
        if (*endptr != '\0') 
            error_exit(EXIT_FAILURE, 0,"invalid argument: '%s'", value);
        if (errno == ERANGE || sz < 0 || sz > MAX_PAYLOAD_SIZE)
            error_exit(EXIT_FAILURE, 0, "invalid argument: '%s': out of range: 1 <= value <= %ld", value, MAX_PAYLOAD_SIZE);
        opts->payload_size = (size_t)sz;
        opts->payload_size_use = 1;
    }*/
    /*if (flag == 't'){
        long ttl = strtol(value, &endptr, 10);
        if (*endptr != '\0') 
            error_exit(EXIT_FAILURE, 0,"invalid argument: '%s'", value);
        if (errno == ERANGE || ttl <= 0 || ttl > 255)
            error_exit(EXIT_FAILURE, 0, "invalid argument: '%s': out of range: 1 <= value <= 255", value);
        opts->ttl = ttl;
        opts->ttl_use = 1;
    }
    
    
    */
}
