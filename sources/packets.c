
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>
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
 * @param opts objeto con las flags
 * @param stats objeto con las estadísticas y demas info pertinente
 * @param seq número de sequencia de envío 
 * @return int valor devuelto, 0 todo bien, -1 fallo de envío del paquete(sistema)
 */
int send_packet(int sockfd, const t_traceroute_options *opts, t_stats *stats, uint16_t seq){
    t_packet    packet;
	uint64_t    now;
	ssize_t     sent_bytes;

	//  Rellenar cabecera
	memset(&packet, 0, sizeof(packet));
	packet.header.type = ICMP_ECHO;
	packet.header.code = ICMP_CODE_DEFAULT;
	packet.header.id = htons((uint16_t)getpid());
	packet.header.sequence = htons(seq);

	// Timestamp en payload
	now = ft_time_now_us();
	memcpy(packet.payload, &now, sizeof(now));

	// Rellenar resto del payload
	for (size_t i = sizeof(now); i < PAYLOAD_SIZE; i++)
		packet.payload[i] = 0x42; // patrón arbitrario

	//  Calcular checksum
	packet.header.checksum = 0;
	packet.header.checksum = htons(calc_checksum(&packet, sizeof(packet)));

	//  Enviar
	sent_bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&stats->target.addr, sizeof(stats->target.addr));

	if (sent_bytes < 0) {
		print_infof(opts->debug, stderr, "sendto failed: %s", strerror(errno));
		return -1;
	}

    stats->transmitted++;
	return (0);
}

/**
 * @brief Funcíon para validar paquete de respuesta a echo, actualización de estadiśticas
 * 
 * @param sockfd fd del socket  
 * @param sent_seq número de sequencia
 * @param opts objeto con las flags
 * @param stats objeto con las estadísticas y demas info pertinente
 * @return int devuelve uno si todo bien. esto es por claridad pues en main esta funcion hace salir del bucle si todo bien
 */
int receive_packet(int sockfd, uint16_t sent_seq, const t_traceroute_options *opts, t_stats *stats){

    char recv_buf[1024];
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);

    ssize_t len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&src_addr, &addrlen);
    if (len < 0) {
        error_exit(EXIT_FAILURE, errno, "recvfrom");
    }

    // Extraer cabeceras IP e ICMP
    struct iphdr *ip = (struct iphdr *)recv_buf;
    struct icmphdr *icmp = (struct icmphdr *)(recv_buf + (ip->ihl * 4));

    //comprobación checksum
    uint16_t received_checksum = icmp->checksum;
    icmp->checksum = 0;
    uint16_t icmp_len = len - (ip->ihl * 4);
    uint16_t calculated_checksum = htons(calc_checksum(icmp, icmp_len));
    if (received_checksum != calculated_checksum) {
        print_infof(opts->debug, stderr, "Invalid ICMP checksum: expected 0x%04x, got 0x%04x\n", calculated_checksum, received_checksum);
        return (0);
    }

    // TTL directamente desde la cabecera IP
    int ttl = ip->ttl;

    // Dirección IP origen
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &src_addr.sin_addr, ip_str, sizeof(ip_str));

    // Verificar tipo y código ICMP
    if (icmp->type != ICMP_ECHOREPLY || icmp->code != 0)
        return (0);

    // Verificar que el paquete es nuestro
    uint16_t expected_id = (uint16_t)getpid();
    if (ntohs(icmp->un.echo.id) != expected_id || ntohs(icmp->un.echo.sequence) != sent_seq)
        return (0);

    // Calcular RTT
    uint64_t now = ft_time_now_us();
    uint64_t sent;
    memcpy(&sent, (void *)icmp + sizeof(struct icmphdr), sizeof(sent));
    double rtt = (now - sent) / 1000.0;

    // Imprimir línea
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n", icmp_len, ip_str, ntohs(icmp->un.echo.sequence), ttl, rtt);

    // Actualizar estadísticas
    stats->received++;
    stats->rtt_total += rtt;
    stats->rtt_squared_total += rtt * rtt;
    if (rtt < stats->rtt_min || stats->rtt_min == 0)
        stats->rtt_min = rtt;
    if (rtt > stats->rtt_max)
        stats->rtt_max = rtt;

    return (1);
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
uint16_t calc_checksum(const void *data, size_t len)
{
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