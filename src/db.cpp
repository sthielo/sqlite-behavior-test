#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>

using namespace std;



// ----------------------------------------------------------------------------
// init db schema

void failOnError(int rc, int expectedRc, string stmt) {
    if( rc != expectedRc ) {
        cerr << "SQL err for stmt: " << stmt << " - rc: " << rc << endl;
        throw "failed : " + stmt;
    }
}

void exec_stmt(sqlite3* db, string stmt) {
    char* errmsg;
    int rc = 0;
    rc = sqlite3_exec(db, stmt.c_str(), NULL, 0, &errmsg);

   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n  stmt: %s\n", errmsg, stmt.c_str());
      sqlite3_free(errmsg);
      throw "failed schema creation";
   }
}


void create_schema(sqlite3* db) {
    exec_stmt(db, "PRAGMA foreign_keys=1");
    exec_stmt(db, "PRAGMA journal_mode=OFF");
    exec_stmt(db, "PRAGMA locking_mode=EXCLUSIVE");

	exec_stmt(db, "create table t1 (" \
                  "  id text not null primary key CHECK (id like '________-____-____-____-____________')," \
                  "  t1f1 text," \
                  "  t1f2 integer not null check(t1f2 in(0, 1))," \
                  "  t1f3 text not null default (date('now')) CHECK (t1f3 like '____-__-__')" \
                  ") without rowid");

	exec_stmt(db, "create table t2 (" \
                  "  id text not null CHECK (id like '________-____-____-____-____________')," \
                  "  t2f1 text not null," \
                  "  t2f2 text not null primary key," \
                  "  foreign key(id) references t1(id) on delete cascade deferrable initially deferred" \
                  ") without rowid");

	exec_stmt(db, "create table t3 (" \
                  "  id text not null primary key CHECK (id like '________-____-____-____-____________')," \
                  "  t3f1 text," \
                  "  t3f2 text," \
                  "  t3f3 text CHECK (t3f3 like '____-__-__')," \
                  "  t3f4 integer," \
                  "  t3f5 real," \
                  "  t3f8 text," \
                  "  foreign key(id) references t1(id) on delete cascade deferrable initially deferred" \
                  ") without rowid");

	exec_stmt(db, "create table t5 (" \
                  "  id text not null primary key CHECK (id like '________-____-____-____-____________')," \
                  "  t1_id text not null," \
                  "  t5f1 integer not null check(t5f1 in(0, 1))," \
                  "  t5f2 text not null CHECK (t5f2 like '____-__-__')," \
                  "  t5f3 text," \
                  "  t5f4 text not null check(t5f4 in('valA', 'valB', 'valC'))," \
                  "  foreign key(t1_id) references t1(id) on delete cascade deferrable initially deferred," \
                  "  unique(t1_id, t5f4, t5f3)" \
                  ") without rowid");

	exec_stmt(db, "create table t6 (" \
                  "  t5_id text not null CHECK (t5_id like '________-____-____-____-____________')," \
                  "  t6f1 text not null CHECK (t6f1 like '____-__-__')," \
                  "  t6f2 real," \
                  "  primary key(t5_id, t6f1)," \
                  "  foreign key(t5_id) references t5(id) on delete cascade deferrable initially deferred" \
                  ") without rowid");

	exec_stmt(db, "create table t10 (" \
                  "  id text not null CHECK (id like '________-____-____-____-____________')," \
                  "  t10f1 text not null," \
                  "  t10f2 text not null," \
                  "  primary key(id)," \
                  "  unique (t10f1, t10f2)" \
                  ") without rowid");

	exec_stmt(db, "create table t11 (" \
                  "  t10_id text not null CHECK (t10_id like '________-____-____-____-____________')," \
                  "  t11f1 text not null CHECK (t11f1 like '____-__-__')," \
                  "  t11f2 real not null," \
                  "  primary key(t10_id, t11f1)," \
                  "  foreign key(t10_id) references t10(id) on delete cascade deferrable initially deferred" \
                  ") without rowid");

    cerr << "debug: Schema created successfully" << endl;
}

sqlite3* init_db() {
    int rc = 0;
    rc = sqlite3_config(SQLITE_CONFIG_URI, 1);
    if (rc) {
        cerr << "Error sqlite cfg SQLITE_CONFIG_URI: rc=" << rc << endl;
        throw "Failed to config SQLITE_CONFIG_URI";
    }

    // corresponds to URI param: _mutex=no - see: SQLITE_OPEN_NOMUTEX in https://www.sqlite.org/c3ref/open.html
    rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    if (rc) {
        cerr << "Error sqlite cfg SQLITE_CONFIG_MULTITHREAD: rc=" << rc << endl;
        throw "Failed to config SQLITE_CONFIG_MULTITHREAD";
    }
    sqlite3* db;
    char conn_str [200];
    sprintf(conn_str, "file:%s?mode=memory&cache=private", tmpnam(nullptr));
    rc = sqlite3_open(conn_str, &db);
    if (rc) {
        cerr << "Error open DB: rc=" << rc << " - errmsg: " << sqlite3_errmsg(db) << endl;
        throw "Failed to open DB";
    }

	create_schema(db);
    return db;
}
