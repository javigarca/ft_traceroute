#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include "ft_traceroute_bonus.h"
#include "ft_traceroute_definitions_bonus.h"
#include "ft_traceroute_structs_bonus.h"

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
 * recibe los argumentos, inicializa estructuras, parseo de argumentos, creación de sockets, envío de socket y recepción de respuestas. Tratamiento de señales, crtl+C. 
 *
 * @param argc 
 * @param argv 
 * @return int 
 */

int main (int argc, char **argv)
{
    t_traceroute_options    opts = {0};
    struct sigaction        sa = {0};
    int                     socket_send, socket_recv;
    int                     seq = -1;
    
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
    //incluir cabecera IP desde el kernel
   // int ip = 1;
    //if (setsockopt(socket_send, IPPROTO_IP, IP_HDRINCL, &ip, sizeof(ip)) < 0)
    //        error_exit(EXIT_FAILURE, 0, "Error setting IP header: %u", ip);

    socket_recv = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_recv < 0){
        if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)
		    error_exit(EXIT_FAILURE, 0, "ft_traceroute: Lacking privilege for ICMP socket.\n");
        error_exit(EXIT_FAILURE, errno, "socket ICMP" );
    }
    
    if (resolve_target(&opts))
        error_exit(EXIT_FAILURE, 0, "Error resolving host.");

    ////impresión cabecera
    print_infofn(1, stdout, "ft_traceroute to %s (%s) %d hops max %d bytes packets", opts.target.hostname, opts.target.ip_str, opts.m_ttl, opts.packet_len);

    for (int ttl = 1; ttl <= opts.m_ttl && g_interrupted == 0; ttl++){
        char    last_ip[INET_ADDRSTRLEN] = "";
        char    dns_host[NI_MAXHOST];

         if (setsockopt(socket_send, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
            error_exit(EXIT_FAILURE, 0, "Error setting TTL: %u", ttl);
        print_infof(1, stdout, "%u ", ttl);
    
        for (int probe = 0; probe < opts.probes; probe++) {
            struct  timeval t_send, t_recv, time_out;
            double  t_rtt_ms;
            int     response = 0;
            fd_set  rfds;
            seq++;
            if (!send_packet(socket_send, &opts, seq)) {
                gettimeofday(&t_send, NULL);     

                FD_ZERO(&rfds);
                FD_SET(socket_recv, &rfds);
                time_out.tv_sec  = DEF_TIME_OUT_S;
                time_out.tv_usec = DEF_TIME_OUT_U;
                
                int reply = select(socket_recv + 1, &rfds, NULL, NULL, &time_out);
     
                if (reply < 0){
                    if (errno == EINTR) {
                        g_interrupted = 1;
                        break;
                    } else {
                        error_exit(EXIT_FAILURE, errno, "select");
                    }
                }
                if (reply == 0) {
                    print_infof(1, stdout," *");
                    fflush(stdout);    
                    continue;
                }
                //analisis de paquete
                response = receive_packet(socket_recv, seq, &opts); 
                    if (response > 0) {
                        if (strcmp(opts.hop.ip_str,last_ip) != 0){
                            strcpy(last_ip, opts.hop.ip_str);
                            if (!opts.dns) {
                                print_infof(1, stdout," %s (%s) ", last_ip, last_ip);
                            } 
                            else {
                                int dns_res = getnameinfo( (struct sockaddr *) &opts.hop.addr, sizeof(opts.hop.addr), dns_host, sizeof(dns_host), NULL, 0, 0);
                                if (dns_res == 0){
                                    print_infof(1, stdout," %s (%s) ", dns_host, last_ip);
                                } else {
                                    print_infof(1, stdout," %s (%s) ", last_ip, last_ip);
                                    print_infof(opts.debug, stdout," Name Resolution failed : %d \n", dns_res);
                                }
                            }
                        }
                        gettimeofday(&t_recv, NULL);
                        t_rtt_ms = (t_recv.tv_sec  - t_send.tv_sec) * 1000.0 + (t_recv.tv_usec - t_send.tv_usec) / 1000.0;
                        print_infof(1, stdout,"%.3f ms ", t_rtt_ms);
                        if (response == 2 ){
                            g_interrupted = 1;
                        }
                }
            }            
        }
        print_infof(1, stdout,"\n");
    }

    if (opts.debug)
        print_opts(&opts);
    close(socket_recv);
    close(socket_send);
    
    return (EXIT_SUCCESS);
}