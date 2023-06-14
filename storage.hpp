#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include "command.hpp"
#include "sqlite3.h"

class SqliteStore : public ICommand<SqliteStore> {
 private:
  static inline sqlite3* handle{nullptr};
  static inline const char* db_name{":memory:"};
  //size_t buff_size_{1024};

  SqliteStore() {
  }
  void initSqlite() {
    if (sqlite3_open(db_name, &handle)) {
      throw std::runtime_error{"SQlite open error"};
    }
    char* errmsg;
    int rc = sqlite3_exec(handle,
                          "CREATE TABLE a (id INTEGER PRIMARY KEY, label TEXT);"
                          "CREATE TABLE b (id INTEGER PRIMARY KEY, label TEXT);",
                          nullptr, 0, &errmsg);
    if (rc != SQLITE_OK) {
      // std::cerr << "Can't execute query: " << errmsg << std::endl;
      sqlite3_free(errmsg);
      sqlite3_close(handle);
      throw std::runtime_error{"SQlite error creating tables"};
    }
  }

 protected:
  ~SqliteStore() { sqlite3_close(handle); }

 public:
  void Delete() override { delete this; }
  size_t process(char* buff, size_t length) override {
    if(!handle)
      initSqlite();   
    std::ostringstream ss;
    auto print_results = [](void* pss, int columns, char** data,
                            char** names) -> int {
      for (int i = 0; i < columns; ++i) {
        *(static_cast<std::ostringstream *>(pss)) <<
            (data[i] ? data[i] : "") << (i < columns-1 ? ',' : '\n');
      }
      return 0;
    };

    char* errmsg;
    int rc = sqlite3_exec(handle, buff, print_results, &ss, &errmsg);
    if(rc != SQLITE_OK) {
        strcpy(buff, errmsg);
        sqlite3_free(errmsg);
    } else {
        strcpy(buff, ss.str().c_str());
        strcat(buff, "OK");
    }
    strcat(buff, "\n");
    return strlen(buff);
  }

  friend ICommand* ICommand::createInstance();
};