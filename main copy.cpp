#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h> 
#include <time.h>
#include <ctime>

using namespace std;

#define TIME_BIN_INTERVAL 60*60*24
    
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
    int t_tx;   // Total amount send
    int t_time;
    int tx_count;
    // Statistical info
    vector<int> l_tx;
    vector<int> l_freq;
    float stat_mean;
    float stat_std;
    float stat_freq;
    float stat_freg_std;
};

void init_traceprofile(<traceprofile> *p) {

    int dtime = rand() % TIME_BIN_INTERVAL;
    //allow enough time to allow for data to be sent
    // on ave what is the max(dtime)
    int dtime = dtime - (p->freq_mean + p->freq_std) * p->data_count;
    if (dtime < 0 ) {
        p->ts_start = 0;
        p->dtime = 1;
    } else {
        p->ts_start = 0;
        p->dtime = dtime;
    }
    p->t_time = 0;
    p->t_tx = 0;
    p->tx_count = 0;
    }
}

void print_profile_entry(profile_entry entry) {
        cout << "shost " <<  entry.shost << endl;
        cout << "bucket " << entry.bucket << endl;
        cout << "data_count " << entry.data_count << endl;
        cout << "data_sum " << entry.data_sum << endl;
        cout << "data_mean " << entry.data_mean << endl;
        cout << "data_std " << entry.data_std << endl;
        cout << "freq " << entry.freq_mean << endl;
        cout << "freq_std " <<entry.freq_std << endl;
        cout << "t_tx " << entry.t_tx << endl;
        cout << "t_time " << entry.t_time << endl;
        cout << "dtime " << entry.dtime << endl;
        cout << "tx_count " << entry. tx_count << endl;
        cout << "stat_mean " << entry.stat_mean << endl;
        cout << "stat_std " << entry.stat_std << endl;
}

void loadProfile(string filename,  vector<profile_entry> &data) {

    ifstream  f(filename);
    if (! f.is_open()) {
        cout << "File failed to open." << endl;
    }

    profile_entry tmp_entry;
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

            data.push_back(tmp_entry);
        } catch(...) {

        }
    }
}

int process_entry(profile_entry* entry) {
    
    int packetsize_var;
    int packetsize;
    int freq;

    if (entry->t_tx >= entry->data_sum  ) {
        entry->dtime = 0;
        return 0;
    }
    if (int(entry->data_std) != 0 ) {
        packetsize_var = rand() % (int(entry->data_std)*2) - int(entry->data_std);
        packetsize = entry->data_mean + packetsize_var;
        if (packetsize < 0) 
            packetsize = 0;
    } else {
            packetsize = entry->data_mean;
    }

    if (int(entry->freq_std) != 0) {
        int freq_var = rand() % (int(entry->freq_std)*2) - int(entry->freq_std);
        freq = entry->freq_mean + freq_var;;
        if (freq < 0) 
            freq = 0;
    } else {
        freq = 0;
    }
    //print_profile_entry(*entry);
    cout << "Sending datato " << entry->shost << " : size " << packetsize << " dtime " << freq << endl;
    entry->t_tx += packetsize;
    entry->dtime += freq;
    entry->tx_count++;
    // Stats
    entry->l_tx.push_back(packetsize);
    entry->l_freq.push_back(freq);
    return 1;
}

void calcStatistics(profile_entry* entry) {
    if (entry->tx_count > 0)
        entry->stat_mean = entry->t_tx/entry->tx_count;
        float sum =0;
        for ( int i = 0; i < entry->l_tx.size(); i++) {
            sum += pow((entry->l_tx.at(i)-entry->stat_mean),2);
        entry->stat_std = sqrt(sum/entry->tx_count);


    }
    // entry->stat_freq = entry->
}


int main() {

    vector<profile_entry> lprofile;
    struct timeval ts_start;
    struct timeval ts_end;


    loadProfiles("./data/profile_b.csv", lprofile);

    for ( int i = 0; i < l->size(); i++){
        initProfile(&lprofile);
    }
    srand((unsigned) time(NULL));


    // Main loop
    int min_dtime = -1;
    gettimeofday(&ts, NULL); // return value can be ignored 
    while (true) {
        int done = 1;
        for ( int i = 0; i < data.size(); i++) {
            if (process_entry(&data.at(i))) {
                if ( min_dtime == -1 ) {
                    min_dtime = data.at(i).dtime;
                } else {
                    if (data.at(i).dtime < min_dtime) {
                        min_dtime = data.at(i).dtime;
                    }
                }
                //cout << "sleep for " << min_dtime << endl;
                min_dtime = -1;
                done = 0;
            }
            // test for one host
            break;
        }
        if (done) 
            break;
    }

    // for ( int i = 0; i < data.size(); i++){
    //     calcStatistics(&data.at(i));
    //     print_profile_entry(data.at(i));
    // }

    calcStatistics(&data.at(0));
    print_profile_entry(data.at(0));
}

