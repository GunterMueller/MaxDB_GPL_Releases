*   *ID* A17SEL12 DBCMD    changed on 1992-09-28-13.22.41 by ADMIN     *
file connect ( kern test !
CREATE TABLE T ( A CHAR (4) KEY, B CHAR (7), C FIXED (3))!
INSERT T VALUES ('AAA', 'ERTYUIO', 1)!
INSERT T VALUES ('AAB', 'ERTYUIO', 4)!
INSERT T VALUES ('CCC', 'Q'      , 1)!
SELECT * FROM T !
FETCH !
SELECT * FROM T ORDER BY C !
FETCH !
SELECT A, B FROM T ORDER BY C !
FETCH !
SELECT DISTINCT A, B FROM T ORDER BY C !
FETCH !
SELECT A, B FROM T ORDER BY 3 !
FETCH !
SELECT T1.A, T2.A, T1.B, T1.C FROM T T1, T T2 ORDER BY 4, 1, 2 !
FETCH !
SELECT * FROM T T1, T T2 ORDER BY T1.C !
FETCH !
SELECT T1.A, T2.A, T1.B FROM T T1, T T2 ORDER BY T1.C, 1, 2 !
FETCH !
drop table t !
COMMIT RELEASE !