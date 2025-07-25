#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "ft_traceroute.h"
#include "ft_traceroute_definitions.h"
#include "ft_traceroute_structs.h"

/**
 * @brief Flujo principal del programa
 * 
 * recibe los argumentos, inicializa estructuras, parseo de argumentos, creación de socket, envío de socket y respuesta. Tratamiento de señales, crtl+C. 
 *
 * varible global de puntero controlado a la dirección de la estructura de la estadística para solo usarla en el caso de imprimir el resumen de las estadísticas, 
 * y no andar cambiando glabales todo el tiempo.

 * @param argc 
 * @param argv 
 * @return int 
 */

static t_stats *g_stats_ref = NULL;

int main (int argc, char **argv)
{
    t_traceroute_options opts = {0};
    t_stats stats = {0};
    g_stats_ref = &stats;

    parse_args(argc, argv, &opts);   
    signal(SIGINT, handle_sigint);

    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd < 0){
        if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)
		    error_exit(EXIT_FAILURE, 0, "ft_traceroute: Lacking privilege for icmp socket.\n");
        error_exit(EXIT_FAILURE, errno, "socket" );
    }
    //Activar IP_RECVTTL en el socket:
    int opt = 1;
    setsockopt(socket_fd, IPPROTO_IP, IP_RECVTTL, &opt, sizeof(opt));
    
    if (resolve_target(&opts, &stats.target))
        error_exit(EXIT_FAILURE, 0, "Error resolving host.");
    get_socket_info(socket_fd, &stats);
  
    ////impresión cabeceras
    print_infof(opts.debug, stderr, "ft_traceroute: sock4.fd: %d (socktype: %s), sock6.fd: -1 (socktype: 0), hints.ai_family: %s.\n", socket_fd, stats.socket_i.socktype_str, stats.socket_i.family_str);
    print_infof(opts.debug,stdout, "ai->ai_family: %s, ai->ai-canonname: '%s'", stats.socket_i.family_str, stats.target.hostname);
    print_infof(1, stdout, "traceroute %s (%s) %d(%d) bytes of data.", stats.target.hostname, stats.target.ip_str, PAYLOAD_SIZE, WIRE_BYTES);

    int seq = 1;
    gettimeofday(&stats.start_traceroute, NULL);
   
    while(1){
        send_packet(socket_fd, &opts, &stats, seq);
        //timestamp de comienzo de bucle
        struct timeval start, now;
        gettimeofday(&start, NULL);

        while (1) {
            // cálculo tiempo restante 
            gettimeofday(&now, NULL);
            double elapsed = (now.tv_sec - start.tv_sec)
                        + (now.tv_usec - start.tv_usec)/1e6;
            double left = 1.0 - elapsed;
            if (left <= 0.0)
                break;

            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(socket_fd, &rfds);
            // un select con el tiempo restante    
            struct timeval tv = {
                .tv_sec  = (int)left,
                .tv_usec = (int)((left - (int)left)*1e6)
            };

            int reply = select(socket_fd+1, &rfds, NULL, NULL, &tv);
            if (reply < 0)
                error_exit(EXIT_FAILURE, errno, "select");
            if (reply == 0)
                break;    // timeout

            //analisis de packete
            if (receive_packet(socket_fd, seq, &opts, &stats)) {
                break;
            }
            /*si no es correcto el paquete o no encontramos nada, empezamos bucle otra vez */
        }
        seq++;
        sleep(1);
    }
    return (EXIT_SUCCESS);
}

/**
 * @brief Función para controlar la señal SIGINT
 * 
 * @param signum el valor de la señal
 */
void    handle_sigint(int signum){
    (void)signum;
    if (g_stats_ref)
		print_summary(g_stats_ref);
    exit(EXIT_SUCCESS);
}