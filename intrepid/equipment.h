#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <iostream>
#include <vector>

using namespace std;

class Equipment {
public:
    static int ndev;                   // number of device types
    static vector<Equipment> pEquip;    // vector of device types
    static void addEquip(string id, string function, string mps_cap, string cost, string adapt_cst,
                         string max_links_ls, string max_links_t1, string max_links_t3, string chan_delay);

    string id;            //  Type BOX_MODEL
    string function;      //  Function of the node: GENERIC, MUX, ROUTER
    double mps_cap;       //  Box capacity in messages per second
    double bps_cap;       //  Box capacity in bits per second
    int    cost;          //  Box cost
    int    adapt_cst;     // Adapter cost
    int    max_adapt_ls;  //  Max. # adapters LS
    int    max_adapt_t1;  //  Max. # adapters T1
    int    max_adapt_t3;  //  Max. # adapters T3
    double chan_delay;    //  Fixed channel delay

    Equipment();
    ~Equipment();
    void setEquip(string id, string function, string mps_cap, string cost, string adapt_cst,
                  string max_links_ls, string max_links_t1, string max_links_t3, string chan_delay);
};

#endif // EQUIPMENT_H
