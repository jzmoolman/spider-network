#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h> 
#include <sys/time.h>

#include "my_ether.h"
#include "my_ip.h"
#include "my_tcp.h"

// Network stuff
#include <pcap.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <netinet/ip6.h>

using namespace std;

#define MAXHOSTSZ 256


#define TIME_BIN_INTERVAL 60*60*24
#define TIME_SPEEDUP 100 



void gennoise(char* buffer, int size) {
    for (int i = 0; i < size; i++) {
        *(buffer+i) = rand() % 255;
    }
}

void sendpacket(pcap_t *p, char* buffer, int size ) {

    char errbuf[PCAP_ERRBUF_SIZE]; 
    int snaplen  = 4095;
    int promisc = 1;
    int to_ms = 1000;
    if (pcap_inject(p, buffer, size) == -1) {
        cout << "error: sending packet lenght " << size << endl;
    }
}

void createpacket( pcap_t *p, const char* src, const char* dest, int size) {
    
    u_int32_t buffersize = sizeof(struct my_ethernet) + sizeof(struct my_ip) + sizeof(struct my_tcp) + size;
    char *buffer = (char*)malloc(buffersize);

    //Ethernet
    struct my_ethernet* ep;
    ep = (struct my_ethernet*)buffer;
    cout << src << endl;
    cout << dest << endl;
    ether_addr *s = ether_aton("d4:21:c8:fe:a6:7a");
    ether_addr *d = ether_aton("ff:ff:ff:ff:ff:ff");
    memcpy(&ep->ether_shost,s,sizeof(struct ether_addr));
    memcpy(&ep->ether_dhost,d,sizeof(struct ether_addr));
    ep->ether_type = htons(ETHERTYPE_IP);

    //ip
    struct my_ip* ip;
    ip = (struct my_ip*)(buffer + sizeof(my_ethernet));

    ip->ip_len = htons(sizeof(my_ip) + sizeof(my_tcp) + size);
    ip->ip_vhl = 0;
    ip->ip_vhl = ip->ip_vhl | 5;  //hlen
    ip->ip_vhl = ip->ip_vhl | 4 << 4; //version 
    ip->ip_off = 0;  // send as first fragment

    inet_aton(src, &ip->ip_src);
    inet_aton(dest, &ip->ip_dest);

    ip->ip_id = 1234;
    ip->ip_p = 0;
    ip->ip_tos = 0;
    ip->ip_ttl = 1;

    //tcp
    struct my_tcp* tp;
    tp = (struct my_tcp*) (buffer + sizeof(my_ethernet) + sizeof(my_ip));
    tp->th_offx2 = 0;
    tp->th_offx2 = tp->th_offx2 | sizeof(struct my_tcp)/4 << 4;
    tp->th_dport = htons(433);
    tp->th_ack = 0;
    tp->th_flags = 0;
    tp->th_seq = 0;
    tp->th_sport = htons(5000+rand() % 1000);
    tp->th_sum = htons(rand() % 1000);
    tp->th_urp = htons(rand() % 1000);
    tp->th_win = htons(rand() % 1000);

    //Data
    char* data = buffer + (sizeof(my_ethernet) + sizeof(my_ip) + sizeof(my_tcp));
    gennoise(data, size);

    sendpacket(p, buffer, buffersize);


    free(buffer);
}

    
struct traceprofile {
    // imported data
    string dummy;
    string shost;
    int bucket;
    int data_count;
    int data_sum;
    float data_mean;
    float data_std;
    float freq_mean;
    float freq_std;
    // meta data
    int ts_start;
    int dtime;   // next_send time
    int tx_data;     // Total amount send
    int tx_count;
    // Statistical info
    vector<int> l_tx;
    vector<int> l_freq;
    float stat_mean;
    float stat_std;
    float stat_freq;
    float stat_freg_std;
};

void init_traceprofile(traceprofile *p) {

    int dtime = rand() % TIME_BIN_INTERVAL;
    //allow enough time to allow for data to be sent
    //on ave what is the max(dtime)
    dtime = dtime - (p->freq_mean + p->freq_std) * p->data_count;
    if (dtime < 0 ) {
        p->ts_start = 0;
        p->dtime = 1;
    } else {
        p->ts_start = 0;
        p->dtime = dtime;
    }
    p->dtime = 0;
    p->tx_data = 0;
    p->tx_count = 0;
}

void print_profile_entry(traceprofile entry) {
        cout << "shost " <<  entry.shost << endl;
        cout << "bucket " << entry.bucket << endl;
        cout << "data_count " << entry.data_count << endl;
        cout << "data_sum " << entry.data_sum << endl;
        cout << "data_mean " << entry.data_mean << endl;
        cout << "data_std " << entry.data_std << endl;
        cout << "freq " << entry.freq_mean << endl;
        cout << "freq_std " <<entry.freq_std << endl;
        cout << "ts_start " << entry.ts_start << endl;
        cout << "dtime " << entry.dtime << endl;
        cout << "tx_data " << entry.tx_data << endl;
        cout << "tx_count " << entry. tx_count << endl;
        cout << "stat_mean " << entry.stat_mean << endl;
        cout << "stat_std " << entry.stat_std << endl;
}

void loadProfiles(string filename,  vector<traceprofile> &l) {

    ifstream  f(filename);
    if (! f.is_open()) {
        cout << "File failed to open." << endl;
    }

    traceprofile tmp_entry;
    string tmp_str;
    while (getline(f, tmp_entry.dummy, ',')) {
        try  {
            getline(f, tmp_entry.shost, ',');
            
            getline(f, tmp_str, ',');
            tmp_entry.bucket = stoi(tmp_str);

            getline(f, tmp_str, ',');
            tmp_entry.data_count = stoi(tmp_str);

            getline(f, tmp_str, ',');
            tmp_entry.data_sum = stoi(tmp_str);

            getline(f, tmp_str, ',');
            tmp_entry.data_mean = stof(tmp_str);

            getline(f, tmp_str, ',');
            tmp_entry.data_std = stof(tmp_str);

            getline(f, tmp_str, ',');
            tmp_entry.freq_mean = stof(tmp_str);

            getline(f, tmp_str);
            tmp_entry.freq_std = stof(tmp_str);

            l.push_back(tmp_entry);
        } catch(...) {

        }
    }
}

int process_entry( pcap_t *p, char* host, traceprofile* profile, timeval ts_start) {
    
    int packetsize_var;
    int packetsize;
    int freq;

    if (profile->tx_data >= profile->data_sum  ) {
        profile->dtime = -1;
        return 0;
    }

    // Check if if enoung time has passed
    timeval ts_current;  
    gettimeofday(&ts_current, NULL); 
    if (ts_current.tv_sec - ts_start.tv_sec  < profile->dtime) {
        return 1; 
    }


    if (int(profile->data_std) != 0 ) {
        packetsize_var = rand() % (int(profile->data_std)*4) - int(profile->data_std)*2;
        packetsize = profile->data_mean + packetsize_var;
        if (packetsize < 0) 
            packetsize = 0;
    } else {
        packetsize = profile->data_mean;
    }

    if (int(profile->freq_std) != 0) {
        int freq_var = rand() % (int(profile->freq_std)*2) - int(profile->freq_std);
        freq = profile->freq_mean + freq_var;;
        if (freq < 0) 
            freq = 0;
    } else {
        freq = 0;
    }
    
    cout << "Sending data to " << profile->shost << " size " << packetsize << " freq " << freq << endl;
    //

    createpacket(p, host ,profile->shost.c_str(), packetsize);

    //
    profile->tx_data += packetsize;
    profile->tx_count++;
    profile->dtime += freq/TIME_SPEEDUP; // next packet
    // Stats
    profile->l_tx.push_back(packetsize);
    profile->l_freq.push_back(freq);
    return 1;
}

void calcStatistics(traceprofile* p) {
    if (p->tx_count > 0)
        p->stat_mean = p->tx_data/p->tx_count;
        float sum =0;
        for ( int i = 0; i < p->l_tx.size(); i++) {
            sum += pow((p->l_tx.at(i)-p->stat_mean),2);
        p->stat_std = sqrt(sum/p->tx_count);
    }
}


int main() {
    // Network stuff
    char *ifname;                  /* interface name (such as "en0") */
    char errbuf[PCAP_ERRBUF_SIZE]; /* buffer to hold error text */
    char lhost[MAXHOSTSZ];         /* local host name */
    pcap_t *p; /* packet capture descriptor */ 
    int optimize = 1;              /* passed to pcap_compile to do optimization */
    int snaplen = 1048;            /* amount of data per packet */
    int promisc = 0;               /* do not change mode; if in promiscuous */
    int to_ms = 1000;              /* timeout, in milliseconds */

    bpf_u_int32 netp = 0;          /* network IP address */
    bpf_u_int32 maskp = 0;         /* network address mask */

    char netstr[INET_ADDRSTRLEN];  /* dotted decimal form of address */
    char maskstr[INET_ADDRSTRLEN]; /* dotted decimal form of net mask */

    //Personnas
    vector<traceprofile> lprofiles;
    struct timeval ts_start;
    struct timeval ts_end;

    //Network init

    /*
     * Find a network device on the system.
     */

    if (!(ifname = pcap_lookupdev(errbuf)))
    {
        fprintf(stderr, "Error getting device on system: %s\n", errbuf);
        exit(1);
    }
    cout << ifname << endl;

     /*
     * Open the network device for packet capture. This must be called
     * before any packets can be captured on the network device.
     */
    if (!(p = pcap_open_live(ifname, snaplen, promisc, to_ms, errbuf)))
    {
        fprintf(stderr,
                "Error opening interface %s: %s\n", ifname, errbuf);
        exit(2);
    }
    printf("pcap_open_live\n");

    int fd;
    struct ifreq ifr;
    struct sockaddr_in *sin4;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&ifr, 0, sizeof(ifr));
    memcpy(ifr.ifr_name, ifname, sizeof(ifname));
    ioctl(fd, SIOCGIFADDR, (char *)&ifr);
    sin4 = (struct sockaddr_in *)&ifr.ifr_addr;
    netp = sin4->sin_addr.s_addr;

    memset(&ifr, 0, sizeof(ifr));
    memcpy(ifr.ifr_name, ifname, sizeof(ifname));
    ioctl(fd, SIOCGIFNETMASK, (char *)&ifr);
    (void)close(fd);

    maskp = sin4->sin_addr.s_addr;

    /*
     * Create the filter and store it in the string called 'fltstr.'
     * Here, you want only incoming packets (destined for this host),
     * which use port 23 (telnet), and originate from a host on the
     * local network.
     */

    /* First, get the hostname of the local system */
    if (gethostname((char *)lhost, sizeof(lhost)) < 0)
    {
        fprintf(stderr, "Error getting hostname.\n");
        exit(4);
    }
    printf("hostname %s\n", lhost);

    /*
     * Second, get the dotted decimal representation of the network address
     * and netmask. These will be used as part of the filter string.
     */
    // inet_ntop(AF_INET, (char*) &net, netstr, sizeof netstr);
    struct in_addr addr;
    addr.s_addr = netp;
    char host[15] = {0};
    memcpy(&host, inet_ntoa(addr), 15);

    inet_ntop(AF_INET, (char *)&maskp, maskstr, sizeof maskstr);
    printf("host %s\n", host);
    printf("netmask %s\n", maskstr);
    


    // Personas

    loadProfiles("./data/profile_b.csv", lprofiles);

    for ( int i = 0; i < lprofiles.size(); i++){
        init_traceprofile(&lprofiles.at(i));
    }
    srand((unsigned) time(NULL));


    // Main loop
    int min_dtime = -1;


    gettimeofday(&ts_start, NULL); // return value can be ignored 
    cout << ts_start.tv_sec << endl;


    while (true) {
        int done = 1;
        for ( int i = 0; i < lprofiles.size(); i++) {
            if (process_entry(p, host,  &lprofiles.at(i), ts_start)) {
                if ( min_dtime == -1 ) {
                    min_dtime = lprofiles.at(i).dtime;
                } else {
                    if (lprofiles.at(i).dtime < min_dtime) {
                        min_dtime = lprofiles.at(i).dtime;
                    }
                }
                //cout << "sleep for " << min_dtime << endl;
                min_dtime = -1;
                done = 0;
            }
            // test for one host
            // break;
        }
        if (done) 
            break;
    }

    // for ( int i = 0; i < data.size(); i++){
    //     calcStatistics(&data.at(i));
    //     print_profile_entry(data.at(i));
    // }

    calcStatistics(&lprofiles.at(0));
    print_profile_entry(lprofiles.at(0));
    
    gettimeofday(&ts_end, NULL); // return value can be ignored 
    cout << ctime(&ts_start.tv_sec);
    cout << ctime(&ts_end.tv_sec);
}

