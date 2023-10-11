
#ifndef _MY_TCP_H_
#define _MY_TCP_H_

#include <unistd.h> /* u_int32_t, timeval */
// #include <netinet/in.h> /* in_addr */

// TCP Header 
typedef u_int tcp_seq;

struct my_tcp {
    u_int16_t th_sport; /* source port */
    u_int16_t th_dport; /* destination port */
    tcp_seq th_seq; /* sequence number */
    tcp_seq th_ack; /* ack number */
    u_char th_offx2;  /* data offset, rsvd */
#define TH_OFF(th)    (((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
#define TH_FIN 0x01;
#define TH_SYN 0x02;
#define TH_RST 0x04;
#define TH_PUSH 0x08;
#define TH_ACK 0x10;
#define TH_URG 0x20;
#define TH_ECE 0x40;
#define TH_CWR 0x80;
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_int16_t th_win;  /* window */
    u_int16_t th_sum;  /* checksum */
    u_int16_t th_urp;  /* urgent pointer */
};

#endif