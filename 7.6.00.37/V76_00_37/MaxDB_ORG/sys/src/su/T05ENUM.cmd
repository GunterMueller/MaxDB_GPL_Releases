*   *ID* T05ENUM  DBCMD    changed on 1992-09-28-13.28.40 by ADMIN     *
 ***************************
 ***   T 0 5 E N U M     ***
 ***************************!
 *** Testfaelle fuer :          ***
 *** Enum-Rueckentschluesselung ***
 *** Enum-Links                 *** !
CREATE TABLE kern.p6 (k1 char(1) key RANGE in ('a','c','x'),
                    f1 char(1) not null RANGE in ('c','x') ) !
CREATE TABLE kern.p7   (k1 char(1) key RANGE in ('a','x'),
                    f1 char(1) not null RANGE in ('u','x','c')) !
CREATE TABLE kern.p8   (k1 char(1) key,
                    f1 char(1) not null) !
COMMIT WORK !
LOCK TABLE p6 , p7 IN EXCLUSIVE MODE !
INSERT INTO p6 SET k1 = 'a', f1 = 'c' !
INSERT INTO p6 SET k1 = 'c', f1 = 'c' !
INSERT INTO p6 SET k1 = 'x', f1 = 'x' !
INSERT INTO p7 SET k1 = 'x', f1 = 'x' !
INSERT INTO p7 SET k1 = 'a', f1 = 'c' !
INSERT INTO p8 SET k1 = 'x', f1 = 'x' !
INSERT INTO p8 SET k1 = 'a', f1 = 'c' !
COMMIT WORK !
 *** Enum-Rueckentschluesselung ! -505
DECLARE erg6 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2
       FROM p6 !
FETCH erg6 !
CLOSE erg6 !
DECLARE erg6 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2
       FROM p6
       WHERE p6.k1 = 'a' !
FETCH erg6 !
CLOSE erg6 !
DECLARE erg6 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2
       FROM p6
       WHERE p6.f1 < 'r' !
FETCH erg6 !
CLOSE erg6 !
DECLARE erg6 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2
       FROM p6
       WHERE p6.f1 < 'x' !
FETCH erg6 !
CLOSE erg6!
 *** Enum-Links ! -505
DECLARE erg7 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2,p7.k1 f3,p7.f1 f4
       FROM p6, p7
       WHERE p6.k1 = p7.k1 !
FETCH erg7 !
CLOSE erg7 !
DECLARE erg7 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2,p7.k1 f3,p7.f1 f4
       FROM p6, p7
       WHERE p6.k1 = p7.k1 AND
       p6.k1 = 'x' !
FETCH erg7 !
CLOSE erg7 !
DECLARE erg7 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2,p7.k1 f3,p7.f1 f4
       FROM p6, p7
       WHERE p6.k1 = p7.k1 AND
       p6.k1 = 'c' !
FETCH erg7 !
CLOSE erg7 !
DECLARE erg7 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2,p8.k1 f3,p8.f1 f4
       FROM p6, p8
       WHERE p6.k1 = p8.k1 !
FETCH erg7 !
CLOSE erg7 !
DECLARE erg7 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2,p8.k1 f3,p8.f1 f4
       FROM p6, p8
       WHERE p6.k1 = p8.k1 AND
       p6.k1 = 'x' !
FETCH erg7 !
CLOSE erg7 !
DECLARE erg7 CURSOR FOR SELECT p6.k1 f1,p6.f1 f2,p8.k1 f3,p8.f1 f4
       FROM p6, p8
       WHERE p6.k1 = p8.k1 AND
       p6.k1 = 'c' !
FETCH erg7 !
CLOSE erg7 !
