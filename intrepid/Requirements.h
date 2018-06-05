#ifndef REQUIREMENTS_H_
#define REQUIREMENTS_H_

#include <iostream>
#include <vector>

using namespace std;

class Node;

class Req {
public:
    //static vector<vector<Req> > req; // vector of 2D requirements
    //static bool defineReq(int nn);
    //static bool addReq(string reqid, string source, string dest, string msg_bytes, string msg_bits,
    //    string msg_rate, string bandwidth);

    string reqid; // req ID
    Node *ends[2];  // Source , destination node number
    double bps;   // bits/sec
    double mps;   // msg/sec
    
    Req();  // constructor
    ~Req(); // destructor
    bool setReq(string reqid, int s, int d, string msg_bytes, string msg_bits,
        string msg_rate, string bandwidth);
    
};

#endif /* REQUIREMENTS_H_ */
