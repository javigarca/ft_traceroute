#ifndef FT_TRACEROUTE_STRUCTS_BONUS_H
 #define FT_TRACEROUTE_STRUCTS_BONUS_H

# include <bits/types/struct_timeval.h>
# include <stdint.h>
# include <netinet/in.h>   // struct sockaddr_in
# include <arpa/inet.h>    // INET_ADDRSTRLEN
# include <sys/time.h>
# include <stdbool.h>


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
  * @brief estructura para las flags, información del socket y del host, tiempos
  * 
  */
 typedef struct s_traceroute_options {
	int             debug;
    int             port;
    size_t          packet_len;
    int             probes;
    int             m_ttl;
    bool            dns;          
    t_target		target;
    t_target        hop;
	struct timeval	start_traceroute; 
} t_traceroute_options;

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
 * @brief Pseudo cabecera para el calculo de checksum de UDP
 *    El checksum UDP no sólo cubre la cabecera y los datos del propio UDP, sino también algunos campos clave de la cabecera IP para detectar errores de enrutamiento o entrega incorrecta. Como esos últimos no están físicamente dentro de la cabecera UDP, se crea la pseudo-cabecera:
 *      saddr	    32 bits	Dirección IPv4 de origen
 *      daddr	    32 bits	Dirección IPv4 de destino
 *      zero	    8 bits	Un byte a cero
 *      protocol	8 bits	El valor IPPROTO_UDP (17)
 *      udp_length  16 bits	Longitud total del UDP (sizeof(udphdr) + payload_len)
 *    Al incluir esta pseudo-cabecera en el cálculo:
 *    Garantizas que el paquete ha llegado al par IP correcto (si alguien cambiase la IP de destino, el checksum fallaría).
 *    Detectar errores en la longitud (si se pierde o duplica algún byte en la capa IP, el checksum cambia).
 *    Proteger contra reenvíos o entregas mal dirigidas que el simple checksum UDP (sólo sobre su cabecera y datos) no cubriría.
 *    Este mecanismo está definido en la especificación UDP (RFC 768) y en las recomendaciones de checksum de la capa IP (RFC 1071), y es obligatorio para que la verificación de integridad UDP funcione correctamente en todos los routers y sistemas finales.
 */
 typedef struct s_pseudo_hdr{ 
    uint32_t saddr;
    uint32_t daddr;
    uint8_t  zero;
    uint8_t  proto;
    uint16_t udplen;
}t_pseudo_hdr;

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
/*
#include <netinet/udp.h>

struct udphdr {
    uint16_t uh_sport;  // puerto origen      
    uint16_t uh_dport;  // puerto destino     
    uint16_t uh_ulen;   // longitud UDP       
    uint16_t uh_sum;    // checksum UDP       
};

en Linux o en la misma libreria como union, tambíne valen estos nombres

struct udphdr {
    __be16 source;   // puerto origen 
    __be16 dest;     // puerto destino
    __be16 len;      // longitud UDP   
    __sum16 check;   // checksum UDP   
};



struct sockaddr_in {
    sa_family_t    sin_family; // Tipo de dirección (siempre AF_INET para IPv4)
    in_port_t      sin_port;   // Puerto (en **orden de red**)
    struct in_addr sin_addr;   // Dirección IP
    char           sin_zero[8];// Relleno para que coincida con struct sockaddr
};
*/