#ifndef FT_TRACEROUTE_H
 #define FT_TRACEROUTE_H

 #include <unistd.h>
 #include <stdarg.h>
 #include <stdio.h>
 #include "ft_traceroute_structs.h"

 //**** prints.c ****//
 void       print_help();
 void       print_version();
 void       error_exit(int status, int errnum, const char *fmt, ...);
 //void       print_summary(t_traceroute_options *opts);
 void       print_infof(int debug, FILE *stream, const char *fmt, ...);

 //**** parse_args.c ****//
 void       parse_args(int argc, char *argv[], t_traceroute_options *opts);
 int        resolve_target(t_traceroute_options *opts);

 //**** main.c ****//
 void       sigint_handler(int signum);

 //**** packets.c *****//
 //int        send_packet(int sockfd, const t_traceroute_options *opts, uint16_t seq);
 //int        receive_packet(int sockfd, uint16_t sent_seq, const t_traceroute_options *opts);
 uint64_t   ft_time_now_us(void);
 uint16_t   calc_checksum(const void *data, size_t len);
 //int        extract_ttl(struct msghdr *msg);
  
 #endif