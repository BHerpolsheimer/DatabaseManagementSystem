// File: Table.cpp
// Author: Brandon Herpolsheimer
// Date: 5/08/2018
// Version: 1.03
// Description: Implementation file for the Table class.

#include <fstream>
#include <vector>
#include "Table.h"

using namespace std;

// Parameterized constructor for Table sets name values and creates file;
Table::Table(string inUseDB, string name, list<string> arguments) {
  dbName = inUseDB;
  tableName = name;
  ofstream fout;
  fout.open("./" + inUseDB + "/" + name + ".txt");
  int i = 0;
  for (list<string>::iterator it = arguments.begin(); it != arguments.end(); it++, i++) {
  	if(i % 2 == 0)
  	  if(it == arguments.begin())
      	fout << it->substr(1, it->length() - 1) << " ";
      else
        fout << *it << " ";
  	else
      if(it->back() == ',' || it == --arguments.end())
      	fout << it->substr(0, it->length() - 1) << "\n";
      else {
      	cout << "Error: \"" << *it <<"\": syntax error\n";
      	return;
      }
  }
  fout << "-data-\n";
  fout.close();
  cout << "Table " << tableName << " created.\n";
}

// Deconstructor for table. Currently nothing here.
Table::~Table() {

}

// Returns the name of the Database that the Table belongs to.
std::string Table::getDBName() {
  return dbName;
}

// Returns the name of the Table.
std::string Table::getTableName() {
  return tableName;
}

// Deletes the file corresponding to the database
void Table::drop() {
  remove(("./" + dbName + "/" + tableName + ".txt").c_str());
}

void Table::insert(string inUseDB, string name, list<string> arguments) {
  ofstream fout;
  fout.open("./" + inUseDB + "/" + name + ".txt", ofstream::app);
  int i = 0;
  for (list<string>::iterator it = arguments.begin(); it != arguments.end(); it++, i++) {
    if(it == arguments.begin())
      fout << it->substr(1, it->length() - 2) << " ";
    else if(it == --arguments.end())
      fout << it->substr(0, it->length() - 1) << "\n";
    else
      fout << it->substr(0, it->length() - 1) << " ";
  }
  fout.close();
  cout << "1 new record inserted.\n";
}

// Seperates the words in a string into a list of substrings by spaces.
list<string> SeperateWords2(string input) {
  list<string> words;
  unsigned int wordStartIndex = 0, wordEndIndex = 0;
  while(wordStartIndex < input.length() - 1) {
    for(wordEndIndex++; input.at(wordStartIndex) == ' ' && wordStartIndex < input.length() - 1; wordStartIndex++, wordEndIndex++);
    for(;wordEndIndex < input.length() && input.at(wordEndIndex) != ' ' &&
      !(wordEndIndex == input.length() - 1 && input.at(wordEndIndex) == ';'); wordEndIndex++);
    words.push_back(input.substr(wordStartIndex, wordEndIndex - wordStartIndex));
    wordStartIndex = wordEndIndex;
  }
  return words;
}

// Returns true of false based on the passed operator and the passed values for it.
bool checkOperator(string str1, list<string> arguments) {
  list<string>::iterator it = arguments.end();
  string str2 = *(--it);
  string givenOpp = *(--it);

  if(!givenOpp.compare("=")) {
    if(!str1.compare(str2))
    	return true;
  }
  else if(!givenOpp.compare("!=")) {
    if(str1.compare(str2))
    	return true;
  }
  else if(!givenOpp.compare(">")) {
    if(strtof((str1).c_str(),0) > strtof((str2).c_str(),0))
    	return true;
  }
  else if(!givenOpp.compare("<")) {
    if(strtof((str1).c_str(),0) < strtof((str2).c_str(),0))
    	return true;
  }
  return false;
}

// Updates the string based on the passed set and where arguments.
void Table::update(string inUseDB, string name, list<string> arguments) {
  ifstream fin;
  ofstream fout;
  string temp;
  int paramCount = 0;

  // count params
  fin.open("./" + inUseDB + "/" + name + ".txt");
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount++;
  }
  fin.close();
  list<string> masterList[paramCount];

  // Read into lists
  fin.open("./" + inUseDB + "/" + name + ".txt");
  for(int i = 0; i < paramCount; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList[i].push_back(*words.begin());
    masterList[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it = words.begin();
    for(int i = 0; i < paramCount && it != words.end(); i++, it++)
      masterList[i].push_back(*it);
  }
  fin.close();

  // Find which list is checked in where condition
  int inUseList = 0;
  for(int i = 0; i < paramCount; i++) {
  	if(!(*masterList[i].begin()).compare(*++(++(++(++arguments.begin())))))
  	  inUseList = i;
  }

  fout.open("./" + inUseDB + "/" + name + ".txt");

  list<string>::iterator it[paramCount];
  for(int i = 0; i < paramCount; i++) {
    it[i] = masterList[i].begin();
    fout << *(it[i]) << " "; 
    fout << *(++it[i]) << "\n";
  }
  fout << "-data-\n";


  // Update the table
  int modifyCount = 0;
  int replaceList = 0;
  for(int i = 0; i < paramCount; i++)
  	if(!(*masterList[i].begin()).compare(*arguments.begin()))
  	  replaceList = i;
  string replaceString = *++(++arguments.begin());
  for(list<string>::iterator v = ++(++masterList[inUseList].begin()); v != masterList[inUseList].end(); v++) {
  	if(checkOperator(*v, arguments)) {
  	  for(int i = 0; i < paramCount; i++) {
  	  	if(i == replaceList) {
  	      fout << replaceString << ((i + 1 == paramCount) ? "\n" : " ");
  	      ++it[i];
  	    }
  	    else {
  	  	  fout << *(++it[i]) << ((i + 1 == paramCount) ? "\n" : " ");
  	    }
  	  }
  	  modifyCount++;
  	}
  	else
  	  for(int i = 0; i < paramCount; i++)
  	  	fout << *(++it[i]) << ((i + 1 == paramCount) ? "\n" : " ");
  }

  fout.close();
  cout << modifyCount << " record" << ((modifyCount != 1) ? "s " : " ") << "modified.\n";
}

// Deletes tuples from the table based on the passed arguments.
void Table::mydelete(string inUseDB, string name, list<string> arguments) {
  ifstream fin;
  ofstream fout;
  string temp;
  int paramCount = 0;
  fin.open("./" + inUseDB + "/" + name + ".txt");

  // Count params
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount++;
  }
  fin.close();

  // Sort into lists
  list<string> masterList[paramCount];
  fin.open("./" + inUseDB + "/" + name + ".txt");
  for(int i = 0; i < paramCount; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList[i].push_back(*words.begin());
    masterList[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it = words.begin();
    for(int i = 0; i < paramCount && it != words.end(); i++, it++)
      masterList[i].push_back(*it);
  }
  fin.close();

  int inUseList = 0;
  for(int i = 0; i < paramCount; i++) {
  	if(!(*masterList[i].begin()).compare(*arguments.begin()))
  	  inUseList = i;
  }
  
  // Modify the table
  fout.open("./" + inUseDB + "/" + name + ".txt");

  list<string>::iterator it[paramCount];
  for(int i = 0; i < paramCount; i++) {
    it[i] = masterList[i].begin();
    fout << *(it[i]) << " "; 
    fout << *(++it[i]) << "\n";
  }
  fout << "-data-\n";

  int modifyCount = 0;
  for(list<string>::iterator v = ++(++masterList[inUseList].begin()); v != masterList[inUseList].end(); v++) {
  	if(checkOperator(*v, arguments)) {
  	  for(int i = 0; i < paramCount; i++) {
  	  	++it[i];
  	  }
  	  modifyCount++;
  	}
  	else
  	  for(int i = 0; i < paramCount; i++)
  	  	fout << *(++it[i]) << ((i + 1 == paramCount) ? "\n" : " ");
  }

  fout.close();
  cout << modifyCount << " record" << ((modifyCount != 1) ? "s " : " ") << "deleted.\n";
}

// Conducts a subquery of the table.
void Table::subQuery(string inUseDB, string name, list<string> arguments, list<string> paramNames) {
  ifstream fin;
  string temp;
  int paramCount = 0;
  fin.open("./" + inUseDB + "/" + name + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + name + ".txt");}

  // Count params
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount++;
  }
  fin.close();

  // Sort into lists
  list<string> masterList[paramCount];
  fin.open("./" + inUseDB + "/" + name + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + name + ".txt");}
  for(int i = 0; i < paramCount; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList[i].push_back(*words.begin());
    masterList[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it = words.begin();
    for(int i = 0; i < paramCount && it != words.end(); i++, it++)
      masterList[i].push_back(*it);
  }
  fin.close();

  int inUseList = 0;
  for(int i = 0; i < paramCount; i++) {
  	if(!(*masterList[i].begin()).compare(*arguments.begin()))
  	  inUseList = i;
  }
  
  // Determine which lists are being used.
  list<string> subList[paramNames.size()];

  int count = 0;
  for(string v : paramNames) {
  	if(v.back() == ',')
      v = v.substr(0, v.length() - 1);
  	for(int i = 0; i < paramCount; i++)
  	  if(!v.compare(*(masterList[i].begin())))
        subList[count++] = masterList[i];
  }

  list<string>::iterator it[paramNames.size()];
  for(unsigned int i = 0; i < paramNames.size(); i++)
    it[i] = subList[i].begin();

  for(unsigned int i = 0; i < paramNames.size(); i++) {
    cout << *(it[i]++);
    cout << " " << *(it[i]++) << ((i + 1 == paramNames.size()) ? "\n" : " | ");
  }

  // Generate subquery output.
  for(list<string>::iterator v = ++(++masterList[inUseList].begin()); v != masterList[inUseList].end(); v++) {
   	if(checkOperator(*v, arguments))
      for(unsigned int i = 0; i < paramNames.size(); i++)
    	cout << ((it[i]->front() == '\'') ? it[i]->substr(1, it[i]->length() - 2) : *it[i]) << ((i + 1 == paramNames.size()) ? "\n" : " | ");
    for(unsigned int i = 0; i < paramNames.size(); i++)
      ++it[i];
  }  
}

// Conducts an inner join using the given arguments.
void Table::innerJoin(std::string inUseDB, std::list<std::string> arguments) {
  list<string>::iterator it = arguments.begin();
  ifstream fin;
  string temp;
  int paramCount = 0, paramCount2 = 0;
  fin.open(("./" + inUseDB + "/" + *++(++(++it)) + ".txt" + "-1").c_str());
  if(fin.good());
  else {fin.close(); fin.open(("./" + inUseDB + "/" + *((it)) + ".txt").c_str());}

  // Count params
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount++;
  }
  fin.close();

  // Sort into vectors
  vector<string> masterList[paramCount];
  fin.open("./" + inUseDB + "/" + *it + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}
  for(int i = 0; i < paramCount; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList[i].push_back(*words.begin());
    masterList[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it2 = words.begin();
    for(int i = 0; i < paramCount && it2 != words.end(); i++, it2++)
      masterList[i].push_back(*it2);
  }
  fin.close();

  fin.open("./" + inUseDB + "/" + *++(++(++(++it))) + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *(((it))) + ".txt");}

  // Count params in joined database
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount2++;
  }
  fin.close();

  // Sort into vectors
  vector<string> masterList2[paramCount2];
  fin.open("./" + inUseDB + "/" + *it + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}
  for(int i = 0; i < paramCount2; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList2[i].push_back(*words.begin());
    masterList2[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it2 = words.begin();
    for(int i = 0; i < paramCount2 && it2 != words.end(); i++, it2++)
      masterList2[i].push_back(*it2);
  }
  fin.close();

  (++(++(++it)));

  // Find lists in use for condition
  int inUseList1 = 0;
  for(int i = 0; i < paramCount; i++) {
    if(!(*masterList[i].begin()).compare((*((it))).substr(2)))
      inUseList1 = i;
  }

  ((++(++it)));

  int inUseList2 = 0;
  for(int i = 0; i < paramCount2; i++) {
    if(!(*masterList2[i].begin()).compare((*(it)).substr(2)))
      inUseList2 = i;
  }

  // Generate a joined list.
  vector<string> masterList3[paramCount + paramCount2];
  for(int i = 0; i < paramCount; i++) {
    masterList3[i].push_back(*masterList[i].begin());
    masterList3[i].push_back(*(++masterList[i].begin()));
  }
  for(int i = paramCount; i < paramCount + paramCount2; i++) {
    masterList3[i].push_back(*masterList2[i - paramCount].begin());
    masterList3[i].push_back(*(++masterList2[i - paramCount].begin()));
  }

  for(unsigned int i = 2; i < masterList[0].size(); i++)
    for(unsigned int j = 2; j < masterList2[0].size(); j++)
      if(!masterList[inUseList1].at(i).compare(masterList2[inUseList2].at(j))) {
        for(int k = 0; k < paramCount; k++)
          masterList3[k].push_back(masterList[k].at(i));
        for(int k = paramCount; k < paramCount + paramCount2; k++)
          masterList3[k].push_back(masterList2[k - paramCount].at(j));
      }

  vector<string>::iterator it2[paramCount + paramCount2];
  for(int i = 0; i < paramCount + paramCount2; i++)
    it2[i] = masterList3[i].begin();

  for(int i = 0; i < paramCount + paramCount2; i++) {
    cout << *(it2[i]++);
    cout << " " << *(it2[i]++) << ((i + 1 == paramCount + paramCount2) ? "\n" : " | ");
  }

  // Generate output.
  for(vector<string>::iterator v = ++(++masterList3[0].begin()); v != masterList3[0].end(); v++) {
    for(int i = 0; i < paramCount + paramCount2; i++)
      cout << ((it2[i]->front() == '\'') ? it2[i]->substr(1, it2[i]->length() - 2) : *it2[i]) << ((i + 1 == paramCount + paramCount2) ? "\n" : " | ");
    for(int i = 0; i < paramCount + paramCount2; i++)
      ++it2[i];
  }
}


// Conducts an inner join using the given arguments in a different format.
void Table::innerJoin2(std::string inUseDB, std::list<std::string> arguments) {
  list<string>::iterator it = arguments.begin();
  ifstream fin;
  string temp;
  int paramCount = 0, paramCount2 = 0;
  fin.open(("./" + inUseDB + "/" + *++(++(++it)) + ".txt" + "-1").c_str());
  if(fin.good());
  else {fin.close(); fin.open(("./" + inUseDB + "/" + *((it)) + ".txt").c_str());}

  // Count params
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount++;
  }
  fin.close();

  // Sort into vectors
  vector<string> masterList[paramCount];
  fin.open("./" + inUseDB + "/" + *it + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}
    for(int i = 0; i < paramCount; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList[i].push_back(*words.begin());
    masterList[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it2 = words.begin();
    for(int i = 0; i < paramCount && it2 != words.end(); i++, it2++)
      masterList[i].push_back(*it2);
  }
  fin.close();

  fin.open("./" + inUseDB + "/" + *((++(++it))) + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}

  // Count params in joined database
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount2++;
  }
  fin.close();

  // Sort into vectors
  vector<string> masterList2[paramCount2];
  fin.open("./" + inUseDB + "/" + *it + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}
  for(int i = 0; i < paramCount2; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList2[i].push_back(*words.begin());
    masterList2[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it2 = words.begin();
    for(int i = 0; i < paramCount2 && it2 != words.end(); i++, it2++)
      masterList2[i].push_back(*it2);
  }
  fin.close();

  // Find lists in use for condition
  (++(++(++it)));

  int inUseList1 = 0;
  for(int i = 0; i < paramCount; i++) {
    if(!(*masterList[i].begin()).compare((*((it))).substr(2)))
      inUseList1 = i;
  }

  ((++(++it)));

  int inUseList2 = 0;
  for(int i = 0; i < paramCount2; i++) {
    if(!(*masterList2[i].begin()).compare((*(it)).substr(2)))
      inUseList2 = i;
  }

  // Generate joined list.
  vector<string> masterList3[paramCount + paramCount2];
  for(int i = 0; i < paramCount; i++) {
    masterList3[i].push_back(*masterList[i].begin());
    masterList3[i].push_back(*(++masterList[i].begin()));
  }
  for(int i = paramCount; i < paramCount + paramCount2; i++) {
    masterList3[i].push_back(*masterList2[i - paramCount].begin());
    masterList3[i].push_back(*(++masterList2[i - paramCount].begin()));
  }

  for(unsigned int i = 2; i < masterList[0].size(); i++)
    for(unsigned int j = 2; j < masterList2[0].size(); j++)
      if(!masterList[inUseList1].at(i).compare(masterList2[inUseList2].at(j))) {
        for(int k = 0; k < paramCount; k++)
          masterList3[k].push_back(masterList[k].at(i));
        for(int k = paramCount; k < paramCount + paramCount2; k++)
          masterList3[k].push_back(masterList2[k - paramCount].at(j));
      }

  vector<string>::iterator it2[paramCount + paramCount2];
  for(int i = 0; i < paramCount + paramCount2; i++)
    it2[i] = masterList3[i].begin();

  for(int i = 0; i < paramCount + paramCount2; i++) {
    cout << *(it2[i]++);
    cout << " " << *(it2[i]++) << ((i + 1 == paramCount + paramCount2) ? "\n" : " | ");
  }

  // Generate output.
  for(vector<string>::iterator v = ++(++masterList3[0].begin()); v != masterList3[0].end(); v++) {
    for(int i = 0; i < paramCount + paramCount2; i++)
      cout << ((it2[i]->front() == '\'') ? it2[i]->substr(1, it2[i]->length() - 2) : *it2[i]) << ((i + 1 == paramCount + paramCount2) ? "\n" : " | ");
    for(int i = 0; i < paramCount + paramCount2; i++)
      ++it2[i];
  }
}

// Conducts an outer join using the given arguments.
void Table::outerJoin(std::string inUseDB, std::list<std::string> arguments) {
  list<string>::iterator it = arguments.begin();
  ifstream fin;
  string temp;
  int paramCount = 0, paramCount2 = 0;
  fin.open("./" + inUseDB + "/" + *++(++(++it)) + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}

  // Count params
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount++;
  }
  fin.close();

  // Sort into vectors
  vector<string> masterList[paramCount];
  fin.open("./" + inUseDB + "/" + *it + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}
  for(int i = 0; i < paramCount; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList[i].push_back(*words.begin());
    masterList[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it2 = words.begin();
    for(int i = 0; i < paramCount && it2 != words.end(); i++, it2++)
      masterList[i].push_back(*it2);
  }
  fin.close();

  fin.open("./" + inUseDB + "/" + *++(++(++(++(++it)))) + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}

  // Count params in joined database
  while(getline(fin, temp)) {
    if(!temp.compare("-data-"))
      break;
    paramCount2++;
  }
  fin.close();

  // Sort into vectors
  vector<string> masterList2[paramCount2];
  fin.open("./" + inUseDB + "/" + *it + ".txt" + "-1");
  if(fin.good());
  else {fin.close(); fin.open("./" + inUseDB + "/" + *it + ".txt");}
  for(int i = 0; i < paramCount2; i++) {
    getline(fin, temp);
    list<string> words =  SeperateWords2(temp);
    masterList2[i].push_back(*words.begin());
    masterList2[i].push_back(*(++words.begin()));
  }
  getline(fin, temp);
  while(getline(fin, temp)) {
    list<string> words =  SeperateWords2(temp);
    list<string>::iterator it2 = words.begin();
    for(int i = 0; i < paramCount2 && it2 != words.end(); i++, it2++)
      masterList2[i].push_back(*it2);
  }
  fin.close();

  // Find lists in use for condition
  (++(++(++it)));

  int inUseList1 = 0;
  for(int i = 0; i < paramCount; i++) {
    if(!(*masterList[i].begin()).compare((*((it))).substr(2)))
      inUseList1 = i;
  }

  ((++(++it)));

  int inUseList2 = 0;
  for(int i = 0; i < paramCount2; i++) {
    if(!(*masterList2[i].begin()).compare((*(it)).substr(2)))
      inUseList2 = i;
  }

  // Generate joined list
  vector<string> masterList3[paramCount + paramCount2];
  for(int i = 0; i < paramCount; i++) {
    masterList3[i].push_back(*masterList[i].begin());
    masterList3[i].push_back(*(++masterList[i].begin()));
  }
  for(int i = paramCount; i < paramCount + paramCount2; i++) {
    masterList3[i].push_back(*masterList2[i - paramCount].begin());
    masterList3[i].push_back(*(++masterList2[i - paramCount].begin()));
  }

  bool useTest = false;
  for(unsigned int i = 2; i < masterList[0].size(); i++) {
    for(unsigned int j = 2; j < masterList2[0].size(); j++)
      if(!masterList[inUseList1].at(i).compare(masterList2[inUseList2].at(j))) {
        for(int k = 0; k < paramCount; k++)
          masterList3[k].push_back(masterList[k].at(i));
        for(int k = paramCount; k < paramCount + paramCount2; k++)
          masterList3[k].push_back(masterList2[k - paramCount].at(j));
        useTest = true;
      }
    if(useTest == false) {
      for(int k = 0; k < paramCount; k++)
        masterList3[k].push_back(masterList[k].at(i));
      for(int k = paramCount; k < paramCount + paramCount2; k++)
        masterList3[k].push_back("");
    }
    useTest = false;
  }


  vector<string>::iterator it2[paramCount + paramCount2];
  for(int i = 0; i < paramCount + paramCount2; i++)
    it2[i] = masterList3[i].begin();

  for(int i = 0; i < paramCount + paramCount2; i++) {
    cout << *(it2[i]++);
    cout << " " << *(it2[i]++) << ((i + 1 == paramCount + paramCount2) ? "\n" : " | ");
  }

  // Generate output.
  for(vector<string>::iterator v = ++(++masterList3[0].begin()); v != masterList3[0].end(); v++) {
    for(int i = 0; i < paramCount + paramCount2; i++)
      cout << ((it2[i]->front() == '\'') ? it2[i]->substr(1, it2[i]->length() - 2) : *it2[i]) << ((i + 1 == paramCount + paramCount2) ? "\n" : " | ");
    for(int i = 0; i < paramCount + paramCount2; i++)
      ++it2[i];
  }
}