#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>
#include "db.h"

using namespace std;


const int nrInT1 = 2500;
const int nrInT10 = 30;
const int nrInT11 = 1800;

static random_device dev;
static seed_seq seed{dev(), dev(), dev(), dev(), dev(), dev(), dev(), dev()};
static mt19937 rng(seed);
string genUuid() {
    uniform_int_distribution<int> dist(0, 15);

    static const char *v = "0123456789abcdef";
    static const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    string res = "";
    for (int i = 0; i < 16; i++) {
        if (dash[i]) {
            res += '-';
        }
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

static const int alphaNumericMaxLetter = 62;
static const int allCharsMaxLetter = 88;
static const int allCharsSpacesLineBreaksMaxLetter = 92;
string genString(int len, int maxLetter, vector<string> exclusions) {
    static const char *all_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789?! _-:;.,#*$(){}[]^'+%&/ \t\n\r";

    uniform_int_distribution<int> dist(0, maxLetter);

    string candidate = "";
    for (int i = 0; i < len; i++) {
        candidate += all_chars[dist(rng)];
    }

	if( exclusions.empty() || find(exclusions.begin(), exclusions.end(), candidate) == exclusions.end() ) {
		return (candidate);
	}
	return (genString(len, maxLetter, exclusions));
}

string genDate(int days) {
        const time_t t = time(0);   // get time now
        time_t res = t + 90000 * days;
        char buf[11] = "2001-01-01";
        strftime(buf, 11, "%Y-%m-%d", localtime(&res));
        return(string(buf,11));
}

double genDouble() {
	return rand();
}


void fill_db(sqlite3* db) {
    int rc;
    sqlite3_stmt* prepareInsT10 = nullptr;
    sqlite3_stmt* prepareInsT11 = nullptr;
    sqlite3_stmt* prepareInsT1 = nullptr;
    sqlite3_stmt* prepareInsT2 = nullptr;
    sqlite3_stmt* prepareInsT3 = nullptr;
    sqlite3_stmt* prepareInsT5 = nullptr;
    sqlite3_stmt* prepareInsT6 = nullptr;

    try {
        vector<string> t10Vals(nrInT10 + 1);
        t10Vals[0] = "aaa";

        const string prepareInsT10Stmt = "insert into t10 (id, t10f1, t10f2) values (?,'aaa',?)";
        rc = sqlite3_prepare_v2(db, prepareInsT10Stmt.c_str(), prepareInsT10Stmt.length(), &prepareInsT10, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT10Stmt);

        const string prepareInsT11Stmt = "insert into t11 (t10_id, t11f1, t11f2) values (?,?,?)";
        rc = sqlite3_prepare_v2(db, prepareInsT11Stmt.c_str(), prepareInsT11Stmt.length(), &prepareInsT11, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT11Stmt);

        for( int i = 1; i <= nrInT10; i++) {
    		const string t10Id = genUuid();
    		const string t10f2 = genString(3, alphaNumericMaxLetter, t10Vals);
    		t10Vals[i] = t10f2;
    		rc = sqlite3_bind_text(prepareInsT10, 1, t10Id.c_str(), t10Id.length(), SQLITE_TRANSIENT);
    		failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT10Stmt);
    		rc = sqlite3_bind_text(prepareInsT10, 2, t10f2.c_str(), t10f2.length(), SQLITE_TRANSIENT);
    		failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT10Stmt);
            rc = sqlite3_step(prepareInsT10);
       		failOnError(rc, SQLITE_DONE, "step-" + prepareInsT10Stmt);
       		rc = sqlite3_reset(prepareInsT10);
       		failOnError(rc, SQLITE_OK, "reset-" + prepareInsT10Stmt);

    		for( int j = 0; j < nrInT11; j++) {
    			const string t11f1 = genDate(-j);
    			const double t11f2 = genDouble();
                rc = sqlite3_bind_text(prepareInsT11, 1, t10Id.c_str(), t10Id.length(), SQLITE_TRANSIENT);
                failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT11Stmt);
                rc = sqlite3_bind_text(prepareInsT11, 2, t11f1.c_str(), t11f1.length(), SQLITE_TRANSIENT);
                failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT11Stmt);
                rc = sqlite3_bind_double(prepareInsT11, 3, t11f2);
                failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT11Stmt);
                rc = sqlite3_step(prepareInsT11);
                failOnError(rc, SQLITE_DONE, "step-" + prepareInsT11Stmt);
                rc = sqlite3_reset(prepareInsT11);
                failOnError(rc, SQLITE_OK, "reset-" + prepareInsT11Stmt);
    		}
        }

        const string prepareInsT1Stmt = "insert into t1 (id, t1f1, t1f2, t1f3) values (?,?,1,?)";
        rc = sqlite3_prepare_v2(db, prepareInsT1Stmt.c_str(), prepareInsT1Stmt.length(), &prepareInsT1, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT1Stmt);

        const string prepareInsT2Stmt = "insert into t2 (id, t2f1, t2f2) values (?,?,?)";
        rc = sqlite3_prepare_v2(db, prepareInsT2Stmt.c_str(), prepareInsT2Stmt.length(), &prepareInsT2, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT2Stmt);

        const string prepareInsT3Stmt = "insert into t3 (id, t3f1, t3f2, t3f3, t3f4, t3f5, t3f8) values (?,?,?,?,12,?,?)";
        rc = sqlite3_prepare_v2(db, prepareInsT3Stmt.c_str(), prepareInsT3Stmt.length(), &prepareInsT3, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT3Stmt);

        const string prepareInsT5Stmt = "insert into t5 (id, t1_id, t5f1, t5f2, t5f3, t5f4) values (?,?,1,?,?,'valC')";
        rc = sqlite3_prepare_v2(db, prepareInsT5Stmt.c_str(), prepareInsT5Stmt.length(), &prepareInsT5, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT5Stmt);

        const string prepareInsT6Stmt = "insert into t6 (t5_id, t6f1, t6f2) values (?,?,?)";
        rc = sqlite3_prepare_v2(db, prepareInsT6Stmt.c_str(), prepareInsT6Stmt.length(), &prepareInsT6, nullptr);
        failOnError(rc, SQLITE_OK, prepareInsT6Stmt);

    	vector<string> t2Vals(2*nrInT1);
        const vector<string> emptyStr = std::vector<std::string>();

    	const std::string lastUsageStr = genDate(-5);
    	for(int i = 0; i < nrInT1; i++) {
    		const std::string t1Id = genUuid();
    		const std::string t1f1 = genString(100, allCharsSpacesLineBreaksMaxLetter, emptyStr);
            rc = sqlite3_bind_text(prepareInsT1, 1, t1Id.c_str(), t1Id.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT1Stmt);
            rc = sqlite3_bind_text(prepareInsT1, 2, t1f1.c_str(), t1f1.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT1Stmt);
            rc = sqlite3_bind_text(prepareInsT1, 3, lastUsageStr.c_str(), lastUsageStr.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT1Stmt);
            rc = sqlite3_step(prepareInsT1);
            failOnError(rc, SQLITE_DONE, "step-" + prepareInsT1Stmt);
            rc = sqlite3_reset(prepareInsT1);
            failOnError(rc, SQLITE_OK, "reset-" + prepareInsT1Stmt);

    		std::string t2f2 = genString(13, alphaNumericMaxLetter, t2Vals);
    		t2Vals[2*i] = t2f2;
            rc = sqlite3_bind_text(prepareInsT2, 1, t1Id.c_str(), t1Id.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT2Stmt);
            rc = sqlite3_bind_text(prepareInsT2, 2, "valA", 5, SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT2Stmt);
            rc = sqlite3_bind_text(prepareInsT2, 3, t2f2.c_str(), t2f2.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT2Stmt);
            rc = sqlite3_step(prepareInsT2);
            failOnError(rc, SQLITE_DONE, "step-" + prepareInsT2Stmt);
            rc = sqlite3_reset(prepareInsT2);
            failOnError(rc, SQLITE_OK, "reset-" + prepareInsT2Stmt);

    		t2f2 = genString(12, alphaNumericMaxLetter, t2Vals);
    		t2Vals[2*i + 1] = t2f2;
            rc = sqlite3_bind_text(prepareInsT2, 1, t1Id.c_str(), t1Id.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT2Stmt);
            rc = sqlite3_bind_text(prepareInsT2, 2, "valB", 5, SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT2Stmt);
            rc = sqlite3_bind_text(prepareInsT2, 3, t2f2.c_str(), t2f2.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT2Stmt);
            rc = sqlite3_step(prepareInsT2);
            failOnError(rc, SQLITE_DONE, "step-" + prepareInsT2Stmt);
            rc = sqlite3_reset(prepareInsT2);
            failOnError(rc, SQLITE_OK, "reset-" + prepareInsT2Stmt);

    		const std::string t3f1 = genString(24, allCharsMaxLetter, emptyStr);
    		const std::string t3f2 = genString(24, allCharsMaxLetter, emptyStr);
    		const std::string t3f3 = genDate(10 + i);
    		const double t3f5 = genDouble();
    		const std::string t3f8 = genString(8, alphaNumericMaxLetter, emptyStr);
            rc = sqlite3_bind_text(prepareInsT3, 1, t1Id.c_str(), t1Id.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT3Stmt);
            rc = sqlite3_bind_text(prepareInsT3, 2, t3f1.c_str(), t3f1.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT3Stmt);
            rc = sqlite3_bind_text(prepareInsT3, 3, t3f2.c_str(), t3f2.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT3Stmt);
            rc = sqlite3_bind_text(prepareInsT3, 4, t3f3.c_str(), t3f3.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind4-" + prepareInsT3Stmt);
            rc = sqlite3_bind_double(prepareInsT3, 5, t3f5);
            failOnError(rc, SQLITE_OK, "bind5-" + prepareInsT3Stmt);
            rc = sqlite3_bind_text(prepareInsT3, 6, t3f8.c_str(), t3f8.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind6-" + prepareInsT3Stmt);
            rc = sqlite3_step(prepareInsT3);
            failOnError(rc, SQLITE_DONE, "step-" + prepareInsT3Stmt);
            rc = sqlite3_reset(prepareInsT3);
            failOnError(rc, SQLITE_OK, "reset-" + prepareInsT3Stmt);

    		const std::string t5Id = genUuid();
    		const std::string t5f2 = genDate(-i);
            rc = sqlite3_bind_text(prepareInsT5, 1, t5Id.c_str(), t5Id.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT5Stmt);
            rc = sqlite3_bind_text(prepareInsT5, 2, t1Id.c_str(), t1Id.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT5Stmt);
            rc = sqlite3_bind_text(prepareInsT5, 3, t5f2.c_str(), t5f2.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT5Stmt);
            rc = sqlite3_bind_text(prepareInsT5, 4, t2f2.c_str(), t2f2.length(), SQLITE_TRANSIENT);
            failOnError(rc, SQLITE_OK, "bind4-" + prepareInsT5Stmt);
            rc = sqlite3_step(prepareInsT5);
            failOnError(rc, SQLITE_DONE, "step-" + prepareInsT5Stmt);
            rc = sqlite3_reset(prepareInsT5);
            failOnError(rc, SQLITE_OK, "reset-" + prepareInsT5Stmt);

    		for( int j = 0; j < nrInT11; j++) {
    			const std::string t6f1 = genDate(-j);
    			const double t6f2 = genDouble();
                rc = sqlite3_bind_text(prepareInsT6, 1, t5Id.c_str(), t5Id.length(), SQLITE_TRANSIENT);
                failOnError(rc, SQLITE_OK, "bind1-" + prepareInsT6Stmt);
                rc = sqlite3_bind_text(prepareInsT6, 2, t6f1.c_str(), t6f1.length(), SQLITE_TRANSIENT);
                failOnError(rc, SQLITE_OK, "bind2-" + prepareInsT6Stmt);
                rc = sqlite3_bind_double(prepareInsT6, 3, t6f2);
                failOnError(rc, SQLITE_OK, "bind3-" + prepareInsT6Stmt);
                rc = sqlite3_step(prepareInsT6);
                failOnError(rc, SQLITE_DONE, "step-" + prepareInsT6Stmt);
                rc = sqlite3_reset(prepareInsT6);
                failOnError(rc, SQLITE_OK, "reset-" + prepareInsT6Stmt);
    		}
    	}
    }
    catch(...) {
        sqlite3_finalize(prepareInsT10);
        sqlite3_finalize(prepareInsT11);
        sqlite3_finalize(prepareInsT1);
        sqlite3_finalize(prepareInsT2);
        sqlite3_finalize(prepareInsT3);
        sqlite3_finalize(prepareInsT5);
        sqlite3_finalize(prepareInsT6);
        throw;
    }
    std::cerr << "debug: dummy data generated successfully" << std::endl;
}
