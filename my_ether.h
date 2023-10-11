#ifndef _MY_ETHER_H_
#define _MY_ETHER_H_

#include <net/ethernet.h>

/* Ethernet header */
struct my_ethernet {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* source host address */
    u_short ether_type;
};

#endif