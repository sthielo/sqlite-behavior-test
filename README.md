# Tracking down a Memory Leak in Go/SQLite

_note:_ 
* _ported from https://github.com/sthielo/go-sqlite-memleak
to track down issue within sqlite directly_  
* _ported https://github.com/schollz/sqlite3dump 
  (or rather its port source: https://github.com/python/cpython/blob/3.6/Lib/sqlite3/dump.py)_

---

run `make test` - WARNING: long running - several minutes on my workstation

---

the memory behavior as in https://github.com/sthielo/go-sqlite-memleak cannot be observed in this ported project.

  => EITHER the port is somewhat different - good chance for my rusty c++ skills ;-)

  => OR the memory misbehavior originates in the go-part of the referenced project! 