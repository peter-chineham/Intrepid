
#include "Requirements.h"

#include "Network.h"
#include "Parm.h"
#include "Utils.h"

//vector<vector<Req> > Req::req; // Initialise requirements 2D vector

//bool Req::addReq(string reqid, string source, string dest, string msg_bytes, string msg_bits,
//    string msg_rate, string bandwidth) {
//
//    source = Utils::trim(source);
//    int s = Network::findNode(source);
//    if (s == -1) {
//        cout << "addReq: SOURCE did not resolve " << source << endl;
//        return false;
//    }
//
//    dest = Utils::trim(dest);
//    int d = Network::findNode(dest);
//    if (d == -1) {
//        cout << "addReq: DEST did not resolve " << dest << endl;
//        return false;
//    }
//
//    if (req[s][d].setReq(reqid, s, d, msg_bytes, msg_bits, msg_rate, bandwidth)) {
//        return true;
//    }
//    else {
//        return false;
//    }
//}

//bool Req::defineReq(int nn) {
//    // Set size of requirements vector (nn x nn)
//    req.resize(nn);
//    for (int i = 0; i < nn; ++i)
//        req[i].resize(nn);
//    return true;
//}

Req::Req() {
    //cout << "Requirement constructor called" << endl;
    this->reqid = "undefined";
    this->ends[0] = 0;
    this->ends[1] = 0;
    this->mps = 0.0;
    this->bps = 0.0;
}

Req::~Req() {
    //cout << "Requirement destructor called" << endl;
}

bool Req::setReq(string reqid, int s, int d, string msg_bytes, 
    string msg_bits, string msg_rate, string bandwidth) {

    string::size_type sz;     // alias of size_t

    reqid = Utils::trim(reqid);
    if (reqid != "")
        this->reqid = reqid;
    else
        this->reqid = "REQ";
    
    this->ends[0] = &Network::nodes[s];
    this->ends[1] = &Network::nodes[d];

    bandwidth = Utils::trim(bandwidth);
    msg_bits = Utils::trim(msg_bits);
    msg_rate = Utils::trim(msg_rate);
    msg_bytes = Utils::trim(msg_bytes);
    if (bandwidth != "") {
        this->bps = stod(bandwidth, &sz);
        this->mps = this->bps / stoi(Parm::getParm("MESSAGE_LEN"), &sz);
    } else if (msg_bits != "" && msg_rate != "") {
        double o_msgbits = stod(msg_bits, &sz);
        double o_msgrate = stod(msg_rate, &sz);
        this->bps = o_msgrate * o_msgbits / 3600.0;
        this->mps = o_msgrate / 3600.0;
    } else if (msg_bytes != "" && msg_rate != "") {
        double o_msgbytes = stod(msg_bytes, &sz);
        double o_msgrate = stod(msg_rate, &sz);
        this->bps = o_msgrate * o_msgbytes * 8 / 3600.0;
        this->mps = o_msgrate / 3600.0;
    } else {
        cout << "setReq failed. Reqs must be BANDWIDTH or (MSG_RATE and (MSG_BITS or MSG_BYTES)." << endl;
        return false;
    } /* endif */
    return true;
}
