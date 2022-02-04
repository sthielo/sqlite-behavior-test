#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include "db.h"

using namespace std;


sqlite3* snapshot(sqlite3* db) {
    sqlite3* snapshot;
    int rc = 0;
    char conn_str [200];
// ALTERNATIVE:   sprintf(conn_str, "file::memory:?mode=memory&cache=private", tmpnam(nullptr));
    sprintf(conn_str, "file:%s?mode=memory&cache=private", tmpnam(nullptr));
    rc = sqlite3_open(conn_str, &snapshot);
    if (rc) {
        cerr << "Error open snapshot DB " << sqlite3_errmsg(snapshot) << endl;
        throw "Failed to open snapshot DB";
    }

    exec_stmt(snapshot, "PRAGMA foreign_keys=0");
    exec_stmt(snapshot, "PRAGMA journal_mode=OFF");
    exec_stmt(snapshot, "PRAGMA locking_mode=EXCLUSIVE");
    exec_stmt(snapshot, "PRAGMA query_only=true");

    auto backup = sqlite3_backup_init(snapshot, "main", db, "main");
    if( ! backup ) {
        cerr << "Error sqlite3_backup_init " << sqlite3_errmsg(db) << endl;
        throw "Failed to sqlite3_backup_init";
    }
    rc = sqlite3_backup_step(backup, -1);
    failOnError(rc, SQLITE_DONE, "sqlite3_backup_step");
    rc = sqlite3_backup_finish(backup);
    failOnError(rc, SQLITE_OK, "sqlite3_backup_finish");
    return (snapshot);
}