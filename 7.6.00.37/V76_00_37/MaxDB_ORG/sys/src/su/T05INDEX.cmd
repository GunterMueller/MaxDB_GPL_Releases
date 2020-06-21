*   *ID* T05INDEX DBCMD    changed on 1992-09-28-13.28.40 by ADMIN     *
 ***************************
 ***   T 0 5 I N D E X   ***
 ***************************!
 *** Testfaelle fuer :                       ***
 *** Eine Invertierung                       ***
 *** Eine Invertierung mit Mehrfachkey       ***
 *** Zwei Invertierungen                     ***
 *** Teilweise aufwaerts- und abwaertssuchen ***
 *** IN-Operator ohne/mit Invertierungen     ***
 *** Select in Transaktionen nach Inserts    *** !
CREATE TABLE kern.p3 (k1 char(1) key,f1 char(1) not null,
                                    f2 char(1) not null) !
COMMIT WORK !
lock table p3 in exclusive mode !
INSERT INTO p3 set k1 = 'a', f1 = 'b', f2 = 'b' !
INSERT INTO p3 set k1 = 'b', f1 = 'b', f2 = 'c' !
INSERT INTO p3 set k1 = 'c', f1 = 'c', f2 = 'c' !
COMMIT WORK !
 *** Eine Invertierung ! -505
DECLARE erg1 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' !
FETCH erg1 !
CLOSE erg1 !
CREATE INDEX "kern.p3.f1"
	on kern.p3( f1 ) !
COMMIT WORK !
DECLARE erg1 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' !
FETCH erg1 !
CLOSE erg1 !
INSERT INTO p3 set k1 = 'd', f1 = 'c', f2 = 'c' !
COMMIT WORK !
DECLARE erg1 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' !
FETCH erg1 !
CLOSE erg1 !
DROP index "kern.p3.f1" !
DECLARE erg1 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' !
FETCH erg1 !
CLOSE erg1 !
COMMIT WORK !
 *** Eine Invertierung mit Mehrfachkey ! -505
CREATE TABLE kern.p4       (k1 char(1) key,
                       k2 char(1) key,
                       k3 char(1) key,
                       f1 char(1) not null,
                       f2 char(1)  not null) !
COMMIT WORK !
INSERT INTO p4 set k1 = 'a', k2 = 'a', k3 = 'a',
         f1 = 'b', f2 = 'b' !
INSERT INTO p4 set k1 = 'a', k2 = 'b', k3 = 'c',
         f1 = 's', f2 = 'b' !
INSERT INTO p4 set k1 = 'x', k2 = 'y', k3 = 'z',
         f1 = 'r', f2 = 'b' !
INSERT INTO p4 set k1 = 'u', k2 = 'y', k3 = 'z',
         f1 = 'a', f2 = 'b' !
COMMIT WORK !
CREATE INDEX "kern.p4.f2"
	on kern.p4( f2 ) !
COMMIT WORK !
DECLARE erg1 CURSOR FOR SELECT k1,k2,k3,f1,f2
       from p4
       where p4.f2 = 'b' !
FETCH erg1 !
CLOSE erg1 !
 *** Zwei Invertierungen ! -505
DECLARE erg2 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' and
       p3.f2 = 'c' !
FETCH erg2 !
CLOSE erg2 !
CREATE INDEX "kern.p3.f1"
	on kern.p3( f1 ) !
CREATE INDEX "kern.p3.f2"
	on kern.p3( f2 ) !
COMMIT WORK !
DECLARE erg2 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' and
       p3.f2 = 'c' !
FETCH erg2 !
CLOSE erg2 !
DROP index "kern.p3.f1" !
COMMIT WORK !
DECLARE erg2 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' and
       p3.f2 = 'c' !
FETCH erg2 !
CLOSE erg2 !
DROP index "kern.p3.f2" !
COMMIT WORK !
DECLARE erg2 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 = 'c' and
       p3.f2 = 'c' !
FETCH erg2 !
CLOSE erg2 !
 *** Teilweise aufwaerts- und abwaertssuchen ! -505
DECLARE erg3 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.k1 > 'b' !
FETCH erg3 !
CLOSE erg3 !
DECLARE erg3 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.k1 >= 'b' !
FETCH erg3 !
CLOSE erg3 !
DECLARE erg3 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.k1 < 'c' !
FETCH erg3 !
CLOSE erg3 !
DECLARE erg3 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.k1 <= 'c' !
FETCH erg3 !
CLOSE erg3 !
DECLARE erg3 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.k1 > 'a' and
       p3.k1 < 'd' !
FETCH erg3 !
CLOSE erg3 !
 *** IN-Operator ohne/mit Invertierungen ! -505
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 IN ('b','c') !
FETCH erg4 !
CLOSE erg4 !
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 NOT IN ('c') !
FETCH erg4 !
CLOSE erg4 !
CREATE INDEX "kern.p3.f1"
	on kern.p3( f1 ) !
COMMIT WORK !
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 IN ('b','c') !
FETCH erg4 !
CLOSE erg4 !
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 NOT IN ('c') !
FETCH erg4 !
CLOSE erg4 !
CREATE INDEX "kern.p3.f2"
	on kern.p3( f2 ) !
COMMIT WORK !
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 IN ('c') AND
             p3.f2 IN ('c') !
FETCH erg4 !
CLOSE erg4 !
DROP index "kern.p3.f1" !
COMMIT WORK !
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 IN ('c') AND
             p3.f2 IN ('c') !
FETCH erg4 !
CLOSE erg4 !
DROP index "kern.p3.f2" !
COMMIT WORK !
DECLARE erg4 CURSOR FOR SELECT k1,f1,f2
       from p3
       where p3.f1 IN ('c') AND
             p3.f2 IN ('c') !
FETCH erg4 !
CLOSE erg4 !
COMMIT WORK !
 *** Select in Transaktionen nach Inserts !
CREATE TABLE kern.p5      (k1 char(1) key,
                      f1 char(1) not null) !
CREATE INDEX "kern.p5.f1"
	on kern.p5( f1 ) !
COMMIT WORK !
INSERT INTO p5 set k1 = 'u', f1 = 'u' !
DECLARE erg5 CURSOR FOR SELECT k1,f1
       from p5
       where f1 = 'u' !
FETCH erg5 !
CLOSE erg5 !
COMMIT WORK !
INSERT INTO p5 set k1 = 'x', f1 = 'u' !
DECLARE erg5 CURSOR FOR SELECT k1,f1
       from p5
       where f1 = 'u' !
FETCH erg5 !
CLOSE erg5 !
COMMIT WORK !
DECLARE erg5 CURSOR FOR SELECT k1,f1
       from p5
       where f1 = 'u' !
FETCH erg5 !
CLOSE erg5 !
