
#ifndef _MY_IP_H_
#define _MY_IP_H_

#include <unistd.h> /* u_int32_t, timeval */
#include <netinet/in.h> /* in_addr */

/* My IP header */
struct my_ip {
         u_int8_t    ip_vhl;  /* header length and version */
#define  IP_V(ip)    (((ip)->ip_vhl & 0xf0)>>4)
#define  IP_HL(ip)   (((ip)->ip_vhl & 0x0f))
         u_int8_t     ip_tos; /* type of service */ 
         u_int16_t    ip_len; /* total lenght */ 
         u_int16_t    ip_id;  /* identification */ 
         u_int16_t    ip_off; /* fragment offset field */ 
#define IF_DF 0x4000          /* dont fragment flag*/  
#define IF_MF 0x2000          /* more fragment flag*/  
#define IF_OFFMASK 0x1fff     /* more fragment flag*/  
        u_int8_t      ip_ttl; /* time to live */
        u_int8_t      ip_p;   /* protocol */
        u_int16_t     ip_sum;   /* checksum */
        struct in_addr ip_src, ip_dest;   /* source and destination address */
};

#endif