#ifndef LINETYPE_H
#define LINETYPE_H

#include <iostream>
#include <vector>

using namespace std;

class Linetype {
public:
    static int ncap;                   // number of line capacities
    static vector<Linetype> pLinetype; // vector of line types
    static void addLinetype(string svty, string speedin, string duplexity, string type,
        string fixed_cost, string dist_cost1, string dist_cost2, string dist1, string rel,
        string channels);

    string svty;
    float  speedin;
    char   duplexity;
    string type;
    float  fixed_cost;
    float  dist_cost1;
    float  dist_cost2;
    float  dist1;
    float  rel;
    int    channels;

    Linetype();
    ~Linetype();
    void   setLinetype(string svty, string speedin, string duplexity, string type, string fixed_cost,
        string dist_cost1, string dist_cost2, string dist1, string rel, string channels);
};

#endif // LINETYPE_H
