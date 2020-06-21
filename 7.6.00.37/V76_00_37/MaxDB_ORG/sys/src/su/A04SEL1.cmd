*   *ID* A04SEL1  DBCMD    changed on 1992-09-28-13.22.25 by ADMIN     *
 ***************************
 ***   A 0 4 S E L 1     ***
 ***************************!
file connect ( kern test !
drop table kern.t1 !
drop table kern.t2 !
drop table kern.t3 !
CREATE TABLE kern.T1 ( F1 CHAR (3), F2 FIXED(5),F3 FLOAT(6) )!
CREATE TABLE kern.T2 ( F1 CHAR (3), F2 FIXED(5), F3 FLOAT(6) )!
CREATE TABLE kern.T3 ( T1 CHAR (3), T2 FIXED(5), T3 FLOAT(6) )!
INSERT kern.T1 VALUES ('a', 1, 40 ) !
INSERT kern.T2 VALUES ('a', 1, 40 ) !
INSERT kern.T3 VALUES ('a', 1, 40 ) !
INSERT kern.T1 VALUES ('b', 2, 50 ) !
INSERT kern.T2 VALUES ('b', 2, 50 ) !
INSERT kern.T3 VALUES ('b', 2, 50 ) !
COMMIT WORK !
SELECT T1.*, kern.T3 FROM kern.T3 , kern.T1
    WHERE T1.F1 = kern.T3.F1 !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
     WHERE T1.F1 = kern.T3.F1 !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
WHERE T1.F1 = kern.T3.T1 !
FETCH !
SELECT T1.*,kern.T2.*  FROM kern.T2, kern.T1
   WHERE T1.F1=kern.T2.F1 !
FETCH !
SELECT T1.*, kern.T2.*  FROM kern.T2 , kern.T1
   WHERE T1.F1 = kern.T2.F1 !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
    WHERE T1.F1 = kern.T3.F1 !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
WHERE T1.F1 = kern.T3.T1 !
FETCH  !
SELECT T1.*, kern.T3.* FROM kern.T3 , kern.T1
     WHERE T1.*  = kern.T3.T1 !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
     WHERE T1.*  = kern.T3.T1 !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
     WHERE T1.F1 = T3.*  !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
     WHERE T1.*  = T3.T1 !
COMMIT WORK RELEASE !
