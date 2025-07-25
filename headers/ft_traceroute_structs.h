#ifndef FT_TRACEROUTE_STRUCTS_H
 #define FT_TRACEROUTE_STRUCTS_H

#include <bits/types/struct_timeval.h>
# include <stdint.h>
# include <netinet/in.h>   // struct sockaddr_in
# include <arpa/inet.h>    // INET_ADDRSTRLEN
# include <sys/time.h>
# include "ft_traceroute_definitions.h"

 /**
  * @brief estructura para las flags. autoexplicativas
  * 
  */
 typedef struct s_traceroute_options {
	int 		verbose;
	const char	*target;
} t_traceroute_options;

/**
 * @brief Información del host de destino resuelta desde el nombre o IP.
 * Contiene:
 *  - la IP como string para imprimir
 *  - el hostname original (si no era una IP directa)
 *  - la estructura sockaddr_in que se usará en sendto()
 */
typedef struct s_target {
	char				ip_str[INET_ADDRSTRLEN];   // Dirección IP como string (e.g. "8.8.8.8")
	const char			*hostname;                 // Hostname original (si aplica)
	struct sockaddr_in	addr;        // Dirección IPv4 en formato binario
} t_target;

/**
 * @brief Estructura con la info del socket para impresiones
 * 
 */
typedef struct s_socket_info {
	int fd;
	int socktype;         // SOCK_RAW, SOCK_DGRAM, etc.
	int family;           // AF_INET, AF_INET6...
	char *family_str;     // "AF_INET" (dinámico o literal)
	char *socktype_str;   // "SOCK_RAW", etc.
} t_socket_info;

/**
 * @brief Estructura para las estadísticas.
 * 
 * RTT significa Round-Trip Time: el tiempo que tarda un paquete en ir desde tu máquina al host remoto y volver con la respuesta.
 * Este valor se mide en milisegundos (ms).
 * 
 * rtt min/avg/max/mdev
 * mdev, desviación de latencia, cuanto menor más estable es la conexión
 */
typedef struct s_stats {
	int 			transmitted;         	// Nº total de paquetes enviados
	int 			received;            	// Nº total de paquetes recibidos correctamente
	double 			rtt_min;          	// Menor RTT observado (latencia más baja)
	double 			rtt_max;          	// Mayor RTT observado (latencia más alta)
	double 			rtt_total;        	// Suma acumulada de todos los RTT (para calcular la media)
	double 			rtt_squared_total;	// Suma de los RTT^2 (para calcular la desviación estándar)
	t_target		target;
	t_socket_info	socket_i;
	struct timeval	start_traceroute; 
} t_stats;

/**
 * @brief Cabecera ICMP tal y como debe enviarse en un paquete ICMP Echo.
 * Campos:
 *  - type: 8 para Echo Request, 0 para Echo Reply
 *  - code: normalmente 0
 *  - checksum: se calcula sobre toda la cabecera + payload
 *  - id: identificador único por proceso
 *  - sequence: número de secuencia creciente
 */
typedef struct s_icmphdr {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	uint16_t id;
	uint16_t sequence;
} t_icmphdr;

/**
 * @brief Paquete completo ICMP, cabecera seguida de payload.
 * Este buffer es el que se enviará y recibirá con sendto()/recvfrom().
 */
typedef struct s_packet {
	t_icmphdr	header;              // Cabecera ICMP
	uint8_t 		payload[PAYLOAD_SIZE];   // Payload arbitrario (p.ej., timestamp o relleno)
} t_packet;

#endif

/* Estructuras de librerias 
struct iphdr {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        unsigned int ihl:4;
        unsigned int version:4;
    #elif __BYTE_ORDER == __BIG_ENDIAN
        unsigned int version:4;
        unsigned int ihl:4;
    #else
    # error "Please fix <bits/endian.h>"
    #endif
    uint8_t  tos;           // Type of Service
    uint16_t tot_len;       // Total Length (header + data)
    uint16_t id;            // Identification
    uint16_t frag_off;      // Fragment offset and flags
    uint8_t  ttl;           // Time to Live
    uint8_t  protocol;      // Protocol (e.g., TCP=6, UDP=17, ICMP=1)
    uint16_t check;         // Header checksum
    uint32_t saddr;         // Source address
    uint32_t daddr;         // Destination address
    // optional options[] might follow
};

version	4 bits	Versión del protocolo (4 para IPv4).
ihl	4 bits	Longitud del header en palabras de 32 bits (mínimo 5).
tos	uint8_t	Type of Service (prioridad/calidad).
tot_len	uint16_t	Longitud total del paquete (header + payload).
id	uint16_t	ID del paquete (usado en fragmentación).
frag_off	uint16_t	Flags y fragment offset (para reconstrucción).
ttl	uint8_t	Time to live (cuántos routers antes de morir).
protocol	uint8_t	Protocolo de la carga útil: TCP, UDP, ICMP, etc.
check	uint16_t	Checksum del header.
saddr	uint32_t	Dirección IP origen (en big endian).
daddr	uint32_t	Dirección IP destino (también en big endian).

*/

/*
struct icmphdr {
    uint8_t  type;      // Tipo de mensaje (8=echo request, 0=echo reply)
    uint8_t  code;      // Subtipo (normalmente 0 para echo)
    uint16_t checksum;  // Checksum ICMP (cabecera + datos)

    union {
        struct {
            uint16_t id;       // Identificador
            uint16_t sequence; // Número de secuencia
        } echo;                // Usado por echo request/reply

        uint32_t gateway;      // Usado en redirect

        struct {
            uint16_t __unused;
            uint16_t mtu;
        } frag;                // Usado en fragmentation-needed
    } un;
};

type	uint8_t	Tipo de mensaje ICMP.
code	uint8_t	Código específico del tipo (más detalle).
checksum	uint16_t	Checksum para verificar integridad.
un.echo.id	uint16_t	Identificador para solicitudes traceroute.
un.echo.sequence	uint16_t	Número de secuencia, incrementa por traceroute.

*/