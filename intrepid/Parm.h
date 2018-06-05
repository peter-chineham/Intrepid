/*
 * Parm.h
 *
 *  Created on: 17 Apr 2016
 *      Author: vinntec
 */

#ifndef PARM_H_
#define PARM_H_

#include <iostream>
#include <string>
#include <vector>

class Utils;

using namespace std;

class Parm {
public:
    static int nParm;          // number of parameters
    static vector<Parm> pParm; // vector of parameters
    static void addParm(string parm, string type, string category, string range, string deflt);
    static string getParm(string parm);
    static bool changeParm(string parm, string value);

    string value;
    string parm;
    char   type;
    char   category;
    string range;
    string deflt;
    void setParm(string parm, string type, string category, string range, string deflt);

    Parm();
    ~Parm();
};

#endif /* PARM_H_ */
