*   *ID* A16LIKE1 DBCMD    changed on 1992-09-28-13.22.36 by ADMIN     *
file CONNECT ( KERN TEST !
CREATE TABLE T ( A CHAR (7) ) !
CREATE TABLE T1 ( B CHAR (8) ) !
INSERT T SET A = 'aaa'!
INSERT T1 SET B = 'sss'!
SELECT * FROM T, T1 WHERE A LIKE 'a%' !
PARS_THEN_EX!
DATA !
SELECT * FROM T, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
INSERT T SET A = 'abc'!
*
INSERT T SET A = 'cvd'!
*
DATA !
SELECT * FROM T, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
FETCH INTO :A, :B!
pars_execute!
CREATE TABLE T2 ( A CHAR (7) ascii) !
INSERT T2 SET A = 'aaa'!
SELECT * FROM T2, T1 WHERE A LIKE 'a%' !
PARS_THEN_EX!
DATA !
SELECT * FROM T2, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
INSERT T2 SET A = 'abc'!
INSERT T2 SET A = 'cvd'!
DATA !
SELECT * FROM T2, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
FETCH INTO :A, :B!
pars_execute !
rollback release !
