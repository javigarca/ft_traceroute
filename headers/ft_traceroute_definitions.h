#ifndef FT_TRACEROUTE_DEFINITIONS_H
 #define FT_TRACEROUTE_DEFINITIONS_H

 // ICMP Types
 #define ICMP_ECHO_REPLY    0   // Tipo para respuesta (Echo Reply)
 #define ICMP_ECHO          8   // Tipo para solicitud (Echo Request)

 // ICMP Codes
 #define ICMP_CODE_DEFAULT  0 // Para echo request/reply siempre es 0

 // ICMP tamaños
 #define ICMP_HEADER_LEN    8
 #define IP_HEADER_LEN      20
 #define PAYLOAD_SIZE       56       
 #define ICMP_PACKET_LEN    (ICMP_HEADER_LEN + PAYLOAD_SIZE)
 #define WIRE_BYTES         (IP_HEADER_LEN + ICMP_HEADER_LEN + PAYLOAD_SIZE)  // 84

 // TRACEROUTE configuration
 #define MAX_HOPS       30
 #define NUM_PROBES     3

#endif