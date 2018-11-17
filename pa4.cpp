// File: pa4.cpp
// Author: Brandon Herpolsheimer
// Date: 5/08/2018
// Version: 1.03
// Description:  Allows a database user to manage the metadata of their relational data

#include <fstream>
#include <algorithm>
#include <chrono>
#include <glob.h>
#include "Database.h"
#include "Table.h"

using namespace std;
using namespace chrono;

list<string> SeperateWords(string input);

bool CheckForName(list<Database> dbList, string name);

bool TableCheck(string name);

int main(int argc, char *argv[])
{
  list<Database> dbList;
  string inUseDB = "";
  milliseconds ms = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
  );
  ifstream inputFile;
  if(argc != 1)
  	inputFile.open(argv[1]);

  cout << "\n";
  while(1) {
    if(cin.eof())
      break;

    // Determine if manual input or input from file.
  	string input;
  	if(argc == 1) {
      // cout << "pa4> ";
      getline(cin, input);
    }
    else 
      getline(inputFile, input);

    if(cin.eof())
      cout << "\n";

    // Skip empty lines
    if(input.size() == 0)
      continue;

    // Skip empty or commented lines.
    if(!input.compare("\r") || !input.compare(";") || (input.at(0) == '-' && input.at(1) == '-'))
      continue;

    // Remove the \r character if included from the input file.
    if(input.back() == '\r')
      input = input.substr(0, input.length() -1);

    // Take input until the semicolon.
    while(input.back() != ';' && input.compare(".exit") && input.compare(".EXIT")) {
      if(argc == 1) {
        string inputContinuation = " ";
        // cout << "...>";
        input.append(" ");
        getline(cin, inputContinuation);
        input.append(inputContinuation);

        // Remove the \r character if included from the input file.
        if(input.back() == '\r')
          input = input.substr(0, input.length() -1);
      }
      else{
        string inputContinuation = " ";
        input.append(" ");
        getline(inputFile, inputContinuation);
        input.append(inputContinuation);

        // Remove the \r character if included from the input file.
        if(input.back() == '\r')
          input = input.substr(0, input.length() -1);
      }
    }

    // add spaces
    size_t found  = input.find("(");
    if (found != string::npos)
      input.replace(found,1," (");

    found  = input.find(",");
    if (found != string::npos)
      input.replace(found,1,", ");

    // remove tabs
    for(int i = 0; i < 20; i++) {
      found  = input.find("\t");
      if (found != string::npos)
        input.replace(found,1," ");
    }

    list<string> words = SeperateWords(input);

    // Convert to lower case.
    for(list<string>::iterator it = words.begin(); it != words.end(); it++)
      if(it->front() != '\'')
        transform(it->begin(), it->end(), it->begin(), ::tolower);

    // Get existing databases
    glob_t bases;
    glob("./*/", 0, NULL, &bases);
    for(size_t i = 0; i < bases.gl_pathc; i++) {
  	  string temp = bases.gl_pathv[i];
      Database dbTemp(temp.substr(2, temp.length() - 3));
      if(!CheckForName(dbList, temp.substr(2, temp.length() - 3))) {
        dbList.push_back(dbTemp);
      }
    }


    // Series of if and else if statements that cover the range of possible inputs
    // as well as the majority of common input errors. Based on the input, the if else
    // tree will eventually lead to the proper output or the proper error message.
    list<string>::iterator it = words.begin();
    if(!it->compare("create")) {
      if(++it == words.end())
        cout << "Error: No object specified\n";
      else if(!it->compare("database")) {
        if(++it == words.end())
          cout << "Error: No database name specified\n";
        else if(++it != words.end())
          cout << "Error: \"" << *it <<"\": syntax error\n";
        else {
          Database dbTemp(*--it);
          if(!CheckForName(dbList, *it)) {
            dbList.push_back(dbTemp);
            cout << "Database " << *it << " created.\n"; 
          }
          else
            cout << "!Failed to create database " << *it << " because it already exists.\n";
        }
      }
      else if(!it->compare("table")) {
        if(!inUseDB.compare(""))
          cout << "Error: No database currently in use\n";
        else if(++it == words.end())
          cout << "Error: No table name specified\n";
        else{
          if(++it == words.end())
            cout << "Error: No argument values specified\n";
          else if(!inUseDB.compare(""))
            cout << "Error: No database currently in use\n";
          else if(!CheckForName(dbList, *--it) && TableCheck("./" + inUseDB + "/" + *++(++words.begin()) + ".txt"))
            cout << "!Failed to create table " << *++(++words.begin()) << " because it already exists.\n";
          else if((++it)->front() == '(') {
            list<string> arguments;
            arguments.splice(arguments.begin(), words, it, words.end());
            Table tableTemp(inUseDB, *++(++words.begin()), arguments);
          }
          else
            cout << "Error: \"" << *it <<"\": syntax error\n";
        }
      } 
      else
    	  cout << "Error: \"" << *it <<"\": syntax error\n";
    }
    else if(!it->compare("drop")) {
      if(++it == words.end())
        cout << "Error: No object specified\n";
      else if(!it->compare("database")) {
        if(++it == words.end())
          cout << "Error: No database name specified\n";
        else if(++it != words.end())
          cout << "Error: \"" << *it <<"\": syntax error\n";
        else {
          if(CheckForName(dbList, *--it)) {
            list<Database>::iterator jt = dbList.begin();
            for (; jt != dbList.end(); jt++)
              if(!jt->getDBName().compare(*it))
                break;
            jt->drop();
            dbList.erase(jt);
            cout << "Database " << *it << " deleted.\n";
            if(!inUseDB.compare(*it))
              inUseDB = "";
          }
          else
            cout << "!Failed to delete " << *it << " because it does not exist.\n";
        }
      }
      else if(!it->compare("table")) {
      	if(!inUseDB.compare(""))
          cout << "Error: No database currently in use\n";
        else if(++it == words.end())
          cout << "Error: No table name specified\n"; 
        else if(++it != words.end())
          cout << "Error: \"" << *it <<"\": syntax error\n";
        else {
          if(TableCheck("./" + inUseDB + "/" + *++(++words.begin()) + ".txt")) {
            remove(("./" + inUseDB + "/" + *++(++words.begin()) + ".txt").c_str());
            cout << "Table " << *--it << " deleted.\n"; 
          }
          else
            cout << "!Failed to delete " << *--it << " because it does not exist.\n";
        }
      }
      else
        cout << "Error: \"" << *it <<"\": syntax error\n";
    }
    else if(!it->compare("use")) {
      if(++it == words.end())
        cout << "Error: No database name specified\n"; 
      else if(++it != words.end())
        cout << "Error: \"" << *it <<"\": syntax error\n";
      else
        if(CheckForName(dbList, *--it)) {
          inUseDB = *it;
          cout <<  "Using Database " << *it << ".\n";
        }
        else
          cout << "Error: Invalid database name\n";
    }
    else if(!it->compare("select")) {
      if(++it == words.end())
        cout << "Error: No object specified\n";
      else if(!it->compare("*")) {
        if(++it == words.end())
          cout << "Error: Syntax error\n";
        else if(!it->compare("from")) {
          if(!inUseDB.compare(""))
            cout << "Error: No database currently in use\n";
          else if(++it == words.end())
            cout << "Error: No table name specified\n";
          else if(++it != words.end()) {
            if(++it == words.end())
              cout << "Error: No table variable specified\n";        	
            else if(!it->compare("inner")) {
              if(++it == words.end())
                cout << "Error: Syntax Error\n";
              else if(!it->compare("join")) {
              	if(++it == words.end())
                  cout << "Error: No joined table name specified\n";
              	else if(++it == words.end())
                  cout << "Error: No joined table variable specified\n";
                else{
                  if(++it == words.end())
                    cout << "Error: Syntax Error\n";
                  else if(!it->compare("on")) {
                    if(++it == words.end())
                	    cout << "Error: No condition specified\n";
                    else if(++it == words.end())
                	    cout << "Error: No condition specified\n";
                	  else if(++it == words.end())
                	    cout << "Error: No condition specified\n";
                	  else if(++it == words.end()) {
                      if(TableCheck("./" + inUseDB + "/" + *--(--(--(--(--(--it))))) + ".txt"))
                        if(TableCheck("./" + inUseDB + "/" + *--(--(--(--it))) + ".txt"))
                          Table::innerJoin(inUseDB, words);
                    }
                  }
                }
              }
            }
            else if(!it->compare("left")) {
              if(++it == words.end())
                cout << "Error: Syntax Error\n";
              else if(!it->compare("outer")) {
                if(++it == words.end())
                  cout << "Error: Syntax Error\n";
                else if(!it->compare("join")) {
                	if(++it == words.end())
                    cout << "Error: No joined table name specified\n";
                	else if(++it == words.end())
                    cout << "Error: No joined table variable specified\n";
                  else{
                    if(++it == words.end())
                      cout << "Error: Syntax Error\n";
                    else if(!it->compare("on")) {
                      if(++it == words.end())
                  	    cout << "Error: No condition specified\n";
                      else if(++it == words.end())
                  	    cout << "Error: No condition specified\n";
                    	else if(++it == words.end())
                  	    cout << "Error: No condition specified\n";
                  	  else if(++it == words.end()) {
                        if(TableCheck("./" + inUseDB + "/" + *--(--(--(--(--(--it))))) + ".txt"))
                          if(TableCheck("./" + inUseDB + "/" + *--(--(--(--(--it)))) + ".txt"))
                            Table::outerJoin(inUseDB, words);
                      }
                    }
                  }
                }
              }
            }
            else {
              if(++it == words.end())
              	cout << "Error: No joined table variable specified\n";
              else{
                if(++it == words.end())
                  cout << "Error: Syntax Error\n";
                else if(!it->compare("where")) {
                  if(++it == words.end())
              	    cout << "Error: No condition specified\n";
                  else if(++it == words.end())
              	    cout << "Error: No condition specified\n";
              	  else if(++it == words.end())
              	    cout << "Error: No condition specified\n";
              	  else if(++it == words.end()) {
                    if(TableCheck("./" + inUseDB + "/" + *--(--(--(--(--(--it))))) + ".txt"))
                      if(TableCheck("./" + inUseDB + "/" + *((--(--it))) + ".txt"))
                        Table::innerJoin2(inUseDB, words);
              	  }
              	}
              }
            }
          }
          else {
            if(TableCheck("./" + inUseDB + "/" + *--it + ".txt")) {
              ifstream fin;
              string storage;
              fin.open(("./" + inUseDB + "/" + *it + ".txt" + "-1").c_str());
              if(fin.good());
              else {fin.close(); fin.open(("./" + inUseDB + "/" + *it + ".txt").c_str());}
              for(int countTest = -1; ; getline(fin, storage), countTest++) {
              	if(!storage.compare("-data-")) {
                  cout << "\n";
                  break;
                }
              	if(countTest > 0)
                  cout << " | ";
                cout << "" << ((storage.front() == '\'') ? storage.substr(1, storage.length() - 2) : storage);
              }
              while(getline(fin, storage)) {
              	list<string> words2 = SeperateWords(storage);
              	for(list<string>::iterator it2 = words2.begin(); it2 != words2.end(); it2++) {
                  cout << ((it2->front() == '\'') ? it2->substr(1, it2->length() - 2) : *it2);
                  cout << ((++it2 == words2.end()) ? "\n" : " | ");
                  it2--;
                }
              }
              fin.close();
            }
            else
              cout << "!Failed to query table " << *it << " because it does not exist.\n";
          }
        }
        else
          cout << "Error: syntax error\n";
      }
      else {
        list<string> paramNames;
        while(it->compare("from"))
          paramNames.push_back(*(it++));
        if(!inUseDB.compare(""))
          cout << "Error: No database currently in use\n";
        else if(++it == words.end())
          cout << "Error: No table name specified\n";
        else{
          if(++it == words.end())
            cout << "Error: Syntax Error\n";
          else if(!it->compare("where")) {
            if(++it == words.end())
              cout << "Error: No parameter name specified\n";
            else{
              if(++it == words.end())
                cout << "Error: No operator specified\n";
              else{
                if(++it == words.end())
                  cout << "Error: No parameter value specified\n";
                else{
                  list<string> arguments;
                  list<string>::iterator tempIt = --(--(--words.end()));
                  arguments.splice(arguments.begin(), words, tempIt, words.end());
                  Table::subQuery(inUseDB, *--(--(((words.end())))), arguments, paramNames);
                }
              }
            }
          }
          else
            cout << "Error: \"" << *it <<"\": syntax error\n";
        }
      }
    }
    else if(!it->compare("alter")) {
      if(++it == words.end())
        cout << "Error: No object specified\n";
      else if(!it->compare("table")) {
      	if(!inUseDB.compare(""))
          cout << "Error: No database currently in use\n";
        else if(++it == words.end())
          cout << "Error: No table name specified\n";
        else{

		    long temp;
	        ifstream fin;
	        fin.open(inUseDB + "/locked.txt");
	        if(fin.good()) {
	        	fin >> temp;

			    if(temp == ms.count()) {
			    }
			    else {
			    	cout << "Error: Table Flights is locked!\n";
			    	continue;
			    }
			}
			fin.close();        	

          if(++it == words.end())
            cout << "Error: No command specified\n";
          else if(!it->compare("add")) {
            if(++it == words.end())
              cout << "Error: No argument values specified\n";
            else if(++it == words.end())
              cout << "Error: Insufficient argument values specified\n";
            else if(!inUseDB.compare(""))
              cout << "Error: No database currently in use\n";
            else if(!TableCheck("./" + inUseDB + "/" + *++(++words.begin()) + ".txt"))
              cout << "!Failed to modify " << *++(++words.begin()) << " because it does not exist.\n";
            else{
              ofstream fout;
              fout.open(("./" + inUseDB + "/" + *++(++words.begin()) + ".txt").c_str(), ios_base::app);
              fout << *--it << " " << *it << "\n";
              fout.close();
              cout << "Table " << *--(--it) << " modified.\n";
            }
          }
          else
            cout << "Error: \"" << *it <<"\": syntax error\n";
        }
      }
      else
        cout << "Error: \"" << *it <<"\": syntax error\n";
    }
    else if(!it->compare("insert")) {
      if(++it == words.end())
        cout << "Error: Syntax Error\n";
      else if(!it->compare("into")) {
        if(!inUseDB.compare(""))
          cout << "Error: No database currently in use\n";
        else if(++it == words.end())
          cout << "Error: No table name specified\n";
	    else{

		    long temp;
	        ifstream fin;
	        fin.open(inUseDB + "/locked.txt");
	        if(fin.good()) {
	        	fin >> temp;

			    if(temp == ms.count()) {
			    }
			    else {
			    	cout << "Error: Table Flights is locked!\n";
			    	continue;
			    }
			}
			fin.close();

          if(++it == words.end())
            cout << "Error: Syntax Error\n";
          else if(!it->compare("values")) {
            if(++it == words.end())
              cout << "Error: No argument values specified\n";
            else if(it->front() == '(') {
              list<string> arguments;
              arguments.splice(arguments.begin(), words, it, words.end());
              Table::insert(inUseDB, *++(++words.begin()), arguments);
            }
            else
              cout << "Error: \"" << *it <<"\": syntax error\n";
          }
        }
      }
      else
        cout << "Error: syntax error\n";
    }
    else if(!it->compare("update")) {
      if(!inUseDB.compare(""))
        cout << "Error: No database currently in use\n";
      else if(++it == words.end())
        cout << "Error: No table name specified\n";
      else{

      	long temp;
        ifstream fin;
        fin.open(inUseDB + "/locked.txt");
        if(fin.good()) {
        	fin >> temp;

		    if(temp == ms.count()) {
		    }
		    else {
		    	cout << "Error: Table Flights is locked!\n";
		    	continue;
		    }
		}
		fin.close();

        if(++it == words.end())
          cout << "Error: Syntax Error\n";
        else if(!it->compare("set")) {
          if(++it == words.end())
            cout << "Error: No parameter name specified\n";
          else{
            if(++it == words.end())
              cout << "Error: No operator specified\n";
            else{
              if(++it == words.end())
                cout << "Error: No parameter value specified\n";
              else{
                if(++it == words.end())
                  cout << "Error: Syntax Error\n";
                else if(!it->compare("where")) {
                  if(++it == words.end())
                    cout << "Error: No parameter name specified\n";
                  else{
                    if(++it == words.end())
                      cout << "Error: No operator specified\n";
                    else{
                      if(++it == words.end())
                        cout << "Error: No parameter value specified\n";
                      else{
                        list<string> arguments;
                        arguments.splice(arguments.begin(), words, ++(++(++words.begin())), words.end());
                        Table::update(inUseDB, *(++words.begin()), arguments);
                      }
                    }
                  }
                }
                else
                  cout << "Error: \"" << *it <<"\": syntax error\n";
              }
            }
          }
        }
      }
    }
    else if(!it->compare("delete")) {
      if(!inUseDB.compare(""))
        cout << "Error: No database currently in use\n";
      else if(++it == words.end())
        cout << "Error: Syntax Error\n";
      else if(!it->compare("from")) {

	    long temp;
        ifstream fin;
        fin.open(inUseDB + "/locked.txt");
        if(fin.good()) {
        	fin >> temp;

		    if(temp == ms.count()) {
		    }
		    else {
		    	cout << "Error: Table Flights is locked!\n";
		    	continue;
		    }
		}
		fin.close();

        if(++it == words.end())
          cout << "Error: No table name specified\n";
        else{
          if(++it == words.end())
            cout << "Error: Syntax Error\n";
          else if(!it->compare("where")) {
            if(++it == words.end())
              cout << "Error: No parameter name specified\n";
            else{
              if(++it == words.end())
                cout << "Error: No operator specified\n";
              else{
                if(++it == words.end())
                  cout << "Error: No parameter value specified\n";
                else{
                  list<string> arguments;
                  arguments.splice(arguments.begin(), words, ++(++(++(++words.begin()))), words.end());
                  Table::mydelete(inUseDB, *++(++words.begin()), arguments);
                }
              }
            }
          }
        }
      }
    }
    else if(!it->compare("begin")) {
      if(!inUseDB.compare(""))
        cout << "Error: No database currently in use\n";
      else if(++it == words.end())
        cout << "Error: syntax error\n";
      else if(!it->compare("transaction")) {
        if(++it == words.end()) {
          cout << "Transaction starts.\n";
      	  ofstream fout;
      	  ifstream fin;
      	  fin.open(inUseDB + "/locked.txt");
      	  if(!fin.good()) {
	      	  fout.open(inUseDB + "/locked.txt");
	          ms = duration_cast< milliseconds >(
	              system_clock::now().time_since_epoch()
	          );
	          fout << ms.count();
	          fout.close();
	          glob_t tables;
			  glob(("./" + inUseDB + "/*").c_str(), 0, NULL, &tables);
			  for(size_t i = 0; i < tables.gl_pathc; i++) {
			  	string temp = tables.gl_pathv[i];
			  	fin.open(temp);
			  	fout.open(temp + "-1");
			  	while(getline(fin, temp))
			  		fout << temp << "\n";
			  	fin.close();
			  	fout.close();
			  }
	      }
        }
        else{
          cout << "Error: \"" << *it <<"\": syntax error\n";
        }
      }
      else
        cout << "Error: \"" << *it <<"\": syntax error\n";
    }
    else if(!it->compare("commit")) {
      if(!inUseDB.compare(""))
        cout << "Error: No database currently in use\n";
      else if(++it == words.end()) {
        long temp;
        ifstream fin;
        fin.open(inUseDB + "/locked.txt");
        if(fin.good()) {
        	fin >> temp;
        }
        else {
        	break;
        }
        fin.close();
        if(temp == ms.count()) {
        	cout << "Transaction committed.\n";
        	glob_t tables;
			glob(("./" + inUseDB + "/" + "*-1").c_str(), 0, NULL, &tables);
			for(size_t i = 0; i < tables.gl_pathc; i++) {
  				string temp2 = tables.gl_pathv[i];
  				remove(temp2.c_str());
			}
			remove(("./" + inUseDB + "/" + "locked.txt").c_str());
        }
        else {
        	cout << "Transaction abort.\n";
        }
      }
      else
      	cout << "Error: syntax error\n";
    }
    else if(!it->compare(".exit")) {
      cout << "All done.\n";
      break;
    }
    else
    	cout << "Error: \"" << *it <<"\": syntax error\n";
  }

  cout << "\n";
  inputFile.close();
  return 0;
}

// Seperates the words in a string into a list of substrings by spaces.
list<string> SeperateWords(string input) {
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

// Returns true if there exists a database in the list with the given name.
bool CheckForName(list<Database> dbList, string name) {
  bool test = false;
  for (list<Database>::iterator jt = dbList.begin(); jt != dbList.end(); jt++)
    if(!jt->getDBName().compare(name))
      test = true;
  return test;
}

// Returns true if there exists a table with the given name in the currently in use database.
bool TableCheck (string name) {
  if(FILE *file = fopen(name.c_str(), "r")) {
    fclose(file);
    return true;
  }
  return false; 
}