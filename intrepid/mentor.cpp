/*
 * mentor.cpp
 *
 *  Created on: 15 Apr 2016
 *      Author: vinntec
 */

#include "mentor.h"

#include "canvas.h"
//#include "Edge.h"
#include "linetype.h"
#include "Network.h"
#include "Parm.h"
#include "Requirements.h"
#include "Utils.h"

/////////////////////////////////////////////////////////////////////
//               MENTOR   ALGORITHM   AND   DRIVER                 //
/////////////////////////////////////////////////////////////////////

void   SetWeights(float Weight[]);
int    SetMedian (int Flag , int Bkbn[] , float Weight[] );
void   Thresh(int median, float Weight[], int Bkbn[]);
void   PrimDijk(int median, int Bkbn[]);
void   Sequence(int **spPred, int *Seq, int **Home);
int    MakeIndexPair(int i, int j);
void   SplitIndexPair(int pair, int *i, int *j);
void   SetDist(int Median, float **spDist, int **spPred);
void   Compress(int Seq[], int *Home[], int Bkbn[]);

ofstream trace;

void MENTOR_Driver() {

    /*  INPUTS  */
    float *Weight;         // total traffic to and from each location
    int   Median;          // network median

    /* OUTPUTS  */
    int   *Bkbn;           // 1 if the location is a backbone node; 0 otherwise
    int   **Home;          // associated backbone node

    /* WORKING VARIABLES  */
    float **spDist;        // length of least cost path between each pair
    int   **spPred;        // predecessors in least cost pair
    int   *Seq;            // sequence for considering node pairs
    float NetCost;         // Network cost
	int nn = Network::nodes.size();

    trace.open ("D:\\Programs\\Qt\\TNda-CPP\\trace.txt");

    cout << "=======================" << endl;
    cout << "MENTOR Algorithm Report" << endl;
    cout << "=======================" << endl;
    trace << "=======================" << endl;
    trace << "MENTOR Algorithm Report" << endl;
    trace << "=======================" << endl;
    spDist = Utils::Alloc2Dflo(nn, nn);
    spPred = Utils::Alloc2Dint(nn, nn);
    Home = Utils::Alloc2Dint(nn, nn);
    Weight = new float[nn];
    Bkbn = new int[nn];
    Seq = new int[nn * nn];

    // Set the node weights
    SetWeights(Weight);
    cout << "Weights" << endl;
    trace << "Weights" << endl;
    for (int i = 0; i < nn; i++) {
            trace << Weight[i] << " ";
    }
    trace << endl << endl;
    cout << "SetWt" << endl;
    trace << "SetWt" << endl;

    // Find the network median of all the nodes
    Median = SetMedian(0, Bkbn, Weight);
    cout << "Median1: " << Median << endl;
    trace << "Median1: " << Median << endl;

    // Select backbone nodes
    Thresh(Median , Weight, Bkbn);
    cout << "Thresh: " << endl;;
    trace << "Thresh: " << endl;
    for (int i = 0; i < nn; i++) {
            trace << Bkbn[i] << " ";
    }
    trace << endl;

    // Find the network median, only considering backbone nodes
    Median = SetMedian(1, Bkbn, Weight);
    cout << "Median2: " << Median << endl;
    trace << "Median2: " << Median << endl;

    // Find the spanning tree
    trace << endl << "+++++ MENTOR Tree +++++" << endl;
    PrimDijk(Median, Bkbn);
    cout << "PrimDijk" << endl;
    trace << "PrimDijk" << endl;
    for (int i = 0; i < nn; i++) {
            trace << "Tree pred of " << i << " is " << Network::nodes[i].pred_node << endl;
    }

    // Find the distances and predecessors within the tree
    SetDist(Median, spDist, spPred);
    cout << "SetDist" << endl;
    trace << "SetDist" << endl;
    for (int i = 0; i < nn; i++) {
        for (int j = 0; j < nn; j++) {
                    trace << "spDist[" << i << "][" << j << "] " << spDist[i][j] << endl;
            trace << "spPred[" << i << "][" << j << "] " << spPred[i][j] << endl;
        }
    }
    // Find the sequence for considering node pairs
    trace << endl << "+++++ Sequence of pairs +++++" << endl;
    Sequence(spPred , Seq , Home);
    cout << "Sequence" << endl;
    trace << "Sequence" << endl;

    // Select backbone links
    trace << endl << "+++++ Considering pairs +++++" << endl;
    //Compress(Seq, Home);
    Compress(Seq, Home, Bkbn);
    cout << "Compress" << endl;
    trace << "Compress" << endl;

    // Set node types
    cout << endl << "+++++ Selected node types +++++" << endl;
    trace << endl << "+++++ Selected node types +++++" << endl;
    for (int i = 0; i < nn; i++) {
        if (Bkbn[i] == 0) {
			Network::nodes[i].ntyped = 'E';
        }
        else {
			Network::nodes[i].ntyped = 'N';
        }
        trace << "Node " << Network::nodes[i].name
            << " is type "
            << Network::nodes[i].ntyped << endl;
    }
	Network::nodes[Median].ntyped = 'C';
    trace << "Node " << Network::nodes[Median].name << " is the centre so is type "
        << Network::nodes[Median].ntyped << endl;
    cout << "Ntype" << endl;
    trace << "Ntype" << endl;

    // Compute net cost
    NetCost = 0.0;
    cout << endl << "+++++ Selected links +++++" << endl;
    trace << endl << "+++++ Selected links +++++" << endl;
	int ne = Network::edges.size();
    for (int i = 0; i < ne; i++) {
        Edge *edge = &Network::edges[i];
		NetCost += edge->mult * edge->cost;
        trace << "Link " << Network::nodes[edge->ends[0]].name << "(" << edge->ends[0] << ")-("
              << edge->ends[1] << ") "<< Network::nodes[edge->ends[1]].name
              << " Distance " << edge->dist
              << " Service: " << Linetype::pLinetype[edge->type].svty
              << " Capacity: " << edge->cap
              << " Cost: " << edge->cost
              << " selected with mult " << edge->mult
              << " and total cost " << round(edge->mult * edge->cost) << endl;
    } /* endfor */
    cout << "Total Cost " << round(NetCost) << endl;
    trace << "Total Cost " << round(NetCost) << endl;
    cout << "NetCost" << endl;
    trace << "NetCost" << endl;

    delete [] Weight;
    delete [] Bkbn;
    delete [] Seq;
    Utils::Free2Darray(spDist, nn);
    Utils::Free2Darray(spPred, nn);
    Utils::Free2Darray(Home,   nn);

    cout << "MENTOR Done" << endl;
    trace << "MENTOR Done" << endl;
    trace.close();
    Canvas::my_canvas->drawNetwork();
}

//////////////////////////
////  Set node weights  //
//////////////////////////
void SetWeights( float Weight[]) {
	int nn = Network::nodes.size();
	for (int i = 0; i < nn; i++)
        Weight[i] = 0.0;
    for (int i = 0; i < nn; i++) {
        for (int j = 0; j < nn; j++) {
            int r = Network::getReq(i, j);
            if (r != -1) {
                // Ignore if there is no requirement between this pair
                Weight[i] += Network::reqs[r].bps;
                Weight[j] += Network::reqs[r].bps;
            }
        } /* endfor */
    } /* endfor */
}

/////////////////////
//  Find a median  //
/////////////////////
int SetMedian(int Flag , int Bkbn[] , float Weight[]) {
    int median;
    float moment, BestMoment;
	int nn = Network::nodes.size();

    BestMoment = FLT_MAX;
    for (int i = 0; i < nn; i++)
        if (!Flag || (Bkbn[i] == 1)) {
            moment = 0.0;
            for (int j = 0; j < nn; j++)
                moment += trunc(Utils::line_cost(0, i, j)) * Weight[j];
            if (moment < BestMoment) {
                BestMoment = moment;
                median = i;
            } /* endif */
        } /* endif */
    return(median);
}

////////////////////////////////////////////////////
//  Threshold backbone node assignment algorithm  //
////////////////////////////////////////////////////
void   Thresh(int median, float Weight[], int Bkbn[]) {
    int   bestI;
    int   nassg;
    float *merit;
    float radius, BestMerit;
    float Wparm;
    string::size_type sz;     // alias of size_t
	int nn = Network::nodes.size();

    merit = new float[nn];
    radius = 0.0;
    for (int i = 0; i < nn - 1; i++)
        for (int j = i + 1; j < nn; j++)
            radius = max(radius, Utils::line_cost(0, i, j));

    float wp = stof(Parm::getParm("WPARM"), &sz);
    float ut = stof(Parm::getParm("UTILIZATION"), &sz);
    float mn_cap = ut * Linetype::pLinetype[0].speedin;
    Wparm = wp * mn_cap;
    float tree_bias = stof(Parm::getParm("TREE_BIAS"), &sz);

    nassg = 0;
    for (int i = 0; i < nn; i++) {
        //if ( Weight[i] >= Wparm ) {
        // Take account of 'N' nodes and do not select 'E' nodes! PV130516
        if (((Weight[i] >= Wparm) || (Network::nodes[i].ntyped == 'N')) && (Network::nodes[i].ntyped != 'E')) {
            nassg++;
            Bkbn[i] = 1;
            merit[i] = (float)-1;
        } else {
            Bkbn[i] = -1;
            merit[i] = tree_bias * Utils::line_cost(0, i, median) / radius
                + (1 - tree_bias) * Weight[i] / Wparm;
        } /* endif */
    } /* endfor */

    float rparm = stof(Parm::getParm("RPARM"), &sz);
    radius *= rparm;
    for (int i = 0; i < nn; i++) {
        if (Bkbn[i] == 1) {
            for (int j = 0; j < nn; j++) {
                if ((Bkbn[j] == -1) && (Utils::line_cost(0, i, j) <= radius)) {
                    Bkbn[j] = 0;
                    // cout << "Bkbn " << j << " set to 0" << endl;
                    nassg++;
                    break;
                } /* endif */
            } /* endfor */
        } /* endif */
    } /* endfor */

    while (nassg < nn) {
        BestMerit = 0.0;
        for (int i = 0; i < nn; i++) {
            if (Network::nodes[i].ntyped == 'E') // Do not select 'E' nodes! PV130516
                continue;
            if ( (Bkbn[i] == -1) && (merit[i] > BestMerit) ) {
                bestI = i;
                BestMerit = merit[i];
            } /* endif */
        } /* endfor */
        Bkbn[bestI] = 1;
        nassg++;
        for (int i = 0; i < nn; i++) {
            if ( (Bkbn[i] == -1) && (Utils::line_cost(0, i, bestI) < radius) ) {
                Bkbn[i] = 0;
                nassg++;
            } /* endif */
        } /* endfor */
    } /* endwhile */
    delete [] merit;
}

//////////////////////////////
//  Tree finding heuristic  //
//////////////////////////////
void PrimDijk(int median , int Bkbn[]) {
    int    besti;
    float  BestLabel;
    float  *label;
    float  *ldist;
    int    *InTree;
    int    node1;
	int nn = Network::nodes.size();
	string::size_type sz;     // alias of size_t

    label = new float[nn];
    ldist = new float[nn];
    InTree = new int[nn];

    for (int i = 0; i < nn; i++) {
        node1 = i;
        Network::nodes[node1].pred_node = median;
        label[i] = Utils::line_cost(0, i, median);
        ldist[i] = Utils::line_cost(0, i, median);
        InTree[i] = false;
    } /* endfor */
    InTree[median] = true;

    float alpha = stof(Parm::getParm("ALPHA"), &sz);
    while ( true ) {
        BestLabel = FLT_MAX;
        for (int i = 0; i < nn; i++) {
            if ( (Bkbn[i]==1) && (!InTree[i]) && (label[i]<BestLabel) ) {
                besti = i;
                BestLabel = label[i];
            } /* endif */
        } /* endfor */
        if ( BestLabel > .99*FLT_MAX ) break;
        InTree[besti] = true;
        for (int i = 0; i < nn; i++) {
            if (!InTree[i] && (label[i] > alpha * ldist[besti] + Utils::line_cost(0 , i, besti))) {
                label[i] = alpha * ldist[besti] + Utils::line_cost(0, i, besti);
                ldist[i] = ldist[besti] + Utils::line_cost(0, i, besti);
                Network::nodes[i].pred_node = besti;
            } /* endif */
        } /* endfor */
    } /* endwhile */

    delete [] label;
    delete [] ldist;
    delete [] InTree;
}

////////////////////////////////////////////
//  Find shortest paths through the tree  //
////////////////////////////////////////////
void SetDist(int Median, float **spDist, int **spPred ) {
    int i, j, k, l, n, p, pp;
    int *PreOrder, *InTree;
    int node1;
	int nn = Network::nodes.size();

    PreOrder = new int[nn];
    InTree = new int[nn];

    // Find a preordering of the nodes
    n = 0;
    for (i = 0; i < nn; i++) {
        InTree[i] = false;
    }
    InTree[Median] = true;
    PreOrder[n++] = Median;

    while (n < nn) {
        for (i = 0; i < nn; i++) {
            node1 = i;
            if (!InTree[i] && InTree[Network::nodes[node1].pred_node]) {
                InTree[i] = true;
                PreOrder[n++] = i;
            } /* endif */
        } /* endfor */
    } /* endwhile */

    // Traverse the nodes in preorder, setting the distances
    for (i = 0; i < nn; i++) {
        j = PreOrder[i];
        p = Network::nodes[j].pred_node;
        spDist[j][j] = 0.0;
        for (k = 0; k < i; k++) {
            l = PreOrder[k];
            spDist[j][l] = spDist[p][l] + Utils::line_cost(0, j, p);
            spDist[l][j] = spDist[j][l];
        } /* endfor */
    } /* endfor */

    // Set the predecessors
    for (i = 0; i < nn; i++) {
        for (j = 0; j < nn; j++) {
            spPred[i][j] = Network::nodes[j].pred_node;
        }
        spPred[i][i] = i;
    }
    for (i = 0; i < nn; i++) {
        if (i == Median) {
            continue;
        }
        p = Network::nodes[i].pred_node;
        spPred[i][p] = i;
        while (p != Median) {
            pp = Network::nodes[p].pred_node;
            spPred[i][pp] = p;
            p = pp;
        } /* endwhile */
    } /* endfor */

    delete [] PreOrder;
    delete [] InTree;
}

//////////////////////////////////////////////////////
//  Find the order in which to consider node pairs  //
//////////////////////////////////////////////////////
void Sequence(int **spPred, int *Seq, int **Home) {
    int h, i, j, d, p, pp, p1, p2, pairih, pairjh, npair, iseq, nseq;
    int *nDep, *Dep1, *Dep2, *Pair;
	int nn = Network::nodes.size();

    nDep = new int[nn * nn];
    Dep1 = new int[nn * nn];
    Dep2 = new int[nn * nn];
    Pair = new int[nn * nn];
    npair = 0;
    for (i = 0; i < nn - 1; i++) {
        for (j = i + 1; j < nn; j++) {
            Pair[npair] = MakeIndexPair(i, j);
            nDep[Pair[npair]] = 0;
            npair++;
        }
    }

    for (p = 0; p < npair; p++) {
        SplitIndexPair(Pair[p], &i, &j);
        p1 = spPred[i][j]; // Pred in shortest path through the tree
        p2 = spPred[j][i];
        if (p1 == i) {  // Tree link
            h = -1;
        } else if (p1 == p2) {   // 2-hop path; only one possible home
            h = p1;
        } else if (Utils::line_cost(0, i, p1) + Utils::line_cost(0, p1, j)
                   <= Utils::line_cost(0, i, p2) + Utils::line_cost(0, p2, j)) { // Choose cheaper detour
            h = p1;
        } else {
            h = p2;
        }
        Home[i][j] = h;
        if (h == -1) {
            Dep1[ Pair[p] ] = -1;
            Dep2[ Pair[p] ] = -1;
            continue;
        }
        pairih = MakeIndexPair(i, h);
        Dep1[Pair[p]] = pairih;
        nDep[pairih]++;
        pairjh = MakeIndexPair(j, h);
        Dep2[Pair[p]] = pairjh;
        nDep[pairjh]++;
    } /* endfor */

    nseq = 0;
    for (p = 0; p < npair; p++) {
        pp = Pair[p];
        if (nDep[pp] == 0) {
            Seq[nseq++] = pp;
        }
    } /* endfor */

    iseq = 0;
    while (iseq < nseq) {
        p = Seq[iseq++];
        d = Dep1[p];
        if (d < 0) { // There seems to be some cases when this causes a crash PV
            continue;
        }
        if ((d >= 0) && (nDep[d] == 1)) {
            Seq[nseq++] = d;
        }
        else {
            nDep[d]--;
        }
        d = Dep2[p];
        if ((d >= 0) && (nDep[d] == 1)) {
            Seq[nseq++] = d;
        }
        else {
            nDep[d]--;
        }
    } /* endwhile */

    if (nseq != npair) {
        abort();
    }

    delete [] nDep;
    delete [] Dep1;
    delete [] Dep2;
    delete [] Pair;
}

int MakeIndexPair(int i , int j )
{
	int nn = Network::nodes.size();
	if ( i < j )
        return(nn * i + j);
    else
        return(nn * j + i);
}

void SplitIndexPair(int pair , int *i , int *j )
{
	int nn = Network::nodes.size();
	*i = pair / nn;
    *j = pair % nn;
}

////////////////////
//  Select Links  //
////////////////////
void Compress(int Seq[], int *Home[], int Bkbn[]) {
    int n1, n2, h, p, npairs;
    float load, overflow12, overflow21;
    int mult;
    float Tload;
    int edge = -1;
    string::size_type sz;     // alias of size_t
	int nn = Network::nodes.size();

    for (n1 = 0; n1 < nn; n1 ++) {
        for (int n2 = 0; n2 < nn; n2++) {
            int r = Network::getReq(n1, n2);
            if (r != -1) {
                Network::Lreq[n1][n2] = Network::reqs[r].bps;
            } else {
                Network::Lreq[n1][n2] = 0.0;
            }
        }
    }

    npairs = nn * (nn - 1) / 2;

    char duplexity = Parm::getParm("DUPLEXITY")[0];
    float utilization = stof(Parm::getParm("UTILIZATION"), &sz);
    float slack = stof(Parm::getParm("SLACK"), &sz);
    float mn_cap = utilization * Linetype::pLinetype[0].speedin;

    for ( p = 0 ; p < npairs ; p++ ) {
        SplitIndexPair(Seq[p] , &n1 , &n2 );
        h = Home[n1][n2];
        if (duplexity == 'F')
            load = max( Network::Lreq[n1][n2] , Network::Lreq[n2][n1] );
        else
            load = Network::Lreq[n1][n2] + Network::Lreq[n2][n1];
        trace << "Considering link " << n1 << "-" << n2 << " and home " << h
              << " with load " << load << endl;

        Tload = load;
        if( load >= mn_cap ) {
            trace << "Load " << load << " is greater than MN_CAP " << mn_cap << endl;
            mult = (int)(load / mn_cap);
            load -= mult * mn_cap;
            trace << "Mult is " << mult << ". Overflow is " << load << endl;
        } else {
            mult = 0;
        }
        if (((h == -1) && (load > 0.0)) || (load >= (1 - slack) * mn_cap)) {
            mult++;
            trace << "Mult has been incremented to " << mult << ". Overflow is 0" << endl;
            overflow12 = 0.0;
            overflow21 = 0.0;
        } else {
            if (duplexity == 'F') {
                overflow12 = max((float) 0.0 , (float) Network::Lreq[n1][n2] - mult * mn_cap );
                overflow21 = max((float) 0.0 , (float) Network::Lreq[n2][n1] - mult * mn_cap );
                trace << "F:overflow12,21 is now " << overflow12 << " " << overflow21 << endl;
            } else {
                overflow12 = max((float) 0.0 , (float) Network::Lreq[n1][n2] + Network::Lreq[n2][n1] - mult * mn_cap );
                overflow21 = 0.0;
                trace << "H:overflow is now " << overflow12 << endl;
            }
        }

        if (mult > 0) {
            if (duplexity == 'F') {
                Tload -= max(overflow12, overflow21);
            } else {
                Tload -= overflow12;
            }
            int Ttype = 0;
            int Tmult = 0;
            float Tcost1 = Utils::line_cost(0, n1, n2);
            float Tmult1 = mult;
            Edge::sizeEdge(n1, n2, Ttype, Tmult, Tload);
            float Tcost2 = Utils::line_cost(Ttype, n1, n2);
            float Tmult2 = Tmult;
            float Tcap = Linetype::pLinetype[Ttype].speedin * utilization;
            if (Tcost1 != Tcost2) {
                trace << "Original: Load: " << Tload << " Cost: " << Tcost1 * Tmult1
                      << " Service: " << Linetype::pLinetype[0].svty << " Mult: " << mult << endl;
                trace << "sizeLine: Load: " << Tload << " Cost: " << Tcost2 * Tmult2
                      << " Service: " << Linetype::pLinetype[Ttype].svty << " Mult: " << Tmult << endl;
            }

            edge = Network::addEdge(n1, n2);
            Network::edges[edge].dist = Utils::line_dist(n1, n2);
			Network::edges[edge].cap = Tcap;
			Network::edges[edge].mult = Tmult;
			Network::edges[edge].type = Ttype;
			Network::edges[edge].cost = Tcost2;
			Network::edges[edge].load = Tload;
            if (h == -1) {
				Network::edges[edge].Ltree = true;
            }
            else {
				Network::edges[edge].Ltree = false;
            }
            trace << "Inserting edge End1 " << n1 << "  End2 " << n2 << " mult " << Tmult << endl;
        }

        if (overflow12 > 0.0) {
            Network::Lreq[n1][h] += overflow12;
            Network::Lreq[h][n2] += overflow12;
            trace << "Lreq " << n1 << " " << h << " is now " << (int) Network::Lreq[n1][h] << endl;
            trace << "Lreq " << h << " " << n2 << " is now " << (int) Network::Lreq[h][n2] << endl;
		}

        if (overflow21 > 0.0) {
            Network::Lreq[n2][h] += overflow21;
            Network::Lreq[h][n1] += overflow21;
            trace << "Lreq " << n2 << " " << h  << " is now " << Network::Lreq[n2][h] << endl;
            trace << "Lreq " << h  << " " << n1 << " is now " << Network::Lreq[h][n1] << endl;
		}
    }
}
