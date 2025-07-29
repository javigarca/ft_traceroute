
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "ft_traceroute.h"
#include "ft_traceroute_definitions.h"
#include "ft_traceroute_structs.h"

/**
 * @brief funcion para emnviar un paquete de echo a través del socket
 * 
 * @param sockfd fd del socket
 * @param opts estructura de traceroute 
 * @param seq modificador de puerto base 
 * @return int valor devuelto, 0 todo bien, -1 fallo de envío del paquete(sistema)
 */
int send_packet(int sockfd, t_traceroute_options *opts, uint16_t seq){
    struct sockaddr_in  destination;
    uint64_t            now;
	ssize_t             sent_bytes;
    char                buffer[PAYLOAD_SIZE];
    
    destination = opts->target.addr;
    destination.sin_port = htons(BASE_DST_PORT + seq);
	
	// Timestamp en payload
	now = ft_time_now_us();    
    memcpy(buffer, &now, sizeof(now));
    // Rellenar resto del payload
    memset(buffer + sizeof(now), 0x42, PAYLOAD_SIZE - sizeof(now));

	//  Enviar
	sent_bytes = sendto(sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&destination, sizeof(destination));

	if (sent_bytes < 0) {
		print_infofn(opts->debug, stderr, "sendto (port %u) failed: %s\n", BASE_DST_PORT + seq, strerror(errno));
		return -1;
	}
	return (0);
}

/**
 * @brief Funcíon para validar paquete de respuesta 
 * 
 * @param sockfd fd del socket  
 * @param sent_seq número de sequencia
 * @param opts objeto de traceroute
 * @return int devuelve mayor de cero si todo bien, uno para salto intermedio, dos si ha llegado a destino y hay que finalizar
 */
int receive_packet(int sockfd, uint16_t sent_seq, t_traceroute_options *opts){

    char recv_buf[512];
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);

    ssize_t recv_bytes = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&src_addr, &addrlen);
    if (recv_bytes < 0) {
        error_exit(EXIT_FAILURE, errno, "recvfrom");
    }
    inet_ntop(AF_INET, &src_addr.sin_addr, opts->hop.ip_str, sizeof(opts->hop.ip_str));

    // Extraer cabeceras IP e ICMP
    struct iphdr *ip = (struct iphdr *)recv_buf;
    struct icmphdr *icmp = (struct icmphdr *)(recv_buf + (ip->ihl * 4));
    // Comprobqción pertenencia del paquete
    uint8_t *inner = (uint8_t *)recv_buf + (ip->ihl * 4) + sizeof(*icmp);
    struct iphdr *iphdr_inner = (struct iphdr *) inner;
    size_t iphdr_inner_len = iphdr_inner->ihl *4;
    struct udphdr *udp_hdr = (struct udphdr *) (inner + iphdr_inner_len);
    
    if (ntohs(udp_hdr->uh_dport) != BASE_DST_PORT + sent_seq){
        return 0;
    }
    if ((icmp->type == ICMP_TIME_EXCEEDED) && (icmp->code == ICMP_EXC_TTL)) {
        return 1;
    }
    if ((icmp->type == ICMP_DEST_UNREACH) && (icmp->code == ICMP_PORT_UNREACH)) {
        return 2;
    }
    return (0);
}

/**
 * @brief Timestamp del momento actual en microsegundos, 64bits
 * 
 * @return uint64_t 
 */
uint64_t    ft_time_now_us(void)
{
	struct timeval	time;
	gettimeofday(&time, NULL);
	return ((uint64_t)time.tv_sec * 1000000ULL) + time.tv_usec;
}