/*
 * Vdict.cpp
 *
 *  Created on: 1 May 2016
 *      Author: vinntec
 */

#include "Dictionary.h"

Dictionary *Dictionary::pDdict = new Dictionary("DESDICT");
Dictionary *Dictionary::pGdict = new Dictionary("GENDICT");
Dictionary *Dictionary::pPdict = new Dictionary("PRMDICT");
Dictionary *Dictionary::pVdict = new Dictionary("VDICT");

Dictionary::Dictionary(string name) {
	this->m_dictionary = name;
	this->m_count = 0;
}

Dictionary::~Dictionary() {

}

void Dictionary::writeDict() {
    cout << "Dictionary name: " << this->m_dictionary << endl;
	cout << "Dictionary size: " << this->m_count << endl;
	for (int i = 0; i < this->m_count; i++) {
		cout << i << ": ";
        cout << this->m_variable[i] << " ";
        cout << this->m_table[i]    << " ";
		cout << this->m_type[i]     << " ";
		cout << this->m_role[i]     << " ";
        cout << this->m_default[i]  << " ";
		cout << this->m_min[i]      << " ";
		cout << this->m_max[i]      << " ";
        cout << this->m_varref[i]   << " ";
        cout << this->m_tabref[i]   << " ";
        cout << this->m_present[i]  << endl;
    }
	cout << endl; // add a blank line
}

void Dictionary::addEntry(string m_variable, string m_table,
    char m_type, char m_role, string m_default,
    int m_min, int m_max, string m_varref, string m_tabref) {

    this->m_variable.resize(this->m_count + 1);
    this->m_table.resize(this->m_count + 1);
    this->m_type.resize(this->m_count + 1);
    this->m_role.resize(this->m_count + 1);
    this->m_default.resize(this->m_count + 1);
    this->m_min.resize(this->m_count + 1);
    this->m_max.resize(this->m_count + 1);
    this->m_varref.resize(this->m_count + 1);
    this->m_tabref.resize(this->m_count + 1);
    this->m_present.resize(this->m_count + 1);

    this->m_variable[this->m_count] = m_variable;
    this->m_table[this->m_count] = m_table;
    this->m_type[this->m_count] = m_type;
    this->m_role[this->m_count] = m_role;
    this->m_default[this->m_count] = m_default;
    this->m_min[this->m_count] = m_min;
    this->m_max[this->m_count] = m_max;
    this->m_varref[this->m_count] = m_varref;
    this->m_tabref[this->m_count] = m_tabref;
    this->m_present[this->m_count] = false; // not present yet

    this->m_count++;
}

int Dictionary::findEntry(string table, string variable) {
    for (int i = 0; i < m_count; i++) {
        if ((m_table[i] == table) && (m_variable[i] == variable))
            return i;
    }
    return -1; // not found
}

int Instructions::i_count = 0;
vector<string>Instructions::i_table;
vector<char>Instructions::i_mandatory;
vector<char>Instructions::i_write;
vector<char>Instructions::i_unique;
vector<bool>Instructions::i_present;

void Instructions::addInstruction(string table, char mandatory,
		char write, char unique) {
	i_count++;
	i_table.push_back(table);
	i_mandatory.push_back(mandatory);
	i_write.push_back(write);
	i_unique.push_back(unique);
	i_present.push_back(false);
}

void Instructions::writeInstructions() {
	cout << "Table Instructions" << endl;
	for (int i = 0; i < i_count; i++) {
        cout << i_table[i] << " ";
		cout << i_mandatory[i] << " ";
		cout << i_write[i] << " ";
		cout << i_unique[i] << endl;
	}
	cout << endl; // add a blank line
}
int Instructions::checkTable(string table_type) {
    for (int i = 0; i < i_count; i++) {
        cout << i << " " << i_table[i] << endl;
        if (i_table[i] == table_type) {
            return i; // found
        }
    }
    return -1; // not found
}
