#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sqlite3.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "db.h"

using namespace std;


struct dump_context {
    sqlite3* db;
    ofstream* dump_file;
};

struct col_info {
    string name;
    string type;
};


int tableinfoCallback(void *collector, int nr_of_cols, char **col_values, char **col_names) {
//    cerr << "debug: colname to be collected: ";
//    for( int i = 0; i < nr_of_cols; i++) {
//        cerr << " " << col_names[i] << "=" << col_values[i];
//    }
//    cerr << endl;

    col_info* ci = new col_info();
    ci->name = string(col_values[1]);
    ci->type = string(col_values[2]);
    ((vector<col_info*>*)collector)->push_back(ci);
    return 0;
}

int insstmtCallback(void *dump_file, int nr_of_cols, char **col_values, char **col_names) {
    char* insstmt = col_values[0];
//    cerr << "debug: insstmt: " << insstmt << endl;
    (*(ofstream*)dump_file) << insstmt << endl;
    return 0;
}

int tableCallback(void *dump_ctxt, int nr_of_cols, char **col_values, char **col_names) {
    int rc;

//    cerr << "debug: nrOfCols (expect 3) =" << nr_of_cols;
//    cerr << " " << col_names[0] << "=" << col_values[0];
//    cerr << " " << col_names[1] << "=" << col_values[1];
//    cerr << " " << col_names[2] << "=" << col_values[2];
//    cerr << endl;

    char* table_name = col_values[0];

    // create insert stmts
    string stmt_tableinfo = string("PRAGMA table_info(`") + table_name + "`)";

    vector<col_info*> target_col_infos(0);
    rc = sqlite3_exec(((dump_context*)dump_ctxt)->db, stmt_tableinfo.c_str(), tableinfoCallback, &target_col_infos, nullptr);
    failOnError(rc, SQLITE_OK, stmt_tableinfo);

    string stmtpart_col_names = "";
    string stmtpart_col_values = "";
//    cerr << "debug: cols of table " << table_name << ": ";
    int i = 0;
    for (auto &ci : target_col_infos) {
        if( i ) {
            stmtpart_col_names += ", ";
            stmtpart_col_values += ", ";
        }
        stmtpart_col_names += "\"" + ci->name + "\"";
        if( ci->type == "text" ) {
            stmtpart_col_values += "' || quote(\"" + ci->name + "\") || '";
        } else {
            stmtpart_col_values += "' || \"" + ci->name + "\" || '";
        }
//        cerr << " " << ci->name << " " << ci->type << ",";
        delete ci;
        i++;
    }
//    cerr << endl;
    string stmt_insstmts = string("SELECT 'INSERT INTO \"") + table_name + "\"(" + stmtpart_col_names + ")" \
                           " VALUES(" + stmtpart_col_values + ")' from \"" + table_name + "\"";
//    cerr << "debug: stmt to gather insstmts: " << stmt_insstmts << endl;

    rc = sqlite3_exec(((dump_context*)dump_ctxt)->db, stmt_insstmts.c_str(), insstmtCallback, ((dump_context*)dump_ctxt)->dump_file, nullptr);
    failOnError(rc, SQLITE_OK, stmt_insstmts);

    return 0;
}

void dump(sqlite3* db) {
    char tmpl_dump_name[] = "../tmp/dump.sql-XXXXXX";
    char *dump_name = mktemp(tmpl_dump_name);
    if( !strlen(dump_name) ) {
        cerr << "could not create dump file name" << endl;
        throw "could not create dump file name";
    }
    cerr << "debug: dump file name: " << dump_name << endl;
    ofstream dump_file;
    dump_file.open(dump_name);
    dump_file << "BEGIN TRANSACTION;" << endl;

    static const string stmt_tables = "SELECT \"name\", \"type\", \"sql\"" \
                                           "  FROM \"sqlite_master\"" \
                                           "  WHERE \"sql\" NOT NULL AND \"type\" == 'table'" \
                                           "  ORDER BY \"name\"";

    dump_context dump_ctxt;
    dump_ctxt.db = db;
    dump_ctxt.dump_file = &dump_file;

    int rc = sqlite3_exec(db, stmt_tables.c_str(), tableCallback, &dump_ctxt, nullptr);
    failOnError(rc, SQLITE_OK, stmt_tables);

    dump_file << "COMMIT;" << endl;
    dump_file.close();
    dump_file.close();
}
