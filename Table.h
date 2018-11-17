// File: pa1.cpp
// Author: Brandon Herpolsheimer
// Date: 4/14/2018
// Version: 1.01
// Description: Header file for the Table class.

#include <iostream>
#include <string>
#include <list>

// Class containing the necessary information and methods for a table;
class Table
{
public:
  Table(std::string inUseDB, std::string name, std::list<std::string> arguments);
  ~Table();
  std::string getDBName();
  std::string getTableName();
  void drop();
  static void insert(std::string inUseDB, std::string name, std::list<std::string> arguments);
  static void update(std::string inUseDB, std::string name, std::list<std::string> arguments);
  static void mydelete(std::string inUseDB, std::string name, std::list<std::string> arguments);
  static void subQuery(std::string inUseDB, std::string name, std::list<std::string> arguments, std::list<std::string> paramNames);
  static void innerJoin(std::string inUseDB, std::list<std::string> arguments);
  static void innerJoin2(std::string inUseDB, std::list<std::string> arguments);
  static void outerJoin(std::string inUseDB, std::list<std::string> arguments);

private:
  std::string dbName;
  std::string tableName;
};
