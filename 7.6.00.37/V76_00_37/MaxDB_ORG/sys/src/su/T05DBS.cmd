*   *ID* T05DBS   DBCMD    changed on 1992-09-28-13.28.39 by ADMIN     *
 ***************************
 ***   T 0 5 D B S       ***
 ***************************!
CREATE TABLE X ( Y CHAR (1) key RANGE BETWEEN 'A' AND 'L' ,
                     A CHAR (7) NOT NULL,
                     B FIXED (3) NOT NULL ) !
COMMIT WORK !
INSERT INTO X
   SET Y = 'B',
       A = 'abc',
       B = 76       !
LOCK TABLE X IN SHARE MODE  !
LOCK (nowait) TABLE X IN SHARE MODE  !
COMMIT WORK !
LOCK (nowait) TABLE X ROW X KEY   Y = 'B'   IN EXCLUSIVE MODE !
UPDATE OF X
   SET Y = 'B'
   KEY A = 'xyz'  !
COMMIT WORK !
 *** DML_KOMMANDOS !
CREATE TABLE p1
       ( k1 CHAR (5) KEY,
         f1 CHAR (20) NOT NULL )  !
COMMIT WORK !
INSERT INTO p1
   SET k1 = 'abcde',
       f1 = 'xyz12345'  !
COMMIT WORK !
INSERT INTO p1
   SET k1 = 'abcde',
       f1 = 'xyz12345'  !
LOCK ROW p1 KEY   k1 = 'abcde'   IN EXCLUSIVE MODE !
LOCK ROW p1 KEY   k1 = 'abcde'   IN EXCLUSIVE MODE !
UPDATE OF p1
   SET f1 = '2XCVB'
   KEY k1 = 'abcde' !
UPDATE OF p1
   SET f1 = 'ZXCVB'
   KEY k1 = 'vwxyz' !
ROLLBACK WORK !
INSERT INTO p1
   SET k1 = 'abcde',
       f1 = 'xyz12345'  !
INSERT INTO p1
   SET k1 = 'abcde',
       f1 = 'xyz12345'  !
COMMIT WORK !
LOCK ROW p1 KEY   k1 = 'abcde'   IN EXCLUSIVE MODE !
DELETE FROM p1
   KEY k1 = 'abcde'  !
DELETE FROM p1
   KEY k1 = 'abcde'  !
COMMIT WORK !
COMMIT WORK !
CREATE TABLE p2
       ( k1 CHAR (5) KEY,
         f1 CHAR (1) NOT NULL ) !
COMMIT WORK !
LOCK TABLE p2 IN EXCLUSIVE MODE !
INSERT INTO p2
   SET k1 = 'abcde',
       f1 = 'a'        !
INSERT INTO p2
   SET k1 = 'ccccc',
       f1 = 'c'        !
INSERT INTO p2
   SET k1 = 'xxxxx',
       f1 = 'x'        !
INSERT INTO p2
   SET k1 = 'cdefg',
       f1 = 'c'        !
INSERT INTO p2
   SET k1 = 'rlyyy',
       f1 = 'r'        !
COMMIT WORK !
DELETE FROM p2
   KEY k1 = 'abcde' !
LOCK TABLE p2 IN EXCLUSIVE MODE  !
DELETE FROM p2
   KEY k1 = 'abcde'  !
DELETE FROM p2
   KEY k1 = 'xxxxx'  !
DELETE FROM p2
   KEY k1 = 'ccccc'  !
DELETE FROM p2
   KEY k1 = 'cdefg'  !
DELETE FROM p2
   KEY k1 = 'rlyyy'  !
COMMIT WORK !
