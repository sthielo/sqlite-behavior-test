#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include "db.h"
#include "dummydatagen.h"
#include "snapshot.h"
#include "dump.h"

using namespace std;



string waitInput() {
    string input;
    getline(cin, input);
    cerr << "debug: received: " << input << endl;
    return input;
}


// ----------------------------------------------------------------------------
// main

int main(int argc, char** argv)
{
    sqlite3* db = nullptr;
    sqlite3* snapshot_db = nullptr;
    try
    {
        db = init_db();
    	fill_db(db);
        cerr << "debug: DONE initdb" << endl;
        cout << "DONE initdb" << endl;

        string cmd = waitInput();  // wait 'END' or 'CONTINUE' (any input) - give time to gather process stats
        for (int i = 0; 0 != cmd.compare("END"); i++) {

            // "snapshotting" from in-memory db to another in-memory db (using distinct file URLs) seems to be the trigger in go code, but not so in this c++ port => ???
        	snapshot_db = snapshot(db);

        	// some intensive db activity - makes memory leak more obvious in go code, but not so in this c++ port => ???
            dump(snapshot_db);

            int rc = sqlite3_close(snapshot_db);
            if( rc != 0 ) {
                cerr << "error closing snapshot: rc=" << rc << endl;
                throw "failed to close snapshot";
            }

            cerr << "debug: DONE iteration " << i << endl;
            cout << "DONE iteration " << i << endl;
            cmd = waitInput();  // wait 'END' or 'CONTINUE' (any input) - give time to gather process stats
        }
    }
    catch(...)
    {
        if(db != nullptr) {
            sqlite3_close(db);
        }
        if(snapshot_db != nullptr) {
            sqlite3_close(snapshot_db);
        }
    }
    return (0);
}


