#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include "ft_traceroute.h"
#include "ft_traceroute_definitions.h"
#include "ft_traceroute_structs.h"

volatile sig_atomic_t g_interrupted;

/**
 * @brief Función para controlar la señal SIGINT
 * 
 * @param signum el valor de la señal
 */
void    sigint_handler(int signum){
    (void)signum;
    g_interrupted = 1;
}

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

int main (int argc, char **argv)
{
    t_traceroute_options opts = {0};
    struct sigaction sa = {0};
    int socket_send, socket_recv;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL)) {
        error_exit(EXIT_FAILURE, errno, "sigaction error.");
    }

    parse_args(argc, argv, &opts);   
    socket_send = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_send < 0){
        error_exit(EXIT_FAILURE, errno, "socket UDP" );
    }
    socket_recv = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_recv < 0){
        if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)
		    error_exit(EXIT_FAILURE, 0, "ft_traceroute: Lacking privilege for ICMP socket.\n");
        error_exit(EXIT_FAILURE, errno, "socket ICMP" );
    }
    
    if (resolve_target(&opts))
        error_exit(EXIT_FAILURE, 0, "Error resolving host.");

    ////impresión cabecera
    print_infof(1, stdout, "traceroute to %s (%s) %d(%d) bytes of data.", opts.target.hostname, opts.target.ip_str, PAYLOAD_SIZE, WIRE_BYTES);

    for (size_t ttl = 1; ttl <= NUM_TTL && g_interrupted == 0; ttl++){
         if (setsockopt(socket_send, IPPROTO_UDP, IP_RECVTTL, &ttl, sizeof(ttl)) < 0)
            error_exit(EXIT_FAILURE, 0, "Error setting TTL: %u", ttl);
        print_infof(1, stdout, "%u ", ttl);
        for (size_t probe = 0; probe < NUM_PROBES; probe++) {
            struct timeval t_send, t_recv;
            long t_rtt_ms;

            gettimeofday(&t_send, NULL);
            //envío de probe
            //recepcion de probe e impresión de su tiempo
            gettimeofday(&t_recv, NULL);
            t_rtt_ms = (t_recv.tv_sec  - t_send.tv_sec) * 1000 + (t_recv.tv_usec - t_send.tv_usec) / 1000;
            // si timeout, imprimir '*'
            print_infof(1, stdout,"%l ", t_rtt_ms);
            
        }
    }

    ////******////*/*/*/*/*/*//***/**/*/*/*/*/*/*/*/* */ */ */
    
    /*/
    Generar el bucle aumentando el ttl hasta lo definido.
        antes sleccionar el ttl correcto para ese intento
        envio de probes hasta el númeor definido
            recibir por cada probe e imprimir la info.
    si se rompre por señal, cerra sockets y pista
    salir

    //Activar IP_RECVTTL en el socket:
    int opt = 1;
    setsockopt(socket_send, IPPROTO_IP, IP_RECVTTL, &opt, sizeof(opt));
  

    int seq = 1;
    
   
    while(1){
        send_packet(socket_fd, &opts, seq);
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
            if (receive_packet(socket_fd, seq, &opts)) {
                break;
            }
            //si no es correcto el paquete o no encontramos nada, empezamos bucle otra vez 
        }
        seq++;
        sleep(1);
    }
    */
    close(socket_recv);
    close(socket_send);
    
    return (EXIT_SUCCESS);
}