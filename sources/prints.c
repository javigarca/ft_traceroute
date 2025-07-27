#include <bits/types/struct_timeval.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "ft_traceroute.h"

/**
 * @brief Función de impresion de ayuda copleta del programa
 * 
 */
void    print_help(){
    printf("\nUsage:\n  ft_traceroute [options] <host>\n");
    printf("Options:\n");
    printf("  -V  --version               Print version info and exit\n");
    printf("  --help                      Read this help and exit\n");
    printf("Arguments:\n+     host          The host to traceroute to\n");
}

/**
 * @brief Función de impresion de la verisión.
 * 
 */
void    print_version(){
    printf("(42 Urduliz Project) - ft_traceroute for Linux, version 0.2.2\n");
    printf("Copyright (c) 2025 Javier García-Arango Vázquez-Prada, License: none (All rights reserved)\n");
}
 /**
 * @brief Función de impresión de error y salida del programa 
 * 
 * Imprime un mensaje de error personalizado, seguido opcionalmente 
 * del mensaje asociado a un código de error del sistema (usando strerror),
 * y termina la ejecución del programa con el código de salida especificado.
 * 
 * @param status Valor que tomará la función exit() al terminar el programa.
 * @param errnum Código de error del sistema (por ejemplo, errno). Si es 0, no se imprime mensaje de error del sistema.
 * @param fmt Cadena de formato estilo printf para el mensaje de error personalizado.
 * @param ... Argumentos variables usados junto con fmt para construir el mensaje de error.
  */
void    error_exit(int status, int errnum, const char *fmt, ...){
    fprintf(stderr, "ft_traceroute: ");

    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }

    if (errnum)
        fprintf(stderr, ": %s", strerror(errnum));

    fprintf(stderr, "\n");
    exit(status);
}

/**
 * @brief Función para imprimir el resumen de las estadísticas que recibe como parámetro
 * 
 * @param stats estructura de las esteadisticas 
 */
 /*
void    print_summary(t_stats *stats){
	double loss = 0.0;
    struct timeval end_traceroute;
    gettimeofday(&end_traceroute, NULL);
    long elapsed_time = (end_traceroute.tv_sec - stats->start_traceroute.tv_sec) * 1000 + (end_traceroute.tv_usec - stats->start_traceroute.tv_usec) / 1000;

	if (stats->transmitted > 0)
		loss = 100.0 * (stats->transmitted - stats->received) / stats->transmitted;

	printf("\n--- %s traceroute statistics ---\n", stats->target.hostname);
	printf("%d packets transmitted, %d received, %.0f%% packet loss, time %ldms\n",
		stats->transmitted, stats->received, loss, elapsed_time);

	if (stats->received > 0) {
		double avg = stats->rtt_total / stats->received;
		double mdev = 0;
		if (stats->received > 1) {
			double mean_sq = stats->rtt_squared_total / stats->received;
			mdev = sqrt(mean_sq - avg * avg);
		}
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats->rtt_min, avg, stats->rtt_max, mdev);
	}
}
*/
/**
 * @brief Función para imprimir mensajes en función de debug
 * 
 * @param debug valor del flag  
 * @param fmt Cadena de formato estilo printf para el mensaje personalizado.
 * @param ... Argumentos variables usados junto con fmt para construir el mensaje.
 */

 void   print_infof(int debug, FILE *stream, const char *fmt, ...){
    if (!debug)
        return;
    va_list args;
    va_start(args, fmt);
    vfprintf(stream, fmt,args);
	va_end(args);
	fprintf(stream, "\n");
 }