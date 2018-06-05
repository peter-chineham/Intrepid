
#include "linetype.h"

#include "Utils.h"

int Linetype::ncap = 0;               // Initialise number of line capacities
vector<Linetype> Linetype::pLinetype; // Initialise linetype vector

void Linetype::addLinetype(string svty, string speedin, string duplexity, string type,
    string fixed_cost, string dist_cost1, string dist_cost2, string dist1, string rel, string channels) {

    pLinetype.resize(ncap + 1);
    Linetype::pLinetype[ncap++].setLinetype(svty, speedin, duplexity, type, fixed_cost, dist_cost1,
        dist_cost2, dist1, rel, channels);

}

Linetype::Linetype() {
    //cout << "Linetype constructor called" << endl;
    this->svty = "unassigned";
    this->speedin = -1;
    this->duplexity = 'X';
    this->type = "unassigned";
    this->fixed_cost = -1;
    this->dist_cost1 = -1;
    this->dist_cost2 = -1;
    this->dist1 = -1;
    this->rel = -1;
    this->channels = -1;
}

Linetype::~Linetype() {
    //cout << "Linetype destructor called" << endl;
}

void Linetype::setLinetype(string svty, string speedin, string duplexity, string type,
    string fixed_cost, string dist_cost1, string dist_cost2, string dist1, string rel, string channels) {

    string::size_type sz;     // alias of size_t

    this->svty       = Utils::trim(svty);
    this->speedin    = stof(Utils::trim(speedin), &sz);
    this->duplexity  = Utils::trim(duplexity)[0];
    this->type       = Utils::trim(type);
    this->fixed_cost = stof(Utils::trim(fixed_cost), &sz);
    this->dist_cost1 = stof(Utils::trim(dist_cost1), &sz);
    this->dist_cost2 = stof(Utils::trim(dist_cost2), &sz);
    this->dist1      = stof(Utils::trim(dist1), &sz);
    this->rel        = stof(Utils::trim(rel), &sz);
    this->channels   = stoi(Utils::trim(channels), &sz);
}
