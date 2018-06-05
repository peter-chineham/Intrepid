#include "Network.h"

#include "canvas.h"
#include "Utils.h"
#include "world.h"

vector<Node> Network::nodes;		 // Initialise node vector
vector<Edge> Network::edges;		 // Initialise edge vector
vector<Arc> Network::arcs;			 // initialise arc vector
vector<Req> Network::reqs;	         // Initialise requirements vector
vector<vector<float> > Network::Lreq; // Initialise line requirements 2D vector
vector<Parm> Network::parms;		 // Initialise parameter vector
vector<Equipment> Network::devtypes; // Initialise device type vector
vector<Linetype> Network::linetypes; // Initialise linetype vector
Qt::GlobalColor Network::edge_colours[] = { Qt::darkBlue, Qt::blue, Qt::darkCyan, Qt::cyan,
	Qt::darkGreen, Qt::green, Qt::darkYellow, Qt::yellow,Qt::darkMagenta, Qt::magenta,
	Qt::darkRed, Qt::red };
int Network::default_link_type = 0;

Network::Network()
{
}

Network::~Network()
{
}

void Network::addNode(string name, string ntypei, string vcord, string hcord,
        string latitude, string longitude, string idd, string local_attach,
        string service, string parent) {
	int nn = nodes.size();
	nodes.resize(nn + 1);
	Network::nodes[nn].setNode(nn, name, ntypei, vcord, hcord, latitude, longitude, idd,
		local_attach, service, parent);
}

int Network::findNode(string node) {
	int  o_node = -1;
	int nn = nodes.size();
	for (int i = 0; i < nn; i++) {
		if (nodes[i].name == node) {
			o_node = i;
			break;
		}
	}
	return o_node;
}

void Network::makeWorld()
{
	int nn = nodes.size();
	for (int i = 0; i < nn; i++) {
		if (Parm::getParm("LLVH") == "VH") {
			nodes[i].screen = World::ConvertWorldtoScreen(nodes[i].hcord, nodes[i].vcord,
				Canvas::my_canvas->pv_width, Canvas::my_canvas->pv_height);
		}
		else if (Parm::getParm("LLVH") == "LL") {
			nodes[i].screen = World::ConvertWorldtoScreen(nodes[i].longitude, nodes[i].latitude,
				Canvas::my_canvas->pv_width, Canvas::my_canvas->pv_height);
		}
		else {
			cout << "setNode: Invalid LLVH parameter. Unable to get screen coordinates for node " << nodes[i].name << endl;
		}
	}
}

int Network::addEdge(int node, int node2)
{
	int ne = edges.size();
	edges.resize(ne + 1);
	edges[ne].setEdge(ne, node, node2);
	return ne;
}

int Network::addArc(string prefix, int edge, int node, int node2)
{
	int na = arcs.size();
	arcs.resize(na + 1);
	arcs[na].setArc(na, prefix, edge, node, node2);
	return na;
}

bool Network::defineLreq(int nn) {
    // Set size of line requirements (nn x nn)
    Lreq.resize(nn);
	for (int i = 0; i < nn; i++) {
        Lreq[i].resize(nn);
	}
	for (int i = 0; i < nn; i++) {
		for (int j = 0; j < nn; j++) {
            Lreq[i][j] = 0.0; // Initialise
		}
	}
	return true;
}

bool Network::addReq(string reqid, string source, string dest, string msg_bytes, string msg_bits,
	string msg_rate, string bandwidth) {

	source = Utils::trim(source);
	int s = Network::findNode(source);
	if (s == -1) {
		cout << "addReq: SOURCE did not resolve " << source << endl;
		return false;
	}

	dest = Utils::trim(dest);
	int d = Network::findNode(dest);
	if (d == -1) {
		cout << "addReq: DEST did not resolve " << dest << endl;
		return false;
	}

	int nr = reqs.size();
	reqs.resize(nr + 1);
	if (reqs[nr].setReq(reqid, s, d, msg_bytes, msg_bits, msg_rate, bandwidth)) {
		return true;
	}
	else {
		return false;
    }
}

int Network::getReq(int n1, int n2)
{
    int nr = reqs.size();
    for (int i = 0; i < nr; i++) {
        if ((reqs[i].ends[0]->nodenum == n1) && (reqs[i].ends[1]->nodenum == n2)) {
            return i;
        }
    }
    return -1;
}
