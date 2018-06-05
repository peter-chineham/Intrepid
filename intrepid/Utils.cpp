/*
 * utils.cpp
 *
 *  Created on: 15 Apr 2016
 *      Author: vinntec
 */

#include "Utils.h"

//#include "arc.h"
//#include "Edge.h"
#include "equipment.h"
#include "linetype.h"
#include "Network.h"
#include "Parm.h"

// trim from start
string Utils::ltrim(string s) {
    size_t len = s.length();
	string o = "";
	for (int i=0; i<len; i++) {
		if (s[i] != ' ') {
			for (int j=i; j<len; j++) {
				o += s[j];
			}
			break;
		};
	}
	return o;
}

// trim from end
string Utils::rtrim(string s) {
    int len = int(s.length());
	string o = "";
    for (int i=len-1; i>=0; i--) {
		if (s[i] != ' ') {
            for (int j=0; j<=i; j++) {
				o += s[j];
			}
			break;
		};
	}
	return o;
}

// trim from both ends
string Utils::trim(string s) {
	s = ltrim(rtrim(s));
	return s;
}

int **Utils::Alloc2Dint(int rows, int columns) {
    int **p;
    p = new int* [rows];
    for(int row = 0; row < rows; row++) {
        p[row] = new int[columns];
    }
    return(p);
}
float **Utils::Alloc2Dflo(int rows, int columns) {
    float **p;
    p = new float* [rows];
    for(int row = 0; row < rows; row++) {
        p[row] = new float[columns];
    }
    return(p);
}

void Utils::Free2Darray(int **base, int columns) {
    for (int i = 0; i < columns; i++) {
        delete(base[i]);
    }
    delete(base);
}
void Utils::Free2Darray(float **base, int columns) {
    for (int i = 0; i < columns; i++) {
        delete(base[i]);
    }
    delete(base);
}

void Utils::print2Df(float **p, int x, int y){
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            cout << p[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void Utils::print3Df(float ***p, int x, int y, int z){
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            for(int k = 0; k < z; k++){
                cout << p[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

float Utils::VHtoMiles( int v1 , int h1 , int v2 , int h2 ) {
    // SQRT(((v1-v2)**2 + (h1-h2)**2) / 10)
    float dv, dh, x;

    dh = (float )(h1 - h2);
    dv = (float )(v1 - v2);
    x = dv * dv + dh * dh;
    x = sqrt( 0.1 * x );
    return x;
}

void Utils::killModel() {
//    Line::kill_dist_cost();
//    Line::killLines();
}

void Utils::tokenise(string str, int &count, string output[]) {

  // construct a stream from the string
  stringstream strstr(str);

  // use stream iterators to copy the stream to the vector as whitespace separated strings
  istream_iterator<string> it(strstr);
  istream_iterator<string> end;
  vector<string> results(it, end);

  // copy to output array and set count
  count = 0;
  for (vector<string>::iterator it = results.begin(); it!=results.end(); ++it) {
      output[count++] = *it;
  }
}

bool Utils::isNumber(string s) {
	bool ok = true;
	if (s == "") {
		ok = false;
		return ok;
	}
	for (unsigned int i = 0; i < s.length(); i++) {
		if (isdigit(s[i]) == false) { // check if 0-9
			if ((s[i] != '-') && (s[i] != '+')) { // check for +/-
				ok = false;
				break;
			}
		}
	}
	return ok;
}

/***********
**  V & H to miles
**  L & L to kilometers
***********/
int Utils::coordinates_to_distance(int node1, int node2, char vhll)
{
    double distL;
    int    dist1;

    if (vhll == 'V') {
        dist1 = VHtoMiles(Network::nodes[node1].vcord, Network::nodes[node1].hcord,
                          Network::nodes[node2].vcord, Network::nodes[node2].hcord);
    }
    else if (vhll == 'L') {
        distL = MapDistLL(Network::nodes[node1].latitude, Network::nodes[node1].longitude,
                                 Network::nodes[node2].latitude, Network::nodes[node2].longitude);
        if (distL < 0.0) {
            cout << "MapDistLL has returned distance between " << Network::nodes[node1].name << " and "
                 << Network::nodes[node2].name << " which is less than zero " << distL << endl;
        }
        dist1 = (int) distL;
    }
    else {
        cout << "Unknown LL_VH parm in coordinates_to_distance. Dist set to 0." << endl;
        dist1 = 0;
    } /* endif */

    return dist1;
}

int Utils::dist(int node1, int node2)
{
    int distance, cost, d1;

//    Node *n1 = &Node::pNode[node1];
//    Node *n2 = &Node::pNode[node2];
    if (Parm::getParm("LLVH") == "VH") {
        /* Piecewise linear; 2 pieces   (AK111993) */
        distance = coordinates_to_distance(node1, node2, 'V');
        if (distance < 0) {
            cout << "dist Warning: Distance less than zero, setting to 0." << endl;
            distance = 0;
        }
        d1 = Linetype::pLinetype[Network::default_link_type].dist1;
        if (distance <= d1) {
            cost = Linetype::pLinetype[Network::default_link_type].fixed_cost +
                Linetype::pLinetype[Network::default_link_type].dist_cost1 * distance;
        }
        else {
            cost = Linetype::pLinetype[Network::default_link_type].fixed_cost +
                Linetype::pLinetype[Network::default_link_type].dist_cost1 * d1 +
                Linetype::pLinetype[Network::default_link_type].dist_cost2 * (distance - d1);
        } /* endif */
    }
    else if (Parm::getParm("LLVH") == "LL") {  
//        if ((n1->latitude == n2->latitude) && (n1->longitude == n2->longitude))
//            return 0;
        /* Piecewise linear; 2 pieces   (AK111993) */
        distance = coordinates_to_distance(node1, node2, 'L');
        if (distance < 0) {
            cout << "dist Warning: Distance less than zero, setting to 0." << endl;
            distance = 0;
        }
        d1 = Linetype::pLinetype[Network::default_link_type].dist1;
        if (distance <= d1) {
            cost = Linetype::pLinetype[Network::default_link_type].fixed_cost +
                Linetype::pLinetype[Network::default_link_type].dist_cost1 * distance;
        }
        else {
            cost = Linetype::pLinetype[Network::default_link_type].fixed_cost +
                Linetype::pLinetype[Network::default_link_type].dist_cost1 * d1 +
                Linetype::pLinetype[Network::default_link_type].dist_cost2 * (distance - d1);
        } /* endif */
    }
    else {
        cout << "dist() failed. LLVH parameter invalid." << endl;
        cost = 0;
    } /* endif */

    return cost;
}

/* -------------- Calculate distance between 2 points --------------------- */
double Utils::MapDistLL(double lat1, double lon1, double lat2, double lon2)
{
   double x1w, y1w, x2w, y2w;
   double worky, work1, work2, work3, work4;
   double distkm;
   const double circumf = 40077.0;    /* earth's circumference along equator */
   const double pi      = 3.14159265; /* pi */
   const double con01   = pi / 180.0; /* to convert deg. to radians */

   x1w = lon1;
   y1w = lat1;
   x2w = lon2;
   y2w = lat2;

   x1w = x1w / 60.0;  /* convert to degrees */
   y1w = y1w / 60.0;
   x2w = x2w / 60.0;
   y2w = y2w / 60.0;

   worky = fabs ( x1w - x2w );
   if ( (360.0 - worky) < worky)
      worky = 360.0 - worky;
   worky = worky * con01;      /* convert to radians */

   work1 = (90.0 - y1w) * con01;
   work2 = (90.0 - y2w) * con01;

   work3 = cos(work2) * cos(work1) + sin(work2) * sin(work1) * cos(worky);
   work4 = pi * 0.5 - asin(work3);

   distkm = max(circumf * work4 / (2 * pi), 0.0);  // allow for tiny distances

   if (isnan(distkm))  // allow for divide by zero for same location
       distkm = 0.0;

   return (distkm);
}

float Utils::line_dist(int node1, int node2)
{
    int distance;

    if (Parm::getParm("LLVH") == "VH") {
        distance = coordinates_to_distance(node1, node2, 'V');
    }
    else if (Parm::getParm("LLVH") == "LL") {
        distance = coordinates_to_distance(node1, node2, 'L');
    }
    else {
        cout << "line_dist() failed. LLVH parameter invalid." << endl;
        distance = -1;
    } /* endif */
    if (distance < 0) {
        cout << "Line_dist Warning: Distance less than zero." << endl;
    }

    return distance;
}

void Utils::writeReport()
{
    // Network formatted report
    ofstream report;
    string filename = "report.txt";
    report.open(filename);
    if (report.is_open()) {
        // Find max length of node names
        int nl = 0;
        for (Node n : Network::nodes) {
            if ((int) n.name.length() > nl)
                nl = int(n.name.length());
        }
        nl = max(4, nl); // Allow room for heading

        // Find max length of equipment
        int el = 0;
        for (Equipment e : Equipment::pEquip) {
            if ((int) e.id.length() > el)
                el = int(e.id.length());
        }
        el = max(5, el); // Allow room for heading

        report << "Nodes" << endl;
        report << setfill('+');
        report << setw(nl) << left << "NAME" << " ";
        report << setw(9)  << left << "NODE_TYPE" << " ";
        report << setw(el) << left << "EQUIP" << " ";
        report << setw(8)  << left << "COST" << " ";
        report << setw(8)  << left << "ADAPT_LS" << " ";
        report << setw(8)  << left << "ADAPT_T1" << " ";
        report << setw(8)  << left << "ADAPT_T3" << " ";
        report << setw(6)  << left << "UTIL" << " ";
        report << endl;
        report << setfill(' ');
        for (Node n : Network::nodes) {
            report << setw(nl) << left  << n.name << " ";
            report << setw(9)  << left  << n.ntyped      << " ";
            report << setw(el) << left  << Equipment::pEquip[n.device].id << " ";
            report << setw(8)  << right << n.cost        << " ";
            report << setw(8)  << right << n.nadapt_ls   << " ";
            report << setw(8)  << right << n.nadapt_t1   << " ";
            report << setw(8)  << right << n.nadapt_t3   << " ";
            report << setprecision(2) << fixed << setw(6) << right << n.utilization << " ";

            report << endl;
        }

        // Find max length of edge names
        el = 0;
        for (Edge e : Network::edges) {
            if ((int) e.id.length() > el)
                el = int(e.id.length());
        }
        el = max(6, el); // Allow room for heading

        // Find max length of service type
        int sl = 0;
        for (Linetype l: Linetype::pLinetype) {
            if ((int) l.svty.length() > sl)
                sl = int(l.svty.length());
        }
        sl = max(7, sl); // Allow room for heading

        report << endl;
        report << "Links" << endl;
        report << setfill('+');
        report << setw(el) << left  << "EDGEID"   << " ";
        report << setw(nl) << left  << "END0"     << " ";
        report << setw(nl) << left  << "END1"     << " ";
        report << setw(sl) << left  << "SERVICE"  << " ";
        report << setw(8)  << left  << "LINE_SPD" << " ";
        report << setw(5)  << left  << "MULT"     << " ";
        report << setw(12)  << left  << "CAPACITY" << " ";
        report << setw(12)  << left  << "MONTHCST" << " ";
        report << setw(8)  << left  << "LENGTH"   << " ";
        report << setw(12)  << left  << "BPS01"    << " ";
        report << setw(12)  << left  << "BPS10"    << " ";
        report << setw(6)  << left  << "UTIL01"   << " ";
        report << setw(6)  << left  << "UTIL10"   << " ";
        report << setw(8)  << left  << "HOPS"     << " ";
        report << setw(8)  << left  << "COS01"    << " ";
        report << setw(8)  << left  << "COS10"    << " ";
        report << endl;
        report << setfill(' ');
        for (Edge e : Network::edges) {
            Arc a = Network::arcs[e.arcs[0]];
            Arc b = Network::arcs[e.arcs[1]];
            Node n1 = Network::nodes[e.ends[0]];
            Node n2 = Network::nodes[e.ends[1]];
            Linetype lt = Linetype::pLinetype[e.type];
            report << setw(el) << left  << e.id    << " ";
            report << setw(nl) << left  << n1.name << " ";
            report << setw(nl) << left  << n2.name << " ";
            report << setw(sl) << left  << lt.svty << " ";
            report << setprecision(0) << fixed << setw(8) << right << lt.speedin << " ";
            report << setw(5)  << right << e.mult  << " ";
            report << setw(12) << right << e.cap   << " ";
            report << setw(12) << right << e.cost  << " ";
            report << setw(8)  << right << e.dist  << " ";
            report << setprecision(0) << fixed << setw(12) << right << a.bps   << " ";
            report << setprecision(0) << fixed << setw(12) << right << b.bps   << " ";
            report << setprecision(2) << fixed << setw(6) << right << a.utilization << " ";
            report << setprecision(2) << fixed << setw(6) << right << b.utilization << " ";
            report << setw(8)  << right << e.hops   << " ";
            report << setprecision(0) << fixed << setw(8) << right << a.length << " ";
            report << setprecision(0) << fixed << setw(8) << right << b.length << " ";
            report << endl;
        }
        report.close();
        cout << "Report complete." << endl;
    } else {
        cout << "writeReport failed. Unable to open " << filename << endl;
    }
}

void Utils::writeCSVReport()
{
    // Network CSV report
    ofstream report;
    string filename = "report.csv";
    report.open(filename);
    if (report.is_open()) {
        report << "NAME"      << ",";
        report << "NODE_TYPE" << ",";
        report << "EQUIP"     << ",";
        report << "COST"      << ",";
        report << "ADAPT_LS"  << ",";
        report << "ADAPT_T1"  << ",";
        report << "ADAPT_T3"  << ",";
        report << "UTIL"      << endl;
        for (Node n : Network::nodes) {
            report << n.name << ",";
            report << n.ntyped << ",";
            report << Equipment::pEquip[n.device].id << ",";
            report << n.cost << ",";
            report << n.nadapt_ls << ",";
            report << n.nadapt_t1 << ",";
            report << n.nadapt_t3 << ",";
            report << n.utilization << endl;
        }
        report << "EDGEID"   << ",";
        report << "END0"     << ",";
        report << "END1"     << ",";
        report << "SERVICE"  << ",";
        report << "LINE_SPD" << ",";
        report << "MULT"     << ",";
        report << "CAPACITY" << ",";
        report << "MONTHCST" << ",";
        report << "LENGTH"   << ",";
        report << "BPS01"    << ",";
        report << "BPS10"    << ",";
        report << "UTIL01"   << ",";
        report << "UTIL10"   << ",";
        report << "HOPS"     << ",";
        report << "COS01"    << ",";
        report << "COS10"    << endl;
        for (Edge e : Network::edges) {
            Arc a = Network::arcs[e.arcs[0]];
            Arc b = Network::arcs[e.arcs[1]];
            Node n1 = Network::nodes[e.ends[0]];
            Node n2 = Network::nodes[e.ends[1]];
            Linetype lt = Linetype::pLinetype[e.type];
            report << e.id          << ",";
            report << n1.name       << ",";
            report << n2.name       << ",";
            report << lt.svty       << ",";
            report << lt.speedin    << ",";
            report << e.mult        << ",";
            report << e.cap         << ",";
            report << e.cost        << ",";
            report << e.dist        << ",";
            report << a.bps         << ",";
            report << b.bps         << ",";
            report << a.utilization << ",";
            report << b.utilization << ",";
            report << e.hops        << ",";
            report << a.length      << ",";
            report << b.length      << endl;
        }
        report.close();
        cout << "Report complete." << endl;
    } else {
        cout << "writeReport failed. Unable to open " << filename << endl;
    }
}

float Utils::line_cost(int link_type, int node1, int node2)
{
    int distance, cost, d1;

    distance = line_dist(node1, node2);
    d1 = Linetype::pLinetype[link_type].dist1;
    if (distance <= d1) {
        cost = Linetype::pLinetype[link_type].fixed_cost +
                Linetype::pLinetype[link_type].dist_cost1 * distance;
    }
    else {
        cost = Linetype::pLinetype[link_type].fixed_cost +
                Linetype::pLinetype[link_type].dist_cost1 * d1 +
                Linetype::pLinetype[link_type].dist_cost2 * (distance - d1);
    } /* endif */

    return cost;
}
