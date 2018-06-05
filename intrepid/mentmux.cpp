
#include "mentmux.h"

//#include "arc.h"
//#include "Edge.h"
#include "equipment.h"
#include "linetype.h"
#include "Network.h"
#include "Parm.h"
#include "Requirements.h"
#include "Utils.h"

long  Mentmux::wmax = -1;
long  Mentmux::wmin = -1;
int   Mentmux::dcmax = -1;
int   Mentmux::distmax = -1;
vector<vector<int> > Mentmux::linkmat;
vector<vector<int> > Mentmux::hops;  // Initialise hops 2D vector
vector<vector<int> > Mentmux::seq; // Initiliase sequence 2D vector

bool Mentmux::size_hops(int nn) {
    // Set size of hop vector (nn x nn)
    hops.resize(nn);
    for (int i = 0; i < nn; ++i)
        hops[i].resize(nn);
    return true;
} // defineHops

bool Mentmux::size_linkmat(int nn) {
    // Set size of vector
    linkmat.resize(nn);
    for (int i = 0; i < nn; ++i)
        linkmat[i].resize(nn);
    // Initialise so I can check for no link easily
    for (int i = 0; i < nn; i++)
        for (int j = 0; j < nn; j++)
            linkmat[i][j] = -1;   // null pointer
    return true;
}

void Mentmux::mentorI(string alg) {
	int nn = Network::nodes.size();
    int cost;
    int node_cost;
    int node;
    string link_type_string;
    float utilization;
    int med;
    bool rc;
    string::size_type sz;     // alias of size_t
    
    link_type_string = Parm::getParm("LINK_TYPE");
    if (link_type_string != "") {
        for (int i = 0; i < Linetype::ncap; i++) {
            if (link_type_string == Linetype::pLinetype[i].svty) {
                Network::default_link_type = i;
                break;
            }
        }
    }

    initialize_net();  // set everything back to the start
    
    // It won't do if we have edges in the network
	if (Network::edges.size() > 0) {
		Network::arcs.resize(0);
		Network::edges.resize(0);
	}

    utilization = stof(Parm::getParm("UTILIZATION"), &sz);
    if (utilization < 0.0 || utilization > 1.0) {
        cout << "MentorI() failed.  Utilization out of range." << endl;
        return;
    }

    size_linkmat(nn); // set size of arcs 2D vector
    size_hops(nn);    // set size of hops 2D vector

    set_bkbn();
//    for (Node &n : Node::pNode) {
//        if (n.bkbn == 0) {
//            n.ntyped = 'E';
//        } else if (n.bkbn == 1) {
//            n.ntyped = 'N';
//        } else {
//            cout << "MentorI: Node " << n.name << " has returned from set_bkbn with bkbn " << n.bkbn << endl;
//            return;
//        }
//    }
    for (int i = 0; i < nn; i++) {
        node = i;
        if (Network::nodes[node].bkbn == 0) {
			Network::nodes[node].ntyped = 'E';
        } else if (Network::nodes[node].bkbn == 1) {
			Network::nodes[node].ntyped = 'N';
        } else {
            cout << "MentorI: Node " << Network::nodes[node].name << " has returned from set_bkbn with bkbn " << Network::nodes[node].bkbn << endl;
            return;
        }
    }

    med = set_med();
	Network::nodes[med].ntyped = 'C';
    cout << "Median: " << Network::nodes[med].nodenum << " " << Network::nodes[med].name << " " << Network::nodes[med].ntyped << endl;

    rc = primd(med);
    if (!rc) {
        cout << "Primd() failed. Mentmux() returning without calculating a network." << endl;
        return;
    }

    if (alg == "MENTOR-I") {
        find_hops(med);
        rc = compressMUX();     // BEWARE: this might delete edges, check edgenum == -1
        if (!rc) {
            cout << "MentorI: CompressMUX has failed." << endl;
            return;
        }
    }
    //    else if (alg == "MENTOR-II") {
    //        compressDG(input, linkmat);
    //    }

    node_cost = select_node_model();
    if (node_cost == -1) {
        cout << "MentorI: first node_cost has failed." << endl;
        return;
    }

    resize_links();

    //    /* The logic is a little funny but we first size the nodes based on   */
    //    /* The links.  Then we examine the tailcircuits to see if we can save */
    //    /* money on the links without changing the nodes.  Finally we resize  */
    //    /* the nodes if we have reduced the number of links.                  */

    node_cost = select_node_model();
    if (node_cost == -1) {
        cout << "MentorI: second node_cost has failed." << endl;
        return;
    } /* endif */

    cout << "Backbone Nodes" << endl;
    for (int i = 0; i < nn; i++) {
        if (Network::nodes[i].ntyped != 'E') {
            cout << i << " " << Network::nodes[i].name << " " << Network::nodes[i].ntyped << endl;
        }
    }

    cost = total_link_cost();
    if (Parm::getParm("LINK_COST") == "") {
        Parm::addParm("LINK_COST", "I", "C", "0 100000000", "0");
    }
    Parm::changeParm("LINK_COST", to_string(cost));

    if (Parm::getParm("NODE_COST") == "") {
        Parm::addParm("NODE_COST", "I", "C", "0 100000000", "0");
    }
    Parm::changeParm("NODE_COST", to_string(node_cost));

    if (Parm::getParm("COST") == "") {
        Parm::addParm("COST", "I", "C", "0 100000000", "0");
    }
    Parm::changeParm("COST", to_string(cost + node_cost));

    cout << "Total line cost: " << cost << endl;
    cout << "Total node cost: " << node_cost << endl;
    cost = cost + node_cost;
    cout << "Total cost:      " << cost << endl;
} // mentorI

bool Mentmux::initialize_net() {
    float xcntr = (float) 0.0;
    float ycntr = (float) 0.0;
    float sumw  = (float) 0.0;
    float dx    = (float) 0.0;
    float dy    = (float) 0.0;
    float smallest_moment;
    int center = -1;
    wmax  = 0;
    wmin  = LONG_MAX;
	int nn = Network::nodes.size();

    for (int i = 0; i < nn; i++) {
        Network::nodes[i].weight = (float) 0.0;
    } /* endforeach */

    // Set weights
	for (int i = 0; i < nn; i++) {
        Network::nodes[i].weight = 0.0;
	}
    for (int i = 0; i < nn; i++) {
        for (int j = 0; j < nn; j++) {
            int r = Network::getReq(i, j);
            if (r != -1) {
                // leave weight alone if no requirement has been found for this pair
                Network::nodes[i].weight += Network::reqs[r].bps;
                Network::nodes[j].weight += Network::reqs[r].bps;
            }
        } /* endfor */
    } /* endfor */


    for (int i = 0; i < nn; i++) {
        sumw += Network::nodes[i].weight;
        if (Network::nodes[i].weight > wmax)
            wmax = Network::nodes[i].weight;
        if (Network::nodes[i].weight < wmin)
            wmin = Network::nodes[i].weight;
    }

    for (int i = 0; i < nn; i++) {
        if (Parm::getParm("LLVH") == "VH") {
            xcntr += Network::nodes[i].hcord * (float)Network::nodes[i].weight;
            ycntr += Network::nodes[i].vcord * (float)Network::nodes[i].weight;
        }
        else if (Parm::getParm("LLVH") == "LL") {
            xcntr += Network::nodes[i].longitude * (float)Network::nodes[i].weight;
            ycntr += Network::nodes[i].latitude  * (float)Network::nodes[i].weight;
        }
        else {
            cout << "Initialize_net failed. Invalid LLVH parameter." << endl;
            return false;
        }
    }

    xcntr /= sumw;
    ycntr /= sumw;

    dcmax = 0;
    distmax = 0;
    for (int i = 0; i < nn; i++) {
        if (Parm::getParm("LLVH") == "VH") {
            dx = Network::nodes[i].hcord - xcntr;
            dy = Network::nodes[i].vcord - ycntr;
        }
        else if (Parm::getParm("LLVH") == "LL") {
            dx = Network::nodes[i].longitude - xcntr;
            dy = Network::nodes[i].latitude  - ycntr;
        }
        Network::nodes[i].dcent = (int)sqrt((double)(dx * dx + dy * dy));
        if (Network::nodes[i].dcent > dcmax)
            dcmax = Network::nodes[i].dcent;
        for (int j = 0; j < nn; j++) {
            // Note that dist return COST of default line type
            if (Utils::dist(i, j) > distmax && Utils::dist(i, j) != LINFINITY)
                distmax = Utils::dist(i, j);
        }
    }

    distmax = 0;
    smallest_moment = (float)1e20;
    for (int i = 0; i < nn; i++) {
        Network::nodes[i].moment = (float) 0.0;
        for (int j = 0; j < nn; j++) {
            if (Utils::dist(i, j) > distmax && Utils::dist(i, j) != LINFINITY)
                distmax = Utils::dist(i, j);
            Network::nodes[i].moment += Network::nodes[j].weight * Utils::dist(i, j);
        } /* endforeach */
        if (Network::nodes[i].moment < smallest_moment) {
            smallest_moment = Network::nodes[i].moment;
            center = i;
        } /* endif */
    } /* endfor */
    cout << "Center: " << center << " " << Network::nodes[center].name << endl;

    if (center == -1) {
        cout << "Initialize_net() failed. Can't find a node for center of net." << endl;
        return false;
    } /* endif */

    dcmax = 0;
    for (int i = 0; i < nn; i++) {
        Network::nodes[i].dcent = Utils::dist(i, center);
        if (Network::nodes[i].dcent > dcmax  && Network::nodes[i].dcent != (float)LONG_MAX)
            dcmax = Network::nodes[i].dcent;
    }
    //    cout << "Distances to center" << endl;
    //    for (int i = 0; i < Node::nn; i++)
    //        cout << Node::pNode[i].name << " " << Node::pNode[i].dcent << endl;

    if (dcmax == 0) {
        cout << "Initialize_net() failed. There is a node at distance 0 from all other nodes." << endl;
        return false;
    } /* endif */

    return true;

} // initialize_net

bool Mentmux::set_bkbn()
{
    string cluster_mode;

    cluster_mode = Parm::getParm("CLUSTER_MODE");
    if (cluster_mode == "") {
        cout << "Set_bkbn() failed. The cluster mode parm is not present." << endl;
        return false;
    } /* endif */
    
    if (cluster_mode == "THRESH") {
        thresh_set_bkbn();
        return true;
    }
    //else if (cluster_mode = "KMEANS" {
    //    for_each(elem, net->nodes) {
    //        node = (PNODE)elem->value;
    //        node->lprop->calc_hpcc.temp_type = node->iprop->ntypei;
    //    } /* end foreach */
    //    kmeans_set_bkbn(net);
    //    return(TRUE);
    //}
    else {
        cout << "Set_bkbn() failed. Unimplemented clustering algorithm." << endl;
        return false;
    }
    
    return true;
} // set_bkbn

bool Mentmux::thresh_set_bkbn() {
    string link_type_string;
    int ltype;
    int lcap;
    float utilization;
    float tree_bias;
    float wparm;
    float rparm;
    int jbest;
//    int jbest_ix;
    float mbest;
    int node1, node2;
	int nn = Network::nodes.size();
	string::size_type sz;     // alias of size_t

    utilization = stof(Parm::getParm("UTILIZATION"), &sz);

    tree_bias = stof(Parm::getParm("TREE_BIAS"), &sz);
    if (tree_bias < 0.0 || tree_bias > 1.0) {
        cout << "Thresh_set_bkbn() failed. Tree bias out of range." << endl;
        return false;
    } /* endif */

    wparm = stof(Parm::getParm("WPARM"), &sz);
    if (wparm <= 0.0) {
        cout << "Thresh_set_bkbn() failed. Wparm out of range." << endl;
        return false;
    } /* endif */
    
    rparm = stof(Parm::getParm("RPARM"), &sz);
    if (rparm <= 0.0) {
        cout << "Thresh_set_bkbn() failed. Rparm out of range." << endl;
        return false;
    } /* endif */

    link_type_string = Parm::getParm("LINK_TYPE");
    ltype = 0;
    if (link_type_string != "") {
        for (int i = 0; i < Linetype::ncap; i++) {
            if (link_type_string == Linetype::pLinetype[i].svty) {
                ltype = i;
                break;
            } /* endif */
        } /* endfor */
    } /* endif */
    lcap = (int)(Linetype::pLinetype[ltype].speedin * utilization);

    for (int i = 0; i < nn; i++) {
        // Allow for nodes present as N or E in input file
        node1 = i;
        if (Network::nodes[node1].ntypei == 'N')
			Network::nodes[node1].bkbn = 1;
        else if (Network::nodes[node1].ntypei == 'E')
			Network::nodes[node1].bkbn = 0;
        else
			Network::nodes[node1].bkbn = -1;
    }

    for (int i = 0; i < nn; i++) {
        node1 = i;
		Network::nodes[node1].merit =
                tree_bias * (dcmax - Network::nodes[node1].dcent) / dcmax
                + (1 - tree_bias) * Network::nodes[node1].weight / wmax
                + (Network::nodes[node1].weight >= wparm * lcap);
        if (Network::nodes[node1].merit < 0) {
            cout << "Node " << node1 << " " << Network::nodes[node1].name 
				<< " merit is " << Network::nodes[node1].merit << endl;
        }
    }

    for (int i = 0; i < nn; i++) {
        mbest = (float)-1;
        jbest = -1;
        for (int j = 0; j < nn; j++) {
            node2 = j;
            if ((Network::nodes[node2].bkbn == -1) && (Network::nodes[node2].merit > mbest )) {
                mbest = Network::nodes[node2].merit;
                jbest = node2;
            }
        }
        if (jbest == -1)
            break;  /* We haven't any nodes left which are undecided */
		Network::nodes[jbest].bkbn = 1;
        if (Network::nodes[jbest].weight < wparm * lcap ) {
            for (int j = 0; j < nn; j++) {
                node2 = j;
                if ((node2 != jbest) &&
                        (Network::nodes[node2].bkbn == 1) &&
                        (Utils::dist(node2, jbest) < (long)(rparm * distmax) ) )
                {
					Network::nodes[jbest].bkbn = 0;
                }
            }  /* endfor_each */
        } /* endif */
    } /* endfor */
    return true;
} // thresh_set_bkbn

int Mentmux::set_med() {
    float  bestm;      /*  Best sum of weight times distance  */
    int med = -1;     /*  Selected median */
    float  orig_bestm;
    int node1;
	int nn = Network::nodes.size();

    orig_bestm = bestm = (float)1.000e20;
    for (int i = 0; i < nn; i++) {
        node1 = i;
        if ((Network::nodes[node1].bkbn == 1) && (Network::nodes[node1].moment < bestm)) {
            bestm = Network::nodes[node1].moment;
            med = node1;
        } /* endif */
    } /* endforeach */

    if (bestm == orig_bestm) {
        cout << "Set_med() failed. No median found." << endl;
        for (int i = 0; i < nn; i++) {   /* RC042095 extra error handling */
            node1 = i;
            if (Network::nodes[node1].moment < bestm) {
                bestm = Network::nodes[node1].moment;
                med = node1;
            } /* endif */
        } /* endforeach */
    } /* endif */

    if (med == -1) {
        med = 0;
    } /* endif */

    return med;
} // set_med

bool Mentmux::primd(int root) {
    int nbkbn;
    int bestd;
    int bestnode;
    int dtest;
    vector<int> dtree;
    vector<int> dnew;
    vector<int> intree;
	int nn = Network::nodes.size();
	int node1;
    float alpha;
    string::size_type sz;     // alias of size_t

    /* This is a modified version of Prim's/Dijkstra's Algorithms. The tree
    formed is a composite of an MST and a shortest path tree from the
    root node. Also, the vector bkbn defines non-pendant nodes in the
    tree (i.e., nodes which are permitted to be non-pendant.)             */

    alpha = stof(Parm::getParm("ALPHA"), &sz);
    if (alpha < 0.0 || alpha > 1.0) {
        cout << "Primd failed. Alpha out of range." << endl;
    } /* endif */

    dtree.resize(nn);
    dnew.resize(nn);
    intree.resize(nn);

    nbkbn = 0;
    for (int i = 0; i < nn; i++) {
        node1 = i;
        intree[node1] = 0;
        if (Network::nodes[node1].bkbn == 1)
            ++nbkbn;
        dnew[node1] = LINFINITY;
		Network::nodes[node1].pred_node = 0;
    } /* end foreach */
    dtree[root] = dnew[root] = 0;
	Network::nodes[root].pred_node = root;

    for (int nscanned = 1; nscanned <= nbkbn; nscanned++) {
        bestd = 10000000;
        for (int i = 0; i < nn; i++) {
            node1 = i;
            if ((intree[node1] == 0) &&
                    (Network::nodes[node1].bkbn == 1) &&
                    (dnew[node1] < bestd)) {
                bestd = dnew[node1];
                bestnode = node1;
            } /* endif */
        } /* foreach */
        intree[bestnode] = 1;
        dtree[bestnode] = dtree[Network::nodes[bestnode].pred_node]
                + Utils::dist(bestnode, Network::nodes[bestnode].pred_node);
        int d = (long)(alpha * dtree[bestnode]);
        for (int i = 0; i < nn; i++) {
            node1 = i;
            if ((intree[node1] == 0) &&
                    (dtest = d + Utils::dist(bestnode, node1)) < dnew[node1]) {
                dnew[node1] = dtest;
				Network::nodes[node1].pred_node = bestnode;
            } /* endif */
        } /* endforeach */
    } /* endfor */

    for (int i = 0; i < nn; i++) {
        node1 = i;
        int parent = Network::findNode(Network::nodes[node1].parent);
        if (node1 != parent) {
			Network::nodes[node1].pred_node = parent;
        } /* endif */
        if (Network::nodes[node1].pred_node == -1) {
            cout << "Primd() has failed. Check the link costs for the network." << endl;
            return false;
        } /* endif */
    } /* endfor */
    //    cout << "Primd Tree" << endl;
    //    for (int i = 0; i < Node::nn; i++)
    //        cout << Node::pNode[i].name << " " << Node::pNode[i].pred_node << endl;

    build_tree();
    return true;
} // primd

bool Mentmux::build_tree() {
    string edge_id;
    string arc_id;
    int    arc = -1;
    int    arc2 = -1;
    int    edge = -1;
    int    node1 = -1;
    int    node2 = -1;
	int    nn = Network::nodes.size();

    for (int i = 0; i < nn; i++) {
        for (int j = 0; j < nn; j++) {
            linkmat[i][j] = -1;   // Null pointer
        } /* endfor */
    } /* endfor */

    for (int i = 0; i < nn; i++) {
        node1 = i;
        node2 = Network::nodes[node1].pred_node;
        if (node1 == node2)
            continue;   /* This is the root */
		edge = Network::addEdge(node1, node2);
		Network::edges[edge].mult = 0;
		Network::edges[edge].type = -1;
		Network::edges[edge].cost = 0;
		Network::edges[edge].length = 0;
		Network::edges[edge].Ltree = true;

        arc = Network::addArc("_F", edge, node1, node2);
        linkmat[node1][node2] = arc;
        arc2 = Network::addArc("_R", edge, node2, node1);
		Network::nodes[node1].pred_arc = arc2;
        linkmat[node2][node1] = arc2;
    } /* endforeach */
    return true;
} // build_tree

void Mentmux::find_hops(int root) {
    int s = -1;
    int ps = -1;
	int nn = Network::nodes.size();
	vector<int> list;
    list.resize(nn);
    int node1 = -1;

    list[0] = root;
    int nlist = 1;

    for (int i = 0 ; i < nn ; i++ ) {
        s = list[i];
        ps = Network::nodes[s].pred_node; // Note: We assume that pred[root] = root
        for (int j = 0; j < nn; j++) {
            node1 = j;
            if((node1 != root) && (Network::nodes[node1].pred_node == s))
                list[nlist++] = node1;
        }
        hops[s][s] = 0;
        for (int j = 0 ; j < i ; j++ ) {
            int k = list[j];
            hops[s][k] = hops[k][s] = hops[k][ps] + 1;
        }
    }
    //    cout << "Hops" << endl;
    //    for (int i = 0; i < nn; i++) {
    //        for (int j = 0; j < nn; j++) {
    //            cout << Node::pNode[i].name << " " << Node::pNode[j].name << " " << hops[i][j] << endl;
    //        }
    //    }
} // find_hops

bool Mentmux::compressMUX() {
	int m;
	long v;
	int nreqs;
	int s, d, h;
	long thresh;
	float ratio;
	long origsd, origds;
	long rsd, rds;
	float tandem_messages;
	float slack;
	string link_type_string;
	int link_type;
	string full_duplex_operation_string;
	bool full_duplex_operation;
	static int nn = Network::nodes.size();
	static bool include_tree = false;
    static vector<vector<int> >   lreqs; // vector of arc requirements
	static vector<vector<int> >   mult; // vector of line multiplicity
	static vector<vector<float> > lmsgs; // vector of arc messages
	static vector<int> node_vector;
	float utilization;
	int cap;
	Req *req;
	int arc, arc2;
	int edge;
	string parm;
	static bool bifurcate = false;
	string::size_type sz;     // alias of size_t

	parm = Parm::getParm("BIFURCATE");
	if (parm != "")
	{
		if (parm == "Y") {
			bifurcate = true;
		}
		else
		{
			bifurcate = false;
		} /* endif */
	} /* endif */

	lreqs.resize(nn);
	for (int i = 0; i < nn; ++i)
		lreqs[i].resize(nn);

	mult.resize(nn);
	for (int i = 0; i < nn; ++i)
		mult[i].resize(nn);

	lmsgs.resize(nn);
	for (int i = 0; i < nn; ++i)
		lmsgs[i].resize(nn);

	seq.resize(3);
	for (int i = 0; i < 3; ++i)
		seq[i].resize((nn*(nn - 1)) / 2);

	node_vector.resize(nn);
	for (int i = 0; i < nn; i++)
		node_vector[i] = i;

	nreqs = sequenceMUX();

	full_duplex_operation_string = Parm::getParm("DUPLEXITY");
	if (full_duplex_operation_string == "") {
		full_duplex_operation = true;
	}
	else if (full_duplex_operation_string == "HALF") {
		full_duplex_operation = false;
	}
	else {
		full_duplex_operation = true;
	} /* endif */

	link_type_string = Parm::getParm("LINK_TYPE");
	cout << "Link type " << link_type_string << endl;
	link_type = 0;
	if (link_type_string != "") {
		for (int i = 0; i < Linetype::ncap; ++i) {
			if (link_type_string == Linetype::pLinetype[i].svty) {
				link_type = i;
				break;
			} /* endif */
		} /* endfor */
	} /* endif */

	utilization = stof(Parm::getParm("UTILIZATION"), &sz);
	cout << "Utilization = " << utilization << endl;

	cap = (int)(Linetype::pLinetype[link_type].speedin * utilization);

	slack = stof(Parm::getParm("SLACK"), &sz);
	cout << "Slack = " << slack << endl;
	if (slack < 0.0 || slack > 1.0) {
		cout << "CompressMUX() failed. Slack out of range." << endl;
		return false;
	} /* endif */

	thresh = (long)(cap * (1 - slack));
	if (thresh < 1)
		thresh = 1;

	for (int i = 0; i < nn; i++) {
		Network::nodes[i].mps = (float)0;
	}

	for (int i = 0; i < nn; i++) { // We initialize the node messages to 0
		for (int j = 0; j < nn; j++) {
			mult[i][j] = lreqs[i][j] = 0;
			lmsgs[i][j] = (float)0.0;
		} /*endfor */
	} /* endfor */

	// Make a local matrix of the requirements and msgs
	int nr = Network::reqs.size();
	for (int r = 0; r < nr; r++) {
		req = &Network::reqs[r];
		int i = req->ends[0]->nodenum;
		int j = req->ends[1]->nodenum;
		lreqs[i][j] += (int)req->bps;
		lmsgs[i][j] += (float)req->mps;
		Network::nodes[node_vector[i]].mps += req->mps;
		Network::nodes[node_vector[j]].mps += req->mps;
	} /*endfor */

    cout << "Considering links" << endl;

    include_tree = false;
    for (long r = 0; r < nreqs; r++) {
        s = seq[0][r];
        d = seq[1][r];
        h = seq[2][r];
        if (full_duplex_operation)
            v = (lreqs[s][d] > lreqs[d][s]) ? lreqs[s][d] : lreqs[d][s];
        else
            v = lreqs[s][d] + lreqs[d][s];
        //       if(trace)
        //           fprintf( fp , "s[%d]=%7s    d[%d]=%7s    h[%d]=%7s    reqs %6ld %6ld " ,
        //                    r, s->id , r, d->id , r, h->id , lreqs[s->nodenum][d->nodenum] , lreqs[d->nodenum][s->nodenum] );

        m = (int) ceil((double)v / cap);
        if (include_tree && ((d == Network::nodes[s].pred_node)||(s == Network::nodes[d].pred_node)) 
			&& m == 0)
            m = 1;  /* Big change */
//        cout << "s:" << s << " d:" << d << " h:" << h << " m:" << m << " v:" << v;
        if (bifurcate) {
            if ((d == Network::nodes[s].pred_node)||(s == Network::nodes[d].pred_node))
                mult[s][d] = m;
            else {  /* non-tree link */
                mult[s][d] = 0;
                rsd = origsd = lreqs[s][d];
                rds = origds = lreqs[d][s];
                while (v > (long)((1.0 - slack) * cap)) {
                    ++mult[s][d];
                    v -= cap;    /* oddly this line is correct in full or half duplex */
                    if (full_duplex_operation) {
                        rsd -= cap;
                        rds -= cap;
                    } else {
                        ratio = ((float)rsd) / (rsd + rds);
                        rsd -= cap * ratio;
                        rds -= cap * ((float)1 - ratio);
                    } /* endif */
                } /* endwhile */
                rsd = max(rsd, 0l);    /* the remaining requirement to be homed */
                rds = max(rds, 0l);    /* "                                     */
                lreqs[s][d] -= rsd;
                lreqs[d][s] -= rds;

                if (origsd > 0)
                    tandem_messages = lmsgs[s][d] * ((float)rsd / (float)origsd);
                else
                    tandem_messages = (float)0;
                lreqs[s][h] += rsd;
                lreqs[h][d] += rsd;
                lmsgs[s][h] += tandem_messages;
                lmsgs[h][d] += tandem_messages;
				Network::nodes[h].mps += tandem_messages;
                if (origds > 0 )
                    tandem_messages = lmsgs[s][d] * ((float)rds / (float)origds);
                else
                    tandem_messages = (float)0;
                if (rds > 0) {
                    lreqs[d][h] += rds;
                    lreqs[h][s] += rds;
                    lmsgs[d][h] += tandem_messages;
                    lmsgs[h][s] += tandem_messages;
					Network::nodes[h].mps += tandem_messages;
                } /* endif */
            } /* endif */
        } else {   /* nonbifurcated */
            if ((m && ((float) v / (m * cap)) >= (float)1.0-slack)
                    || ((d == Network::nodes[s].pred_node)||(s == Network::nodes[d].pred_node))){
                mult[s][d] = m;
//                cout << " SELECTED" << endl;
            } else {
//                cout << " NOT SELECTED" << endl;
                lreqs[s][h] += lreqs[s][d];
                lreqs[h][d] += lreqs[s][d];
                lmsgs[s][h] += lmsgs[s][d];
                lmsgs[h][d] += lmsgs[s][d];
				Network::nodes[h].mps += lmsgs[s][d];
                lreqs[d][h] += lreqs[d][s];
                lreqs[h][s] += lreqs[d][s];
                lmsgs[d][h] += lmsgs[d][s];
                lmsgs[h][s] += lmsgs[d][s];
				Network::nodes[h].mps += lmsgs[d][s];
                mult[s][d] = 0;
            } /* endif */
        } /* endif */
        //       if (mult[s][d] > 0)
        //           cout << s << "-" << d << " Mult " << mult[s][d] << endl;
    }

    for (int i = 0; i < nn; i++) {
        for (int j = 0; j < nn; j++) {
//            if (i == j)
//                continue;
            if (mult[i][j] > 0) {
                if (linkmat[i][j] != -1) {
                    arc  = linkmat[i][j];
                    arc2 = linkmat[j][i];
                    edge = Network::arcs[arc].edge;
                } else {
                    s = node_vector[i];
                    d = node_vector[j];
                    edge = Network::addEdge(s, d);
                    arc = Network::addArc("_F", edge, s, d);
                    linkmat[s][d] = arc;
                    arc2 = Network::addArc("_R", edge, d, s);
                    linkmat[d][s] = arc2;
                } /* endif */

				Network::edges[edge].mult = mult[i][j];
				Network::edges[edge].type = link_type;
				Network::edges[edge].cost = Utils::dist(s, d) * mult[i][j];
				Network::edges[edge].hops = hops[i][j];
				Network::edges[edge].Lcolor = Network::edge_colours[link_type];
				Network::arcs[arc].length = Network::arcs[arc2].length
                        = Network::edges[edge].length
                        = 100 + min(hops[i][j] - 1, 6) * 90;

                if (full_duplex_operation) {
					Network::arcs[arc].bps  = lreqs[i][j];
					Network::arcs[arc2].bps = lreqs[j][i];
                } else {
					Network::arcs[arc].bps = Network::arcs[arc2].bps = lreqs[i][j] + lreqs[j][i];
                }
            } else if (mult[i][j] == 0 && mult[j][i] == 0) {
                if (linkmat[i][j] != -1) {  /* Delete a tree link wo traffic */
                    cout << "Deleting tree link " << i << "-" << j << endl;
                    edge = Network::arcs[linkmat[i][j]].edge;
                    arc  = Network::edges[edge].arcs[0];
                    arc2 = Network::edges[edge].arcs[1];
					Network::arcs[arc].arc_delete();
					Network::arcs[arc2].arc_delete();
					Network::edges[edge].edge_delete();
                    linkmat[i][j] = linkmat[j][i] = -1;   /* To make sure we don't remove again */
                } /* endif */
            } /* endif */
        }
    }
    return true;
} // compressMUX

int Mentmux::sequenceMUX() {
	int nn = Network::nodes.size();
	int h, p1, p2, ni, nj;
    int h1, h2;
    static vector<int> node_vector(nn);
    static vector<vector<int> > ndeps; // node dependancies
    static vector<vector<int> > home;  // home for each pair

    ndeps.resize(nn);
    for (int i = 0; i < nn; ++i)
        ndeps[i].resize(nn);

    home.resize(nn);
    for (int i = 0; i < nn; ++i)
        home[i].resize(nn);

    for (int i = 0; i < nn; i++) {
        node_vector[i] = i;
    }

    for (int i = 0; i < nn; i++ ) {
        for (int j = 0; j < nn; j++) {
            ndeps[i][j] = 0;
        }
    }

    for (int i = 0; i < nn-1; i++) {
        for (int j = i + 1; j < nn; j++) {
            p1 = Network::nodes[node_vector[i]].pred_node;
            p2 = Network::nodes[node_vector[j]].pred_node;
            h1 = hops[i][p1] + hops[p1][j];
            h2 = hops[i][p2] + hops[p2][j];
            if (i == p1)
                h = p2;
            else if (j == p2)
                h = p1;
            else if (h1 < h2)
                h = p1;
            else if (h2 < h1)
                h = p2;
            else if (Utils::dist(node_vector[i], p1) + Utils::dist(p1, node_vector[j])
                     <= Utils::dist(node_vector[i], p2) + Utils::dist(p2,node_vector[j]))
                h = p1;
            else
                h = p2;
            home[i][j] = h;
            if ((h != node_vector[i]) && (h != node_vector[j])) {
                if (i < h)
                    ndeps[i][h] += 1;
                else
                    ndeps[h][i] += 1;
                if (j < h)
                    ndeps[j][h] += 1;
                else
                    ndeps[h][j] += 1;
            }
        }
    }

    int nr = 0;
    for (int i = 0; i < nn-1; i++) {
        for (int j = i + 1; j < nn; j++) {
            if (ndeps[i][j] == 0) {
                seq[0][nr] = node_vector[i];
                seq[1][nr] = node_vector[j];
                seq[2][nr++] = home[i][j];
            }
        }
    }

    for (int r = 0; r < (nn * (nn-1)) / 2; r++) {
        ni = seq[0][r];
        nj = seq[1][r];
        h  = seq[2][r];
        if ((ni != h) && (nj != h)) {
            if(ni < h) {
                ndeps[ni][h] -= 1;
                if (ndeps[ni][h] == 0) {
                    seq[0][nr] = ni;
                    seq[1][nr] = h;
                    seq[2][nr++] = home[ni][h];
                }
            }
            if (h < ni) {
                ndeps[h][ni] -= 1;
                if (ndeps[h][ni] == 0) {
                    seq[0][nr] = h;
                    seq[1][nr] = ni;
                    seq[2][nr++] = home[h][ni] ;
                }
            }
            if (nj < h) {
                ndeps[nj][h] -= 1;
                if (ndeps[nj][h] == 0) {
                    seq[0][nr] = nj;
                    seq[1][nr] = h;
                    seq[2][nr++] = home[nj][h];
                }
            }
            if (h < nj) {
                ndeps[h][nj] -= 1;
                if (ndeps[h][nj] == 0) {
                    seq[0][nr] = h;
                    seq[1][nr] = nj;
                    seq[2][nr++] = home[h][nj];
                }
            }
        }
    }
    return nr;
} // sequenceMUX

int Mentmux::select_node_model() {
	int nn = Network::nodes.size();
	int best_cost;
    int l;
    int best_node_type;
    vector<int> deg_LS;
    vector<int> deg_T1;
    vector<int> deg_T3;
    int total_node_cost;
    int biggest_box;
    int devtype;
    int edge;
    int node;
    int most_cost;
    int first_box;  /* RC111094 */
    string box_utilization_string;
    float box_utilization;
    string design_type_string;
    string box_type_string;
    string::size_type sz;     // alias of size_t

    design_type_string = Parm::getParm("DESIGN_TYPE");
    box_type_string = Parm::getParm("BOX_TYPE");
    if (box_type_string == "") {
        cout << "Select_note_model() warning. The parm BOX_TYPE is not present. Setting it to GENERIC" << endl;
        box_type_string = "GENERIC";
    } /* endif */

    box_utilization_string = Parm::getParm("BOX_UTILIZATION");
    if (box_utilization_string == "") {
        box_utilization = (float)0.5;
    } else {
        box_utilization = stof(box_utilization_string, &sz);
        if (box_utilization <= 0.0 || box_utilization >= 1.0) {
            cout << "Select_node_model() failed. BOX_UTIL in parms is out of range." << endl;
        } /* endif */
    } /* endif */

    most_cost = 0;
    first_box = -1;
    for (int i = 0; i < Equipment::ndev; i++) {
        if (box_type_string != Equipment::pEquip[i].function) // ignore equipment not for this design
            continue;
        if (Equipment::pEquip[i].cost > most_cost) {
            most_cost = Equipment::pEquip[i].cost;
            biggest_box = i;
        } /* endif */
        if (first_box == -1)
            first_box = i;
    } /*endforeach */

    if (first_box == -1) {
        cout << "Select_node_model() failed. Could find no equipment for the design type." << endl;
        return -1;
    } /* endif */

    if (biggest_box == -1) {
        cout << "Select_node_model() warning. All nodes cost 0. The output of node sizing is suspect." << endl;
        biggest_box = first_box;
    } /* endif */

    deg_LS.resize(nn);
    deg_T1.resize(nn);
    deg_T3.resize(nn);

    for (int i = 0; i < nn; ++i) {
        deg_LS[i] = 0;
        deg_T1[i] = 0;
        deg_T3[i] = 0;
    }

	int ne = Network::edges.size();
    for (int i = 0; i < ne; i++) {
        edge = Network::edges[i].edgenum;
        if (edge == -1)
            continue; // deleted link
        if(Linetype::pLinetype[Network::edges[edge].type].type == "T3") {
            deg_T3[Network::edges[edge].ends[0]] += Network::edges[edge].mult;
            deg_T3[Network::edges[edge].ends[1]] += Network::edges[edge].mult;
        } else if(Linetype::pLinetype[Network::edges[edge].type].type == "T1") {
            deg_T1[Network::edges[edge].ends[0]] += Network::edges[edge].mult;
            deg_T1[Network::edges[edge].ends[1]] += Network::edges[edge].mult;
        } else if(Linetype::pLinetype[Network::edges[edge].type].type == "LS") {
            deg_LS[Network::edges[edge].ends[0]] += Network::edges[edge].mult;
            deg_LS[Network::edges[edge].ends[1]] += Network::edges[edge].mult;
        } else {
            cout << "Select_node_model() failed. An edge type is not LS, T1, or T3." << endl;
            return -1;
        } /* endif */
    } // endfor

    total_node_cost = 0;
    for (int n = 0; n < nn; n++) {
        node = n;
        best_cost = 2000000;  /* More than the biggest box costs */
        for (int d = 0; d < Equipment::ndev; d++) {
            devtype = d;
            if (box_type_string != Equipment::pEquip[devtype].function)
                continue;
            if (deg_LS[n] > Equipment::pEquip[devtype].max_adapt_ls)
                continue;
            if (deg_T1[n] > Equipment::pEquip[devtype].max_adapt_t1)
                continue;
            if (deg_T3[n] > Equipment::pEquip[devtype].max_adapt_t3)
                continue;
            if (total_utilization(node, devtype) > box_utilization)
                continue;
            l = Equipment::pEquip[devtype].cost + deg_LS[n] * Equipment::pEquip[devtype].adapt_cst +
                    deg_T1[n] * Equipment::pEquip[devtype].adapt_cst +
                    deg_T3[n] * Equipment::pEquip[devtype].adapt_cst;
            if (l < best_cost) {
                best_cost = l;
                best_node_type = d;
            }
        } /* endfor */


        if (best_cost == 2000000)
			Network::nodes[node].device = biggest_box;
        else
			Network::nodes[node].device = best_node_type;
		Network::nodes[node].nadapt_ls = deg_LS[n];
		Network::nodes[node].nadapt_t1 = deg_T1[n];
		Network::nodes[node].nadapt_t3 = deg_T3[n];
		Network::nodes[node].cost = Equipment::pEquip[Network::nodes[node].device].cost +
			Network::nodes[node].nadapt_ls * Equipment::pEquip[Network::nodes[node].device].adapt_cst +
			Network::nodes[node].nadapt_t1 * Equipment::pEquip[Network::nodes[node].device].adapt_cst +
			Network::nodes[node].nadapt_t3 * Equipment::pEquip[Network::nodes[node].device].adapt_cst;
        devtype = Network::nodes[node].device;
		Network::nodes[node].utilization = total_utilization(node, devtype);
        total_node_cost += Network::nodes[node].cost;
    } // endfor

    return total_node_cost;
} // select_node_model

double Mentmux::total_utilization(int node, int devtype)
{
    double  rho, svc_time;

    rho  = 0.0;     /*  Accumulating node utilization               */
    if (Equipment::pEquip[devtype].mps_cap > 0)
        svc_time = 1.0 / Equipment::pEquip[devtype].mps_cap;
    else
        svc_time = 9999.0;
    rho += Network::nodes[node].mps  * svc_time;
    return rho;
} // total_utilization

void Mentmux::resize_links()
{
    vector<int> spare_degree_LS;
    vector<int> spare_degree_T1;
    vector<int> spare_degree_T3;
    //   int j;
    int m;
    int c;
    int cost;
    int e0, e1;
    int best_type;
    int best_mult;
    int sd0_LS,sd1_LS;
    int sd0_T1,sd1_T1;
    int sd0_T3,sd1_T3;
    //   PELEM elem;
    Node *node;
    int edge;
    Equipment *device;
	int nn = Network::nodes.size();
	int arc0, arc1;
    int flow;
    float utilization;
    string one_speed_bkbn;
    string::size_type sz;     // alias of size_t

    one_speed_bkbn = Parm::getParm("ONE_SPEED_BKBN");  /*RC070694 */
    if (one_speed_bkbn == "")
        cout << "Resize_links() warning. The parm ONE_SPEED_BKBN is not set." << endl;

	spare_degree_LS.resize(nn);
    spare_degree_T1.resize(nn);
    spare_degree_T3.resize(nn);

    utilization = stof(Parm::getParm("UTILIZATION"), &sz);

    /* Notice that we aren't trying to get the best order on the links but   */
    /* we are trying to meet the degree constraint                           */
    /* All links are full duplex.                                            */

	for (int i = 0; i < nn; i++) {
        node = &Network::nodes[i];
        device = &Equipment::pEquip[node->device];
        spare_degree_LS[i] = device->max_adapt_ls;
        spare_degree_T1[i] = device->max_adapt_t1;
        spare_degree_T3[i] = device->max_adapt_t3;
    }

	int ne = Network::edges.size();
	for (int i = 0; i < ne; i++) {
        edge = i;
        if (Network::edges[edge].edgenum == -1)
            continue; // this edge has been deleted
        e0 = Network::edges[edge].ends[0];
        e1 = Network::edges[edge].ends[1];
        if (Linetype::pLinetype[Network::edges[edge].type].type == "LS") {
            spare_degree_LS[e0] -= Network::edges[edge].mult;
            spare_degree_LS[e1] -= Network::edges[edge].mult;
        } else if (Linetype::pLinetype[Network::edges[edge].type].type == "T1") {
            spare_degree_T1[e0] -= Network::edges[edge].mult;
            spare_degree_T1[e1] -= Network::edges[edge].mult;
        } else if (Linetype::pLinetype[Network::edges[edge].type].type == "T3") {
            spare_degree_T3[e0] -= Network::edges[edge].mult;
            spare_degree_T3[e1] -= Network::edges[edge].mult;
        } else {
            cout << "Resize_links() failed. Bad medium type for edge." << endl;
            return;
        } /* endif */
    } /* endforeach */


    for (int i = 0; i < nn; i++) {
        if (spare_degree_T1[i] < 0 ||
                spare_degree_T3[i] < 0 ||
                spare_degree_LS[i] < 0) {
            cout << "Resize_links() failed. Spare degree is negative on entry." << endl;
            return;
        } /* endif */
    } /* endforeach */

	for (int i = 0; i < ne; i++) {
        edge = i;
        if (Network::edges[edge].edgenum == -1)
            continue; // this edge has been deleted
        e0 = Network::edges[edge].ends[0];
        e1 = Network::edges[edge].ends[1];
        if (Network::nodes[e0].bkbn == 1 && Network::nodes[e1].bkbn == 1 && one_speed_bkbn == "TRUE")                       /*RC070694 */
            continue;
        sd0_T3 = spare_degree_T3[e0];
        sd1_T3 = spare_degree_T3[e1];
        sd0_T1 = spare_degree_T1[e0];
        sd1_T1 = spare_degree_T1[e1];
        sd0_LS = spare_degree_LS[e0];
        sd1_LS = spare_degree_LS[e1];

        if (Linetype::pLinetype[Network::edges[edge].type].type == "LS") {
            sd0_LS += Network::edges[edge].mult;
            sd1_LS += Network::edges[edge].mult;
        } else if (Linetype::pLinetype[Network::edges[edge].type].type == "T3") {
            sd0_T3 += Network::edges[edge].mult;
            sd1_T3 += Network::edges[edge].mult;
        } else {
            sd0_T1 += Network::edges[edge].mult;
            sd1_T1 += Network::edges[edge].mult;
        }

        best_type = Network::edges[edge].type;
        best_mult = Network::edges[edge].mult;
        //      cost = Line::cost[best_type][e0][e1] * best_mult;
        cost = Utils::line_cost(best_type, e0, e1) * best_mult;
        arc0 = linkmat[e0][e1];
        arc1 = linkmat[e1][e0];
        flow = max(Network::arcs[arc0].bps, Network::arcs[arc1].bps);

        for(int j = 0; j < Linetype::ncap; ++j) {
            m = (int)ceil((double)flow / (Linetype::pLinetype[j].speedin * utilization));
            if (m == 0)  /* Never get rid of a link */
                m = 1;
            //         c = Line::cost[j][e0][e1]; // <==========
            c = Utils::line_cost(j, e0, e1);
            if (m * c >= cost)
                continue;
            if ((Linetype::pLinetype[j].type) == "T3" && (m > sd0_T3 || m > sd1_T3))
                continue;
            if ((Linetype::pLinetype[j].type) == "T1" && (m > sd0_T1 || m > sd1_T1))
                continue;
            if ((Linetype::pLinetype[j].type) == "LS" && (m > sd0_LS || m > sd1_LS))
                continue;
            cost = m * c;
            best_type = j;        /* Set the link type */
            best_mult = m;        /* Set the multiplicity */
        } /* endfor */
        if (Linetype::pLinetype[Network::edges[edge].type].type == "T3") {
            spare_degree_T3[e0] += Network::edges[edge].mult;
            spare_degree_T3[e1] += Network::edges[edge].mult;
        } else if (Linetype::pLinetype[Network::edges[edge].type].type == "T1") {
            spare_degree_T1[e0] += Network::edges[edge].mult;
            spare_degree_T1[e1] += Network::edges[edge].mult;
        } else if (Linetype::pLinetype[Network::edges[edge].type].type == "LS") {
            spare_degree_LS[e0] += Network::edges[edge].mult;
            spare_degree_LS[e1] += Network::edges[edge].mult;
        } /* endif */

        if (Linetype::pLinetype[best_type].type == "T3") {
            spare_degree_T3[e0] -= best_mult;
            spare_degree_T3[e1] -= best_mult;
        } else if (Linetype::pLinetype[best_type].type == "T1") {
            spare_degree_T1[e0] -= best_mult;
            spare_degree_T1[e1] -= best_mult;
        } else if (Linetype::pLinetype[best_type].type == "LS") {
            spare_degree_LS[e0] -= best_mult;
            spare_degree_LS[e1] -= best_mult;
        }

        if (spare_degree_T3[e0] < 0 || spare_degree_T3[e1] < 0 ||
                spare_degree_T1[e0] < 0 || spare_degree_T1[e1] < 0 ||
                spare_degree_LS[e0] < 0 || spare_degree_LS[e1] < 0 ) {
            cout << "Resize_links() failed. Spare degree has gone negative" << endl;
            return;
        } /* endif */

		Network::edges[edge].type = best_type;        /* Set the link type */
		Network::edges[edge].Lcolor = Network::edge_colours[best_type];  /* Set the link colour */
		Network::edges[edge].mult = best_mult;        /* Set the multiplicity */
		Network::edges[edge].cost = cost;
		Network::edges[edge].cap  = best_mult * Linetype::pLinetype[best_type].speedin;

		Network::arcs[Network::edges[edge].arcs[0]].utilization = Network::arcs[Network::edges[edge].arcs[0]].bps / Network::edges[edge].cap;
		Network::arcs[Network::edges[edge].arcs[1]].utilization = Network::arcs[Network::edges[edge].arcs[1]].bps / Network::edges[edge].cap;
    } /* endfor */
} // resize_links

int Mentmux::total_link_cost()
{
    int cost;

    cost = 0;

	int ne = Network::edges.size();
	for (int i = 0; i < ne; i++) {
        if (Network::edges[i].edgenum == -1)
            continue; // this edge has been deleted
        cost += Network::edges[i].cost;
    }
    return cost;
} // total_link_cost
