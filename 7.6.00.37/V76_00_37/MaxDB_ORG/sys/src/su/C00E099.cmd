*   *ID* C00E099  DBCMD    changed on 1992-09-28-13.23.03 by ADMIN     *
*************************************************************************
* JOIN via TABLE SCAN liefert ROW NOT FOUND
* JOIN mit multiplen Index liefert das gewuenschte Ergebnis
*************************************************************************
file connect ( kern test !
*
CREATE TABLE t01 (s11 CHAR(8) KEY, s12 CHAR(18) KEY)!
CREATE TABLE t02 (s21 CHAR(8) KEY, s22 CHAR(5)  KEY, s23 FIXED(3) KEY,
                  s24 CHAR(12),    s25 CHAR(18))!
COMMIT!
*
INSERT t01 VALUES ('AAAAAAA','BBBBBB')!
INSERT t02 VALUES ('AAAAAAA','CCCC',4,'AAAAAAA','BBBBBB')!
COMMIT!
*
explain SELECT * FROM t01, t02
  WHERE s11 = s24
    AND s12 = s25
    AND s22 = 'CCCC'
    AND s25 = 'BBBBBB'
    AND s24 = 'AAAAAAA'
    AND s11 = 'AAAAAAA'
    AND s12 = 'BBBBBB'!
SELECT * FROM t01, t02
  WHERE s11 = s24
    AND s12 = s25
    AND s22 = 'CCCC'
    AND s25 = 'BBBBBB'
    AND s24 = 'AAAAAAA'
    AND s11 = 'AAAAAAA'
    AND s12 = 'BBBBBB'!
FETCH !
commit release!
