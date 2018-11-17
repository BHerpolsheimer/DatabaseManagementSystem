// File: Database.h
// Author: Brandon Herpolsheimer
// Date: 3/22/2018
// Version: 1.01
// Description: Header file for the Database class.

#include <string>

// Class containing the necessary information and methods for a database;
class Database
{
public:
  Database(std::string name);
  ~Database();
  std::string getDBName();
  void drop();

private:
  std::string dbName;
};
