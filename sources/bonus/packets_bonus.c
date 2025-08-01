
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
#include "ft_traceroute_bonus.h"
#include "ft_traceroute_definitions_bonus.h"
#include "ft_traceroute_structs_bonus.h"

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
    unsigned short      send_to_port = opts->port + seq;

    if (send_to_port == 0) {
        send_to_port = 1;
    }
    
    destination = opts->target.addr;
    destination.sin_port = htons(send_to_port);
	
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
   //int send_packet(int sockfd, t_traceroute_options *opts, uint16_t seq, int ttl){
    /*
   size_t              selected_len = opts->packet_len_use ? opts->packet_len : WIRE_BYTES;
    size_t              ip_len    = sizeof(struct iphdr);   // 20
    size_t              udp_len   = sizeof(struct udphdr);  // 8
    size_t              payload_len = selected_len > ip_len + udp_len ? selected_len - (ip_len + udp_len) : 0;
    size_t              total_len = ip_len + udp_len + payload_len;
    unsigned short      send_to_port = opts->port + seq;
    uint16_t            src_port = (uint16_t)(getpid() & 0xFFFF);
    uint64_t            now;
    ssize_t             sent_bytes;
    struct sockaddr_in  send_to;
    struct sockaddr_in  local;
    socklen_t           local_len = sizeof(local);

    if (getsockname(sockfd, (struct sockaddr *) &local, &local_len) < 0) {
        error_exit(EXIT_FAILURE, errno, "getsockname: ");
    }
    
    uint8_t *packet = malloc(ip_len + udp_len + payload_len);
    if (!packet) {
        print_infof(opts->debug, stderr, "malloc failed: %s", strerror(errno));
        return (-1);
    }
    memset(packet, 0, total_len);
    struct iphdr  *ip  = (struct iphdr *) packet;
    struct udphdr *udp = (struct udphdr *)(packet + ip_len);
    uint8_t       *data= packet + ip_len + udp_len; /////siezof?

    // – IP header
    ip->version  = 4;
    ip->ihl      = ip_len/4;
    ip->tot_len  = htons(total_len);
    ip->ttl      = ttl;
    ip->protocol = IPPROTO_UDP;
    ip->saddr    = local.sin_addr.s_addr;
    ip->daddr    = opts->target.addr.sin_addr.s_addr;
    ip->check    = 0;
    ip->check    = calc_checksum(ip, ip_len);

    // – UDP header + payload
    udp->source = htons(src_port);
    udp->dest   = htons(send_to_port);
    udp->len    = htons(udp_len + payload_len);
    udp->check  = 0;
    now = ft_time_now_us();
    memcpy(data, &now,sizeof(now));
    if (payload_len > sizeof(now)) {
        memset(data + sizeof(now), 0x42, payload_len - sizeof(now));
    } 

	//  Pseudo cabecera para  Calcular checksum
    t_pseudo_hdr phdr = {ip->saddr,ip->daddr,0,IPPROTO_UDP,htons(udp->len)};

    size_t csum_len = sizeof(phdr) + ntohs(udp->len);
    uint8_t *csum_buf = malloc(csum_len);
    if (!csum_buf){
        print_infof(opts->debug, stderr, "checksum malloc failed: %s", strerror(errno));
        return (-1);
    } 
    // pseudo-cabecera y cabecera udp al buffer
    memcpy(csum_buf, &phdr, sizeof(phdr));
    memcpy(csum_buf + sizeof(phdr), udp, ntohs(udp->len));

    udp->check = calc_checksum(csum_buf, csum_len);
    free(csum_buf);

	//  Enviar
    send_to.sin_family = AF_INET;
    send_to.sin_addr   = opts->target.addr.sin_addr;
    send_to.sin_port   = udp->dest;

	sent_bytes = sendto(sockfd, packet, total_len, 0, (struct sockaddr *) &send_to, sizeof(send_to));

	if (sent_bytes < 0) {
		print_infofn(opts->debug, stderr, "sendto (port %u) failed: %s\n", send_to_port, strerror(errno));
		return -1;
	}
    print_infofn(opts->debug, stderr, "PUERTO enviado: %u", send_to_port);
    free(packet);
	return (0);
   
}
     */

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

    unsigned short sent_to_port = opts->port + sent_seq;
    if (sent_to_port == 0) {
        sent_to_port = 1;
    }

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
    
    print_infofn(opts->debug, stderr, "PUERTO recibido: %u - esperado: %u ", sent_to_port, ntohs(udp_hdr->uh_dport));
    
    if (ntohs(udp_hdr->uh_dport) != sent_to_port){
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
uint64_t    ft_time_now_us(void) {
	struct timeval	time;
	gettimeofday(&time, NULL);
	return ((uint64_t)time.tv_sec * 1000000ULL) + time.tv_usec;
}

/**
 * @brief Calcula el checksum de ICMP seǵun the Internet checksum (RFC 1071).
 *
 * Usando el header y el payload, usando datos como palabras de 16 bits. 
 * uint8_t *bytes = ...	Convertir datos a array de bytes
 * uint32_t sum = 0;	Inicializar acumulador de 16-bit words
 * Bucle for	Sumar todas las palabras de 16 bits
 * if (len % 2)	Añadir último byte si el número es impar
 * while (sum >> 16)	Añadir acarreo si la suma sobrepasa 16 bits
 * ~sum & 0xFFFF	Complemento a uno → checksum final
 * uint8_t *bytes	--  8 bits	Acceso byte a byte seguro y portable
 * uint16_t word	--  16 bits	Para representar cada bloque de 2 bytes como palabra
 * uint32_t sum	    --  32 bits	Para acumular sin perder acarreo
 * Resultado final	--  16 bits	El valor ICMP checksum es siempre uint16_t
 *
 * @param data Puntero a los datos (por ejemplo t_packet)
 * @param len tamaño en bytes
 * @return uint16_t el checksum final, listo para incluir en header, htons(in host byte order)
 */
uint16_t calc_checksum(const void *data, size_t len) {
	const uint8_t *bytes = (const uint8_t *)data;
	uint32_t sum = 0;

	// Sum all 16-bit words
	for (size_t i = 0; i + 1 < len; i += 2)
	{
		uint16_t word = (bytes[i] << 8) + bytes[i + 1];
		sum += word;
	}

	// If odd number of bytes, pad with 0
	if (len % 2 != 0)
	{
		uint16_t last = bytes[len - 1] << 8;
		sum += last;
	}

	// Fold carries
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	// One's complement and return
	return ~sum & 0xFFFF;
}
