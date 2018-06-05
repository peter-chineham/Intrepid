#ifndef ARC_H
#define ARC_H

#include <iostream>
#include <vector>

using namespace std;

class Arc
{
public:
    string id;           /*  Arc ID                    */
    int    arcnum;       /*  Arc number                */
    int    edge;         /*  Associated edge           */
    vector<int> ends;    /*  Source , destination      */

    double  length;      /*  Length (COS) for routing  */
    double  bps;         /*  Flow (bps)                */
    double  utilization; /*  RCAK020294 for FE display */

    Arc();
    ~Arc();
    void setArc(int arc, string prefix, int edge, int node, int node2);
    void arc_delete();
};

#endif // ARC_H
