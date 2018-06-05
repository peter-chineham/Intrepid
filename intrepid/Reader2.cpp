/*
 * Reader2.cpp
 *
 *  Created on: 4 May 2016
 *      Author: vinntec
 */

#include "Reader2.h"

#include "Dictionary.h"
#include "equipment.h"
#include "linetype.h"
#include "Network.h"
#include "Parm.h"
#include "Requirements.h"
#include "Utils.h"

int Reader2::ntvar = 0;
vector<int> Reader2::tvix;
vector<int> Reader2::ref_var;
vector<int> Reader2::ref_table;
vector<int> Reader2::vcol;
vector<int> Reader2::vwidth;

int Reader2::p_ntvar = 0;
vector<int> Reader2::p_tvix;
vector<int> Reader2::p_vcol;
vector<int> Reader2::p_vwidth;

void Reader2::expand() {
    ntvar++;
    tvix.resize(ntvar);
    ref_var.resize(ntvar);
    ref_table.resize(ntvar);
    vcol.resize(ntvar);
    vwidth.resize(ntvar);
}

void Reader2::p_expand() {
    p_ntvar++;
    p_tvix.resize(p_ntvar);
    p_vcol.resize(p_ntvar);
    p_vwidth.resize(p_ntvar);
}

bool Reader2::readDictionary(string dic, string dir) {
	// relies on C++11

	ifstream myfile;
	string line;
	int count = 0;
	string words[7];
	Dictionary *dict;

	if (dic == "DESDICT") {
		dict = Dictionary::pDdict;
	} else if (dic == "GENDICT") {
		dict = Dictionary::pGdict;
	} else if (dic == "PRMDICT") {
		dict = Dictionary::pPdict;
	} else if (dic == "VDICT") {
		dict = Dictionary::pVdict;
	}
	myfile.open(dir + dic);
	if (myfile.is_open()) {
		while ( getline (myfile, line) ) {
            if (Utils::trim(line).length() == 0) {
				continue;
			}
			if (line[0] == '*') {
				continue;
			}
			// clear words before reuse
			for (int i = 0; i < 7; i++) {
				words[i] = "";
			}
            Utils::tokenise(line, count, words);
			if (line[0] == '%') { // table record
				words[0].erase(0,1); // remove the %
				char c = words[1][0];
				if (c != 'M' && c != 'O' && c != 'F')
				{
					cout << "ReadDict() failed. Invalid %" << words[0] << " record. The second field must be M or O or F." << endl;;
					return false;
				}       /* endif */
				c = words[2][0];
				if (c != 'N' && c != 'W')
				{
					cout << "ReadDict() failed. Invalid %" << words[0] << " record. The third field must be N or W." << endl;
					return false;
				}       /* endif */
				c = words[3][0];
				if (c != 'S' && c != 'U')
				{
					cout << "ReadDict() failed. Invalid %" << words[0] << " record. The fourth field must be S or U." << endl;
					return false;
				}       /* endif */
				// OK to add the record
				Instructions::addInstruction(words[0], words[1][0],
						words[2][0], words[3][0]);
			} else {
                if (Utils::isNumber(words[5]) && (Utils::isNumber(words[6]))) {
					int w5 = stoi(words[5]);
					int w6 = stoi(words[6]);
					dict->addEntry(words[0], words[1], words[2][0], words[3][0],
							words[4], w5, w6, "", "");
				} else {
					dict->addEntry(words[0], words[1], words[2][0], words[3][0],
							words[4], -1, -1, words[5], words[6]);
				}
			}
		}
		myfile.close();
        return true;
    }
    else {
        cout << "Unable to open dictionary " << dic << endl;
        return false;
    }
}

bool Reader2::readInp(string f) {
    string line;
//    int len;
    char ch;
    string table;
    static string ptable = "";
    string table_type = "";
    string table_name = "";
    string file_name = "";
    int ins_table_index;
    ifstream myfile;
    string val[] = { "", "", "", "", "", "", "", "" , "", "" };
    int ix[] = { -1, -1, -1, -1, -1, -1, -1, -1 , -1, -1 };

    Utils::killModel();

    bool alloc_pending = true;

    // Read input file and load data
    myfile.open(f);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if (Utils::trim(line).length() == 0) {
                continue;
            }
//            len = line.length();
            ch = line[0];
            switch (ch) {
            case '*': // comment
                continue;
                break;
            case '%': // header
                if (line.substr(0, 6) == "%TABLE") {
                    istringstream buf(line);
                    ptable = "";
                    table_type = "";
                    table_name = "";
                    file_name = "";

                    buf >> ptable >> table_type >> table_name >> file_name;
                    if (table_type == "")
                    { // CHECK NUMBER OF STRINGS. IF LESS THAN 2, RETURN ERROR
                        cout << "ReadTable() failed. Bad header record " << line << ". Must be at least two strings." << endl;
                        return false;
                    } // endif

                    // CHECK FOR TABLE IDENTIFIER
                    if (ptable != "%TABLE")
                    { // SEND ERROR AND RETURN IF NOT FOUND
                        cout << "ReadTable() failed. Bad header record " << line << ". Does not start with %TABLE" << endl;
                        return false;
                    } // endif

                    if ((table_name[0] == '"') && (file_name == ""))
                    { // then the third string is filename
                        table_name.substr(1, table_name.length() - 2); // Get rid of ".."
                        file_name += table_name;
                        table_name = "";
                    } // endif

                    ins_table_index = Instructions::checkTable(table_type);
                    if (ins_table_index == -1)
                    {
                        cout << "ReadTable() failed. Table " << table_type << " has no processing instructions." << endl;
                        return false;
                    } // endif
//                    cout << "Processing instructions found at " << ins_table_index << endl;

                    if (Instructions::i_unique[ins_table_index] == 'U')
                    {
                        if (Instructions::i_present[ins_table_index])
                        {
                            cout << "ReadTable() failed. Table " << table_type << " is already present but dictionary specifies that it is unique." << endl;
                            return false;
                        }  // endif
                    } // endif

                    if (Instructions::i_unique[ins_table_index] == 'F')
                    {
                        cout << "ReadTable() failed. Table " << table_type << " is a forbidden table in the dictionary." << endl;
                        return false;
                    } // endif

                    Instructions::i_present[ins_table_index] = true; // mark this table present in input
                }
                else {
                    bool result = parse_header('V', line, table_type);
                    if (!result) {
                        cout << "readInp error: parse_header failed." << endl;
                        return false;
                    }
                }
                if ((table_type != "LOCATIONS") && (alloc_pending)) {
                    alloc_pending = false;
                    Network::defineLreq(Network::nodes.size());
                }
                break;
            case ' ': // data
                if (table_type == "LOCATIONS") {
                    for (int i = 0; i < 10; i++) {
                        val[i] = "";
                        ix[i] = -1;
                    }

                    ix[0] = findRecord('V', table_type, "NAME");
                    ix[1] = findRecord('V', table_type, "TYPE");
                    ix[2] = findRecord('V', table_type, "VCORD");
                    ix[3] = findRecord('V', table_type, "HCORD");
                    ix[4] = findRecord('V', table_type, "LATITUDE");
                    ix[5] = findRecord('V', table_type, "LONGITUDE");
                    ix[6] = findRecord('V', table_type, "IDD");
                    ix[7] = findRecord('V', table_type, "LOCAL_ATTACH");
                    ix[8] = findRecord('V', table_type, "SERVICE");
                    ix[9] = findRecord('V', table_type, "PARENT");
                    for (int i = 0; i < 10; i++) {
                        if (ix[i] >= 0) { // found
                            if (vcol[ix[i]] == -1) {
                                // use default
                                val[i] = Dictionary::pVdict->m_default[tvix[ix[i]]];
                            }
                            else {
                                val[i] = line.substr(vcol[ix[i]], vwidth[ix[i]]);
                            }
                        }
                    }
                    Network::addNode(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7],
                        val[8], val[9]);
                }
                else if (table_type == "REQUIREMENTS") {
                    for (int i = 0; i < 10; i++) {
                        val[i] = "";
                        ix[i] = -1;
                    }
                    ix[0] = findRecord('V', table_type, "REQID");
                    ix[1] = findRecord('V', table_type, "SOURCE");
                    ix[2] = findRecord('V', table_type, "DEST");
                    ix[3] = findRecord('V', table_type, "MSG_BYTES");
                    ix[4] = findRecord('V', table_type, "MSG_BITS");
                    ix[5] = findRecord('V', table_type, "MSG_RATE");
                    ix[6] = findRecord('V', table_type, "BANDWIDTH");
                    for (int i = 0; i < 7; i++) {
                        if (ix[i] >= 0) { // found
                            if (vcol[ix[i]] == -1) {
                                // use default
                                val[i] = Dictionary::pVdict->m_default[tvix[ix[i]]];
                            }
                            else {
                                val[i] = line.substr(vcol[ix[i]], vwidth[ix[i]]);
                            }
                        }
                    }
                    Network::addReq(val[0], val[1], val[2], val[3], val[4], val[5], val[6]);
                }
                else if (table_type == "LINETYPES") {
                    for (int i = 0; i < 10; i++) {
                        val[i] = "";
                        ix[i] = -1;
                    }

                    ix[0] = findRecord('V', table_type, "SVTY");
                    ix[1] = findRecord('V', table_type, "SPEEDIN");
                    ix[2] = findRecord('V', table_type, "DUPLEXITY");
                    ix[3] = findRecord('V', table_type, "TYPE");
                    ix[4] = findRecord('V', table_type, "FIXED_COST");
                    ix[5] = findRecord('V', table_type, "DIST_COST1");
                    ix[6] = findRecord('V', table_type, "DIST_COST2");
                    ix[7] = findRecord('V', table_type, "DIST1");
                    ix[8] = findRecord('V', table_type, "REL");
                    ix[9] = findRecord('V', table_type, "CHANNELS");
                    for (int i = 0; i < 10; i++) {
                        if (ix[i] >= 0) { // found
                            if (vcol[ix[i]] == -1) {
                                // use default
                                val[i] = Dictionary::pVdict->m_default[tvix[ix[i]]];
                            }
                            else {
                                val[i] = line.substr(vcol[ix[i]], vwidth[ix[i]]);
                            }
                        }
                    }
                    Linetype::addLinetype(val[0], val[1], val[2], val[3], val[4], val[5], val[6],
                        val[7], val[8], val[9]);
                }
                else if (table_type == "EQUIPMENT") {
                    for (int i = 0; i < 10; i++) {
                        val[i] = "";
                        ix[i] = -1;
                    }
                    ix[0] = findRecord('V', table_type, "BOX_MODEL");
                    ix[1] = findRecord('V', table_type, "FUNCTION");
                    ix[2] = findRecord('V', table_type, "DEFAULT");
                    ix[3] = findRecord('V', table_type, "BOX_COST");
                    ix[4] = findRecord('V', table_type, "ADAPT_CST");
                    ix[5] = findRecord('V', table_type, "MAX_LINKS_LS");
                    ix[6] = findRecord('V', table_type, "MAX_LINKS_T1");
                    ix[7] = findRecord('V', table_type, "MAX_LINKS_T3");
                    ix[8] = findRecord('V', table_type, "CHAN_DELAY");
                    for (int i = 0; i < 9; i++) {
                        if (ix[i] >= 0) { // found
                            if (vcol[ix[i]] == -1) {
                                // use default
                                val[i] = Dictionary::pVdict->m_default[tvix[ix[i]]];
                            }
                            else {
                                val[i] = line.substr(vcol[ix[i]], vwidth[ix[i]]);
                            }
                        }
                    }
                    Equipment::addEquip(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8]);
                }
                else if (table_type == "PARMS") {
                    for (int i = 0; i < 10; i++) {
                        val[i] = "";
                        ix[i] = -1;
                    }

                    // PARMS VALUE
                    ix[0] = findRecord('V', table_type, "PARMS");
                    ix[1] = findRecord('V', table_type, "VALUE");
                    for (int i = 0; i < 2; i++) {
                        if (ix[i] >= 0) { // found
                            if (vcol[ix[i]] == -1) {
                                // use default
                                val[i] = Dictionary::pVdict->m_default[tvix[ix[i]]];
                            }
                            else {
                                val[i] = line.substr(vcol[ix[i]], vwidth[ix[i]]);
                            }
                        }
                    }
                    Parm::changeParm(val[0], val[1]); // new function
                }
                break;
            }
        }
        myfile.close();
        if (Linetype::ncap <= 0) {
            cout << "Error: No line types." << endl;
            return false;
        }
        Network::makeWorld();
        //Line::make_dist_cost(); // Make distance and cost matrices
        //Edge::defineCost();
        return true;
    }
    else {
        cout << "Unable to open file" << endl;;
        return false;
    }
}

bool Reader2::readStdPrm(string f) {
    ifstream myfile;
    string line;
//    int len;
    char ch;
    string table_type = "";
    string val[] = { "", "", "", "", "" };
    int ix[] = { -1, -1, -1, -1, -1 };

    // Read standard parameter file and load data
    myfile.open(f);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if (Utils::trim(line).length() == 0) {
                continue;
            }
//            len = line.length();
            ch = line[0];
            switch (ch) {
            case '*': // comment
                continue;
                break;
            case '%': // header
                if (line.substr(0, 15) == "%TABLE STDPARMS") {
                    table_type = "STDPARMS";
                }
                else {
                    bool result = parse_header('P', line, table_type);
                    if (!result) {
                        cout << "readStdPrm parse_header failed." << endl;
                        return false;
                    }
                }
                break;
            case ' ': // data
                if (table_type == "STDPARMS") {
                    for (int i = 0; i < 5; i++) {
                        val[i] = "";
                        ix[i] = -1;
                    }

                    ix[0] = findRecord('P', table_type, "PARM");
                    ix[1] = findRecord('P', table_type, "TYPE");
                    ix[2] = findRecord('P', table_type, "CATEGORY");
                    ix[3] = findRecord('P', table_type, "RANGE");
                    ix[4] = findRecord('P', table_type, "DEFAULT");
                    for (int i = 0; i < 5; i++) {
                        if (ix[i] >= 0) { // found
                            if (p_vcol[ix[i]] == -1) {
                                // use default
                                val[i] = Dictionary::pPdict->m_default[p_tvix[ix[i]]];
                            }
                            else {
                                val[i] = line.substr(p_vcol[ix[i]], p_vwidth[ix[i]]);
                            }
                        }
                    }
                    Parm::addParm(val[0], val[1], val[2], val[3], val[4]);
                }
            }
        }
        myfile.close();
        cout << "Check parameters:" << endl;
        for (int i = 0; i < Parm::nParm; i++) {
            cout << Parm::pParm[i].parm << " " << Parm::pParm[i].deflt << endl;
        }
        return true;
    }
    else {
        cout << "Unable to open file" << endl;;
        return false;
    }
}

bool Reader2::parse_header(char dic, string buf, string tname) {
    string header;
    int n = 0;
    int start_at = ntvar;
    int ich = 1;                // Skip over the '%' in COL 1
    int header_length = buf.length();

    // CONTINUE TO PARSE UNTIL END OF FILE, CARRIAGE RETURN, OR LINE FEED
    while (true)
    {   // SET THE INDEX VAR TO THE FIRST NON-BLANK COLUMN POSITION OF THE RECORD
        while ((ich < header_length) && (buf[ich] == ' '))
            ich++;
        int col = ich;      // SET THE START COLUMN VARIABLE
        int w = 0;          // INITIALIZE WIDTH TO 0
        header = "";

        //*********************************************************************
        // SCAN THE HEADER, HEADERS DO NOT INCLUDE BLANKS, '+', OS SPEC. char. *
        //*********************************************************************
        // PLACE EACH VALID charACTER INTO THE THE HEADER charACTER ARRAY
        while ((ich < header_length) && (buf[ich] > ' ') && (buf[ich] != '+')) {
            header += buf[ich++];
            w++;
        }
        // HEADER CAN BE EXTENDED WITH '+'
        while ((ich < header_length) && (buf[ich] == '+'))
        {
            ich++;  // INCREMENT INDEX
            w++;    // INCREMENT WIDTH OF HEADER
        } //END WHILE

        //*********************************************************************
        //*********************************************************************
        if (dic == 'V') {
            for (int i = 0; i < Dictionary::pVdict->m_count; i++) {
                if ((Dictionary::pVdict->m_table[i] == tname)
                    && (Dictionary::pVdict->m_variable[i] == header)) {
                    expand();
                    tvix[ntvar - 1] = i;    // SET TABLE VARIABLE INDEX
                    vcol[ntvar - 1] = col;  // SET VARIABLE COLUMN START LOCATION
                    vwidth[ntvar - 1] = w;  // SET THE WIDTH
                    Dictionary::pVdict->m_present[i] = true; // present in input
                    n++;    // INCREMENT VARIABLE COUNTER
                    break;
                } // END IF
            } // END FOR
        }
        else if (dic == 'P') {
            for (int i = 0; i < Dictionary::pPdict->m_count; i++) {
                if ((Dictionary::pPdict->m_table[i] == tname)
                    && (Dictionary::pPdict->m_variable[i] == header)) {
                    p_expand();
                    p_tvix[p_ntvar - 1] = i;    // SET TABLE VARIABLE INDEX
                    p_vcol[p_ntvar - 1] = col;  // SET VARIABLE COLUMN START LOCATION
                    p_vwidth[p_ntvar - 1] = w;  // SET THE WIDTH
                    Dictionary::pPdict->m_present[i] = true; // present in input
                    n++; // INCREMENT VARIABLE COUNTER
                    break;
                } // END IF
            } // END FOR
        }
        // Break on linefeed, CR, or end of buf
        if ((buf[ich] == 10) || (buf[ich] == 13) || (ich >= header_length))
            break;
    } // END WHILE TRUE

    if (dic == 'V') {
        for (int i = 0; i < Dictionary::pVdict->m_count; i++) {
            if ((Dictionary::pVdict->m_table[i] != tname)
                || (Dictionary::pVdict->m_role[i] == '+'))
                continue;
            bool flag = false;
            for (int j = start_at; j < (start_at + n); j++) {
                if (i == tvix[j]) {
                    flag = true;
                    break;
                } // endif
            } // endfor
            if (flag == true)
                continue;
            if (Dictionary::pVdict->m_role[i] == '*') {
                cout << "Mandatory header " << Dictionary::pVdict->m_variable[i]
                    << " missing in table " << Dictionary::pVdict->m_table[i] << endl;
                return false;
            } // endif
            if (Dictionary::pVdict->m_role[i] == 'd') {
                expand();
                tvix[ntvar - 1] = i;    // SET TABLE VARIABLE INDEX
                vcol[ntvar - 1] = -1;   // SET invalid VARIABLE COLUMN START LOCATION for scan_field
                vwidth[ntvar - 1] = Dictionary::pVdict->m_variable[i].length(); // SET THE WIDTH to the variable name
                n++; // INCREMENT VARIABLE COUNTER
            } // endif
        } // endfor

        for (int i = start_at; i < (start_at + n); ++i) {
            char type = Dictionary::pVdict->m_type[tvix[i]];
            if (type == 'x' || type == 'u') {
//                cout << "THIS SHOULD NOW BE RUNNING!" << endl;
//                cout << "m_variable" << " " << Dictionary::pVdict->m_variable[tvix[i]] << endl;
//                cout << "m_table" << " " << Dictionary::pVdict->m_table[tvix[i]] << endl;
//                cout << "m_type" << " " << Dictionary::pVdict->m_type[tvix[i]] << endl;
//                cout << "m_role" << " " << Dictionary::pVdict->m_role[tvix[i]] << endl;
//                cout << "m_default" << " " << Dictionary::pVdict->m_default[tvix[i]] << endl;
//                cout << "m_min" << " " << Dictionary::pVdict->m_min[tvix[i]] << endl;
//                cout << "m_max" << " " << Dictionary::pVdict->m_max[tvix[i]] << endl;
//                cout << "m_varref" << " " << Dictionary::pVdict->m_varref[tvix[i]] << endl;
//                cout << "m_tabref" << " " << Dictionary::pVdict->m_tabref[tvix[i]] << endl;
//                if (Dictionary::pVdict->m_present[tvix[i]])
//                    cout << "m_present" << " True" << endl;
//                else
//                    cout << "m_present" << " False" << endl;
                // We now need to set the reference fields.  Can't be
                // done at the time we read the dictionary
                int ix = Instructions::checkTable(Dictionary::pVdict->m_tabref[tvix[i]]);
                int iy = Dictionary::pVdict->findEntry(Dictionary::pVdict->m_tabref[tvix[i]],
                    Dictionary::pVdict->m_varref[tvix[i]]);
                if ((ix == -1) || (iy == -1)) {
                    cout << "parse_header() failed.  Can't find " << Dictionary::pVdict->m_varref[tvix[i]]
                        << " in " << Dictionary::pVdict->m_tabref[tvix[i]]
                        << " to read " << Dictionary::pVdict->m_variable[tvix[i]]
                        << " in table " << tname << "." << endl;
                    return false;
                }
                ref_table[i] = ix;
                ref_var[i] = iy;
            }
            else {
                ref_table[i] = -1;
                ref_var[i] = -1;
            }  // endif
        }
    }
    else if (dic == 'P') {
        for (int i = 0; i < Dictionary::pPdict->m_count; i++) {
            if ((Dictionary::pPdict->m_table[i] != tname)
                || (Dictionary::pPdict->m_role[i] == '+'))
                continue;
            bool flag = false;
            for (int j = 0; j < n; j++) {
                if (i == p_tvix[j]) {
                    flag = true;
                    break;
                } // endif
            } // endfor
            if (flag == true)
                continue;
            if (Dictionary::pPdict->m_role[i] == '*') {
                cout << "Mandatory header " << Dictionary::pPdict->m_variable[i]
                    << " missing in table " << Dictionary::pPdict->m_table[i] << endl;
                return false;
            } // endif
            if (Dictionary::pPdict->m_role[i] == 'd') {
                p_expand();
                p_tvix[p_ntvar - 1] = i;    // SET TABLE VARIABLE INDEX
                p_vcol[p_ntvar - 1] = -1;   // SET invalid VARIABLE COLUMN START LOCATION for scan_field
                p_vwidth[p_ntvar - 1] = Dictionary::pPdict->m_variable[i].length(); // SET THE WIDTH to the variable name
                n++; // INCREMENT VARIABLE COUNTER
            } // endif
        } // endfor
    }
    return true;
}

int Reader2::findRecord(char dic, string table_type, string field)
{
	// First check if this field was on the header record so has a position defined
    if (dic == 'V') {
        for (int i = 0; i < ntvar; i++) {
            if ((Dictionary::pVdict->m_table[tvix[i]] == table_type) && (Dictionary::pVdict->m_variable[tvix[i]] == field)) {
                return i;
            }
        }
    }
    else if (dic == 'P') {
        for (int i = 0; i < p_ntvar; i++) {
            if ((Dictionary::pPdict->m_table[p_tvix[i]] == table_type) && (Dictionary::pPdict->m_variable[p_tvix[i]] == field)) {
                return i;
            }
        }
    }
	return -999; // not found
}
