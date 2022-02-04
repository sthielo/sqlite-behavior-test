#include <sqlite3.h>


void failOnError(int rc, int expectedRc, std::string stmt);
void exec_stmt(sqlite3* db, std::string stmt);
sqlite3* init_db();