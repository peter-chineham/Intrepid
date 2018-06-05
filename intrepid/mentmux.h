
#ifndef MENTMUX_H_
#define MENTMUX_H_

#include <iostream>
#include <vector>

using namespace std;

class Mentmux {
public:
    static long  wmax;
    static long  wmin;
    static int   dcmax;
    static int   distmax;
    static vector<vector<int> > linkmat; // vector of 2D arcs
    static vector<vector<int> > hops;    // vector of 2D hops
    static vector<vector<int> > seq;     // sequence vector

    static bool   size_hops(int nn);
    static bool   size_linkmat(int nn);
    static void   mentorI(string alg);
    static bool   initialize_net();
    static bool   set_bkbn();
    static bool   thresh_set_bkbn();
    static int    set_med();
    static bool   primd(int root);
    static bool   build_tree();
    static void   find_hops(int root);
    static bool   compressMUX();
    static int    sequenceMUX();
    static int    select_node_model();
    static double total_utilization(int node, int devtype);
    static void   resize_links();
    static int    total_link_cost();
};

#endif /* MENTMUX_H_ */
