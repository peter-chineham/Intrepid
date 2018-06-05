
#include "Node.h"

#include "Utils.h"

Node::Node() {
    //cout << "Node constructor called" << endl;

    this->nodenum = -1;
    this->name = "Undefined";
    this->ntypei = 'U';
    this->vcord = -999;
    this->hcord = -999;
    this->latitude = -999;
    this->longitude = -999;
    this->idd = -1;
    this->local_attach = -1;
    this->service = -1;
    this->parent = "";

    this->ntyped = 'U';
    this->screen.setX(-1);
    this->screen.setY(-1);
    this->bkbn = 0;
    this->pred_arc = -1;
    this->pred_node = -1;
    this->merit = 0.0;
    this->weight = 0;
    this->dcent = -1;
    this->moment = -1.0;
    this->mps = -1;
    this->adj_edges.resize(0);
    this->adj_arcs.resize(0);
    this->inadj_arcs.resize(0);
}

Node::~Node() {
    //cout << "Node destructor called" << endl;
}

void Node::setNode(int nodenum, string name, string ntypei, string vcord, string hcord, 
	string latitude, string longitude, string idd, string local_attach, string service, 
	string parent) {
    
    string::size_type sz;     // alias of size_t

	this->nodenum = nodenum;
    this->name   = Utils::trim(name);
    this->ntypei = Utils::trim(ntypei)[0];
    
    // I probably don't know status of LLVH parameter yet
    if (vcord != "") {
        this->vcord = stoi(vcord, &sz);   // y
    }
    if (hcord != "") {
        this->hcord = -stoi(hcord, &sz); // reverse x
    }
    if (latitude != "") {
        this->latitude = latToY(latitude);    // y
    }
    if (longitude != "") {
        this->longitude = lonToX(longitude);  // x
    }
    this->idd = stoi(idd, &sz);
    this->local_attach = stoi(local_attach, &sz);
    this->service = stod(service, &sz);
    this->parent = Utils::trim(parent);
    this->ntyped = this->ntypei;
}

float Node::lonToX(string lon) {
    string ddd, mmm;
    int i;
    float result;
    float d, m;
    char direction = 'X';
    string::size_type sz;     // alias of size_t

    i = lon.find('E');
    if (i != -1) {
        direction = 'E';
    }
    else {
        i = lon.find('W');
        if (i != -1) {
            direction = 'W';
        }
        else {
            result = -999;  // invalid direction
            return result;
        }
    }
    ddd = lon.substr(0, i);
    mmm = lon.substr(i + 1, lon.length() - i - 1);
    d = stof(ddd, &sz);
    m = stof(mmm, &sz);

    if (direction == 'E') {
        result = round(d * 60 + m);
    }
    else if (direction == 'W') {
        result = round(-(d * 60 + m));
    }
    return result;
}

float Node::latToY(string lat) {
    string ddd, mmm;
    int i;
    float result;
    float d, m;
    char direction = 'X';
    string::size_type sz;     // alias of size_t

    i = lat.find('N');
    if (i != -1) {
        direction = 'N';
    }
    else {
        i = lat.find('S');
        if (i != -1) {
            direction = 'S';
        }
        else {
            result = -1;  // invalid direction
            return result;
        }
    }
    ddd = lat.substr(0, i);
    mmm = lat.substr(i + 1, lat.length() - i - 1);
    d = stof(ddd, &sz);
    m = stof(mmm, &sz);

    if (direction == 'N') {
        result = round((90 * 60) - (d * 60 + m));
    }
    else if (direction == 'S') {
        result = round((90 * 60) + (d * 60 + m));
    }
    return result;
}
