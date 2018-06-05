/*
 * Reader2.h
 *
 *  Created on: 4 May 2016
 *      Author: vinntec
 */

#ifndef READER2_H_
#define READER2_H_

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <vector>

using namespace std;

class Reader2 {
public:
    static vector<int> tvix;        // TABLE VARIABLE INDEX
    static vector<int> ref_var;     //
    static vector<int> ref_table;   //
    static vector<int> vcol;        // COLUMN START POSITION
    static vector<int> vwidth;      // ELEMENT WIDTH
    static int ntvar;               // NUMBER OF TABLE VARIABLES
    static void expand();

    static vector<int> p_tvix;      // PARM VARIABLE INDEX
    static vector<int> p_vcol;      // COLUMN START POSITION
    static vector<int> p_vwidth;    // ELEMENT WIDTH
    static int p_ntvar;             // NUMBER OF TABLE VARIABLES
    static void p_expand();

    static bool readDictionary(string dic, string dir);
    static bool readInp(string f);
    static bool readStdPrm(string f); // read standard parameter file
    static bool parse_header(char dic, string buf, string tname);
    static bool vartabnum(string table_type, string table_name, string var_name, int fatal,
			int &table_number, int &var_number);
    static int findRecord(char dic, string table_type, string field);

};

#endif /* READER2_H_ */
