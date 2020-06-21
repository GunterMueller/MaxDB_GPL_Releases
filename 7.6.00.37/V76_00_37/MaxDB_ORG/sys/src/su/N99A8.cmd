*   *ID* N99A8    DBCMD    changed on 1992-09-28-13.27.23 by ADMIN     *
file connect ( kern test !
CREATE TABLE T1 ( A CHAR (18))!
INSERT T1 SET A = 'aaa'!
CREATE TABLE T2 ( B CHAR (5)) !
INSERT T2 SET B = 'aaa'!
CREATE INDEX "T2.B"
	on T2( B DESC ) !
EXPLAIN SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
fetch !
drop index "T2.B" !
create index "t2.b"
	on t2( b ) !
SELECT * FROM T1, T2 WHERE T1.A = T2.B !
fetch !
rollback !
CREATE TABLE T1 ( A CHAR (38))!
INSERT T1 SET A = 'aaa'!
CREATE TABLE T2 ( B CHAR (25)) !
INSERT T2 SET B = 'aaa'!
CREATE INDEX "T2.B"
	on T2( B DESC ) !
EXPLAIN SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
fetch !
drop index "T2.B" !
create index "t2.b"
	on t2( b ) !
SELECT * FROM T1, T2 WHERE T1.A = T2.B !
fetch !
rollback work release !
