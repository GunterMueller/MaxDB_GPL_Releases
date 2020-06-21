*   *ID* N18LOCK4 DBCMD    changed on 1992-09-28-13.25.28 by ADMIN     *
file connect ( kern test !
* Kernprotokoll 204
CREATE TABLE T01 (
S11 FIXED(2) KEY,
S12 FIXED(1) KEY,
S13 FIXED(1) KEY,
S14 FIXED(1) KEY)!
PARSING!
LOCK (NOWAIT) ROW T01
KEY S11 = :A, S12 = :B, S13 = :C, S14 = :D
IN EXCLUSIVE MODE!
EXECUTING!
DATA!
LOCK!
1
n 2 1
* der naechste Wert passt nicht zum Datentyp fixed (1)
n 4 1991
n 1 1
n 1 1

nodata !
pars_execute !
drop table t01 !
commit release !
