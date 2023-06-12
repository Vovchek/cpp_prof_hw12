#pragma once

#include <mem.h>
#include <memory>

#include "command.hpp"
#include "storage.hpp"

class Net2SQL : public ICommand<Net2SQL> {
private:
    ICommand<SqliteStore> *db{nullptr};

    Net2SQL() {
        db = SqliteStore::createInstance();
    }

protected:
    ~Net2SQL() {};

public:
    void Delete() override {delete this;}
    size_t process(char *buff, size_t length) override {
        const char *commands[] = {"INSERT", "TRUNCATE","INTERSECTION","SYMMETRIC_DIFFERENCE"};
        int ec = 0;
        // translate input -> SQL
        std::string sql;
        char *ptr = std::strtok(buff, " ");
        if(!ptr)
            return errMsg(-1, buff);
        if(!_strnicmp(ptr, commands[0], strlen(commands[0]))) { // INSERT
            ptr = strtok(nullptr, " ");
            if(tolower(*ptr) != 'a' && tolower(*ptr) != 'b' || ptr[1] != '\0')
                return errMsg(1, buff);
            sql = "INSERT INTO ";
            sql += ptr;
            sql += " VALUES(";
            ptr = strtok(nullptr, " ");
            for(auto i = 0; ptr[i] != '\0'; ++i)
                if(!isdigit(ptr[i]))
                    return errMsg(2, buff);
            sql += ptr;
            sql += ", '";
            ptr = strtok(nullptr, "\n");
            sql += ptr;
            sql += "');";

        } else if(!_strnicmp(buff, commands[1], strlen(commands[1]))) { // TRUCNCATE
            ptr = strtok(nullptr, " \n");
            if(tolower(*ptr) != 'a' && tolower(*ptr) != 'b' || ptr[1] != '\0')
                return errMsg(1, buff);
            sql = "DELETE FROM ";
            sql += ptr;
            sql += ";";
        } else if(!_strnicmp(ptr, commands[2], strlen(commands[2]))) { // INTERSECTION
            sql = "SELECT a.id, a.label, b.label FROM a, b WHERE a.id=b.id;";
        } else if(!_strnicmp(ptr, commands[3], strlen(commands[3]))) { // SYMMETRIC_DIFFERENCE
            sql = "SELECT id, label la, NULL lb FROM a WHERE id NOT IN (SELECT id FROM b) UNION SELECT id, NULL,label FROM b WHERE id NOT IN (SELECT id FROM a);";
        } else {
            return errMsg(-1, buff);
        }

        // send request to database and get response
        strcpy(buff, sql.c_str());
        length = db->process(buff, length);

        return length;
    }
    friend ICommand* ICommand::createInstance();

private:
    size_t errMsg(int ec, char *buff) {
        const char* err_msg[] = {"OK\n", "INVALID TABLE NAME\n", "id NOT AN INTEGER\n", "UNKNOWN COMMAND\n"};
        constexpr size_t max_err = sizeof(err_msg) / sizeof(err_msg[0]) - 1;
        size_t err_num;
        switch(ec) {
            case 0:
                err_num = 0;
                break;
            case 1:
                err_num = 1;
                break;
            case 2:
                err_num = 2;
                break;
            default:
                err_num = max_err;
                break;
        }
        auto len = strlen(err_msg[err_num]);
        memcpy(buff, err_msg[err_num], len);
        return len;
    }
};
