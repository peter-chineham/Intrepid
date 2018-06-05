
#include "Equipment.h"

#include "Parm.h"
#include "Utils.h"

int Equipment::ndev = 0;             // Initialise number of device types
vector<Equipment> Equipment::pEquip; // Initialise device type vector

void Equipment::addEquip(string id, string function, string mps_cap, string cost, string adapt_cst,
                    string max_links_ls, string max_links_t1, string max_links_t3, string chan_delay)
{
    pEquip.resize(ndev + 1);
    Equipment::pEquip[ndev++].setEquip(id, function, mps_cap, cost, adapt_cst, max_links_ls, max_links_t1, max_links_t3, chan_delay);
}

Equipment::Equipment()
{
//    cout << "Equipment constructor called" << endl;
    this->id = "";
    this->function = "";
    this->mps_cap = (float) -1;
    this->bps_cap = (float) -1;
    this->cost = -1;
    this->adapt_cst = -1;
    this->max_adapt_ls = 0;
    this->max_adapt_t1 = 0;
    this->max_adapt_t3 = 0;
    this->chan_delay = (float) -1;
}

Equipment::~Equipment()
{
//    cout << "Equipment destructor called" << endl;
}

void Equipment::setEquip(string id, string function, string mps_cap, string cost, string adapt_cst, string max_links_ls, string max_links_t1, string max_links_t3, string chan_delay)
{
    string::size_type sz;     // alias of size_t

    this->id           = Utils::trim(id);
    this->function     = Utils::trim(function);
    this->mps_cap      = stof(Utils::trim(mps_cap), &sz);
    this->bps_cap      = this->mps_cap * stoi(Utils::trim(Parm::getParm("MESSAGE_LEN")), &sz);
    this->cost         = stoi(Utils::trim(cost), &sz);
    this->adapt_cst    = stoi(Utils::trim(adapt_cst), &sz);
    this->max_adapt_ls = stoi(Utils::trim(max_links_ls), &sz);
    this->max_adapt_t1 = stoi(Utils::trim(max_links_t1), &sz);
    this->max_adapt_t3 = stoi(Utils::trim(max_links_t3), &sz);
    if (chan_delay != "") {
        this->chan_delay   = stof(Utils::trim(chan_delay), &sz);
    } else {
        this->chan_delay = stof(Utils::trim(Parm::getParm("CHANNEL_DELAY")), &sz);
    }
}

