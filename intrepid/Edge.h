
#ifndef EDGE_H_
#define EDGE_H_

#include <iostream>
#include <vector>
#include <string>
#include <qcolor.h>
#include <float.h>

using namespace std;

class Edge
{
public:
    static void sizeEdge(int n1, int n2, int &Ltype, int &mult, float load);

    string id;           /*  Edge ID                  */
    int    edgenum;      /*  Edge number              */
    vector<int> ends;    /*  Endpoints                */
    vector<int> arcs;    /*  Index to the arcs      */
    int    dist;         /*  Length of the edge in km if LL or miles if VH */

    int    cost;         /*  Edge cost                   */
    int    type;         /*  Edge type                   */
    int    mult;         /*  Edge multiplicity           */
    int    length;       /*  Edge length                 */
    int    hops;         /*  Hops in MENTOR tree         */
    int    cap;          /*  Capacity (bps)              */
    bool   Ltree;        // Tree link?
    int    load;         // load in bps, used only by AKER TNDA Mentor code

    QColor Lcolor;

    Edge();
    ~Edge();
    void setEdge(int id, int node, int node2);
    void edge_delete();
};

#endif // EDGE
