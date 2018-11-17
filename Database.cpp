// File: Database.cpp
// Author: Brandon Herpolsheimer
// Date: 5/08/2018
// Version: 1.03
// Description: Implementation file for the Database class.

#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include "Database.h"

using namespace std;

// Parameterized constructor for Database sets the name value and creates a corresponding directory.
Database::Database(string name) {
  dbName = name;
  mkdir(("./" + name).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

// Deconstructor for database. Currently nothing here.
Database::~Database() {

}

// Returns the name of the database.
string Database::getDBName() {
  return dbName;
}

// Deletes all tables within the database as well as the dabase directory itself.
void Database::drop() {
  glob_t tables;
  glob(("./" + dbName + "/*").c_str(), 0, NULL, &tables);
  for(size_t i = 0; i < tables.gl_pathc; i++) {
  	string temp = tables.gl_pathv[i];
  	remove(temp.c_str());
  }
  rmdir(("./" + dbName).c_str());
}