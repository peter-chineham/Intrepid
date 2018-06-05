/*
 * Vdict.h
 *
 *  Created on: 1 May 2016
 *      Author: vinntec
 */

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Dictionary {
public:
	static Dictionary *pDdict;      // pointer used for DESDICT
	static Dictionary *pGdict;      // pointer used for GENDICT
	static Dictionary *pPdict;      // pointer used for PRMDICT
	static Dictionary *pVdict;      // pointer used for VDICT

    string m_dictionary;       // dictionary name
	int m_count;               // number of entries in this dictionary

    vector<string> m_variable; // Name of entry e.g. NAME
    vector<string> m_table;    // Table name e.g. LOCATIONS
    vector<char>   m_type;	   // Type of entry e.g. X = string
    vector<char>   m_role;     // Role * mandatory, + optional, d defaultable
    vector<string> m_default;  // If role = d then value to be used if blank;
    vector<int>    m_min;      // minimum value allowed
    vector<int>    m_max;      // maximum value allowed
    vector<string> m_varref;   // Variable reference
    vector<string> m_tabref;   // Table reference
    vector<bool>   m_present;  // Is this variable and table present in the input?

    Dictionary(string name);
	virtual ~Dictionary();
	void writeDict();
    void addEntry(string, string, char, char, string, int, int, string, string);
    int findEntry(string table, string variable);
};

// Table Processing Instructions built from table records in dictionary
// e.g. %LOCATIONS M W U
class Instructions {
public:
    static int i_count;              // number of records
    static vector<string>i_table;    // name of table
    static vector<char>i_mandatory;  // Mandatory, Optional, Forbidden
    static vector<char>i_write;      // Write to output,  No write to output
    static vector<char>i_unique;     // Unique, Several of same name allowed
    static vector<bool>i_present;    // Is this table present in input?

    static void addInstruction(string i_table, char i_mandatory, char i_write, char i_unique);
	static void writeInstructions();
    static int  checkTable(string table_type);
};

#endif /* DICTIONARY_H_ */
