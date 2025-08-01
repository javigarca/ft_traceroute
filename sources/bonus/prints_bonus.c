#include <bits/types/struct_timeval.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "ft_traceroute_bonus.h"

/**
 * @brief Función de impresion de ayuda copleta del programa
 * 
 */
void    print_help(){
    printf("\nUsage:\n  ft_traceroute [options] <host>\n");
    printf("Options:\n");
    printf("  -p port                     Set the destination port to use as initial udp port value for \"default\" method\n");
    printf("                              (incremented by each probe, default is 33434)\n");
    printf("  -V  --version               Print version info and exit\n");
    printf("  --help                      Read this help and exit\n");
    printf("Arguments:\n");
    printf("+     host          The host to traceroute to\n");
    printf("      packetlen     The full packet length (default is the length of an IP header plus 40).\n");
    printf("                    Can be ignored or increased to a minimal allowed value");
}

/**
 * @brief Función de impresion de la verisión.
 * 
 */
void    print_version(){
    printf("(42 Urduliz Project) - ft_traceroute for Linux, version 0.8.9\n");
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
 * @brief Función para imprimir mensajes sin línea nueva.
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
 }

 /**
 * @brief Función para imprimir mensajes en función de debug, línea nueva al final.
 * 
 * @param debug valor del flag  
 * @param fmt Cadena de formato estilo printf para el mensaje personalizado.
 * @param ... Argumentos variables usados junto con fmt para construir el mensaje.
 */

 void   print_infofn(int debug, FILE *stream, const char *fmt, ...){
    if (!debug)
        return;
    va_list args;
    va_start(args, fmt);
    vfprintf(stream, fmt,args);
	va_end(args);
	fprintf(stream, "\n");
 }

 /**
 * @brief Función debug para imprimir las ping options
 * 
 * @param opts estructura de ping options
 */
void    print_opts(const t_traceroute_options *opts) {
    printf("=== Parsed options ===\n");
    printf("  port          = %d\n", opts->port);
    //printf("  count         = %d\n", opts->count);
    //printf("  interval      = %.6f\n", opts->interval);
    printf("  target host   = %s\n", opts->host ? opts->host : "(null)");
    //printf("  route         = %d\n", opts->route);
    //printf("  use_pattern   = %d\n", opts->pattern_use);
    //printf("  ttl           = %d\n", opts->ttl_use);
    //if (opts->ttl_use)
    //    printf("  ttl value     = %d\n", opts->ttl);
    //if (opts->pattern_use) {
    //    printf("  pattern_len   = %zu\n", opts->pattern_len);
    //printf("  pattern bytes =");
     //   for (size_t i = 0; i < opts->pattern_len; i++) {
     //       printf(" %02" PRIx8, opts->pattern[i]);
     //   }
      //  printf("\n");
 //  }

    printf("  use packetlen      = %d\n", opts->packet_len_use);
    if (opts->packet_len_use)
        printf("  packetsize    = %zu\n", opts->packet_len);

    printf("======================\n");
}