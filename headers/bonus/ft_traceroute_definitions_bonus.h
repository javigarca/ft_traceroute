#ifndef FT_TRACEROUTE_DEFINITIONS_BONUS_H
 #define FT_TRACEROUTE_DEFINITIONS_BONUS_H

 // ICMP Types
 #define ICMP_ECHO_REPLY    0   
 #define ICMP_ECHO          8   
 #define ICMP_TIME_EXCEEDED 11
 #define ICMP_DEST_UNREACH  3

 // ICMP Codes
 #define ICMP_EXC_TTL       0 //(11)
 #define ICMP_PORT_UNREACH  3 //(3)

 // ICMP Codes
 #define ICMP_CODE_DEFAULT  0 // Para echo request/reply siempre es 0

 // ICMP tamaños del UDP
 #define ICMP_HEADER_LEN    8
 #define IP_HEADER_LEN      20
 #define PAYLOAD_SIZE       32       
 #define ICMP_PACKET_LEN    (ICMP_HEADER_LEN + PAYLOAD_SIZE)
 #define WIRE_BYTES         (IP_HEADER_LEN + ICMP_HEADER_LEN + PAYLOAD_SIZE)  // 60

 // TRACEROUTE configuration
 #define NUM_TTL       30
 #define NUM_PROBES     3
 #define DEF_TIME_OUT_S 5
 #define DEF_TIME_OUT_U 0
 #define UDP_HDR_LEN    8          // tamaño de la cabecera UDP, añadida por el SO
 #define BASE_DST_PORT  33434
 #define MAX_NUM_PORT   65535

#endif