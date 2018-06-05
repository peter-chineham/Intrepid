/*
 * utils.h
 *
 *  Created on: 15 Apr 2016
 *      Author: vinntec
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <string>
#include <vector>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <math.h>

class Edge;
class Equipment;
class Linetype;
class Node;
class Parm;

#define IINFINITY 9999
#define LINFINITY 999999999
#define FINFINITY (float)1.0e10

using namespace std;

class Utils {
public:
    static int    **Alloc2Dint(int, int);
    static float  **Alloc2Dflo(int, int);
    static void   Free2Darray(int**, int);
    static void   Free2Darray(float**, int);
    static string ltrim(string);
    static string rtrim(string);
    static string trim(string);
    static void   print2Df(float**,  int, int);
    static void   print3Df(float***, int, int, int);
//    static float  MapDistLL (float, float, float, float);
    static double MapDistLL(double lat1, double lon1, double lat2, double lon2);
    static float  VHtoMiles(int, int, int, int);
    static void   killModel();
    static void   tokenise(string, int&, string[]);
    static bool   isNumber(string);
    static int    coordinates_to_distance(int node1, int node2, char vhll);
    static int    dist(int node1, int node2); // return cost of default line type
    static float  line_cost(int link_type, int node1, int node2); // return cost of specified line type
    static float  line_dist(int node1, int node2); // return distance in km between two nodes
    static void   writeReport();
    static void   writeCSVReport();
};

#endif /* UTILS_H_ */
