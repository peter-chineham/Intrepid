/*
 * Parm.cpp
 *
 *  Created on: 17 Apr 2016
 *      Author: vinntec
 */

#include "Parm.h"

#include "Utils.h"

int Parm::nParm = 0;
vector<Parm> Parm::pParm; // Initialise parameter vector

void Parm::addParm(string parm, string type, string category, string range, string deflt) {
    pParm.resize(nParm + 1);
    pParm[nParm++].setParm(parm, type, category, range, deflt);
}

string Parm::getParm(string parm) {
    for (int i = 0; i < nParm; i++) {
        if (parm == pParm[i].parm) {
            return pParm[i].value;
        }
    }
    return "";
}

bool Parm::changeParm(string parm, string value) {
    parm  = Utils::trim(parm);
    value = Utils::trim(value);
    for (int i = 0; i < nParm; i++) {
        if (parm == pParm[i].parm) {
            pParm[i].value = value;
            cout << "Parameter " << parm << " changed to " << value << endl;
            return true;
        }
    }
    cout << "Parameter " << parm << " not recognised." << endl;
    return false;
}

Parm::Parm() {
    //cout << "Parm constructor called." << endl;
    this->parm = "Undefined";
    this->type = ' ';
    this->category = ' ';
    this->range = "";
    this->deflt = "";
    this->value = "";
}

Parm::~Parm() {
    //cout << "Parm destructor called." << endl;
}

void Parm::setParm(string parm, string type, string category, string range, string deflt) {
    this->parm     = Utils::trim(parm);
    this->type     = Utils::trim(type)[0];
    this->category = Utils::trim(category)[0];
    this->range    = Utils::trim(range);
    this->deflt    = Utils::trim(deflt);
    this->value    = this->deflt;
}
