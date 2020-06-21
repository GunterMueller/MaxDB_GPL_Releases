*   *ID* MAXSTACK DBCMD    changed on 1992-09-28-13.24.38 by ADMIN     *
file CONNECT ( kern test !
create table t ( k char (8) key, b fixed(8), c char (6))!
create index "t.c"
	on t( c ) !
insert t set k = 'aaa', b = 1, c = 'aaa' !
insert t set k = 'bbb', b = 3, c = 'bbb' !
create table t1 (a1 char (7), b1 char (8))!
insert t1 values ('aaa', 'aaa')!
insert t1 values ('bbb', 'bbb')!
create index "t1.b1"
	on t1( b1 ) !
select sum(b), k
       from t, t1
       where k = any ( select a1 from t1 where t.k = t1.b1)
         and (c = 'bbb' or c = 'aaa')
         and (c = 'bbb' or c = 'aaa')
         and (k = 'aaa' or k = 'bbb')
         and t.k = t1.a1
       group by k !
select sum(b), k
       from t, t1
       where k = any ( select a1 from t1 where t.k = t1.b1)
         and (c = 'bbb' or c = 'aaa')
         and (c = 'bbb' or c = 'aaa')
         and (k = 'aaa' or k = 'bbb')
         and t.c = t1.b1
       group by k !
commit release !
file cleark !
file CONNECT ( kern test !
create table s ( sno char(5) key,
                 sname char(20),
                 status decimal(3),
                 city char(15) ) !
create table p ( pno char(6) key,
                 pname char(20),
                 color char(6),
                 weight decimal(3),
                 city char(15) ) !
create table j ( jno char(4) key,
                 jname char(10),
                 city char(15) ) !
create table spj ( sno char(5) key,
                   pno char(6) key,
                   jno char(4) key,
                   qty decimal(5)) !
commit work !
insert s values ( 'S1' , 'Smith' , 20 , 'London' ) !
insert s values ( 'S2' , 'Jones' , 10 , 'Paris'  ) !
insert s values ( 'S3' , 'Blake' , 30 , 'Paris'  ) !
insert s values ( 'S4' , 'Clark' , 20 , 'London' ) !
insert s values ( 'S5' , 'Adams' , 30 , 'Athens' ) !
commit work !
insert p values ( 'P1' , 'Nut'   , 'Red'   , 12 , 'London' ) !
insert p values ( 'P2' , 'Bolt'  , 'Green' , 17 , 'Paris'  ) !
insert p values ( 'P3' , 'Screw' , 'Blue'  , 17 , 'Rome'   ) !
insert p values ( 'P4' , 'Screw' , 'Red'   , 14 , 'London' ) !
insert p values ( 'P5' , 'Cam'   , 'Blue'  , 12 , 'Paris'  ) !
insert p values ( 'P6' , 'Cog'   , 'Red'   , 19 , 'London' ) !
commit work !
insert j values ( 'J1' , 'Sorter'   , 'Paris'  ) !
insert j values ( 'J2' , 'Punch'    , 'Rome'   ) !
insert j values ( 'J3' , 'Reader'   , 'Athens' ) !
insert j values ( 'J4' , 'Console'  , 'Athens' ) !
insert j values ( 'J5' , 'Collator' , 'London' ) !
insert j values ( 'J6' , 'Terminal' , 'Oslo'   ) !
insert j values ( 'J7' , 'Tape'     , 'London' ) !
commit work !
insert spj values ( 'S1' , 'P1' , 'J1' , 200 ) !
insert spj values ( 'S1' , 'P1' , 'J4' , 700 ) !
insert spj values ( 'S2' , 'P3' , 'J1' , 400 ) !
insert spj values ( 'S2' , 'P3' , 'J2' , 200 ) !
insert spj values ( 'S2' , 'P3' , 'J3' , 200 ) !
insert spj values ( 'S2' , 'P3' , 'J4' , 500 ) !
insert spj values ( 'S2' , 'P3' , 'J5' , 600 ) !
insert spj values ( 'S2' , 'P3' , 'J6' , 400 ) !
insert spj values ( 'S2' , 'P3' , 'J7' , 800 ) !
insert spj values ( 'S2' , 'P5' , 'J2' , 100 ) !
insert spj values ( 'S3' , 'P3' , 'J1' , 200 ) !
insert spj values ( 'S3' , 'P4' , 'J2' , 500 ) !
insert spj values ( 'S4' , 'P6' , 'J3' , 300 ) !
insert spj values ( 'S4' , 'P6' , 'J7' , 300 ) !
insert spj values ( 'S5' , 'P2' , 'J2' , 200 ) !
insert spj values ( 'S5' , 'P2' , 'J4' , 100 ) !
insert spj values ( 'S5' , 'P5' , 'J5' , 500 ) !
insert spj values ( 'S5' , 'P5' , 'J7' , 100 ) !
insert spj values ( 'S5' , 'P6' , 'J2' , 200 ) !
insert spj values ( 'S5' , 'P1' , 'J4' , 100 ) !
insert spj values ( 'S5' , 'P3' , 'J4' , 200 ) !
insert spj values ( 'S5' , 'P4' , 'J4' , 800 ) !
insert spj values ( 'S5' , 'P5' , 'J4' , 400 ) !
insert spj values ( 'S5' , 'P6' , 'J4' , 500 ) !
commit work !
select * from s !
fetch !
select * from p !
fetch !
select * from j !
fetch !
select * from spj !
fetch !
SELECT DISTINCT SPJX.SNO
   FROM  SPJ SPJX
   WHERE EXISTS
       ( SELECT SPJY.PNO
         FROM   SPJ SPJY
         WHERE NOT EXISTS
             ( SELECT J.JNO
               FROM   J
               WHERE  NOT EXISTS
                    ( SELECT *
                      FROM SPJ SPJZ
                      WHERE SPJZ.SNO = SPJX.SNO
                      AND   SPJZ.PNO = SPJY.PNO
                      AND   SPJZ.JNO = J.JNO ))) !
SELECT DISTINCT SPJX.SNO
   FROM  SPJ SPJX
   WHERE EXISTS
       ( SELECT SPJY.PNO
         FROM   SPJ SPJY
         WHERE NOT EXISTS
             ( SELECT J.JNO
               FROM   J
               WHERE  NOT EXISTS
                    ( SELECT *
                      FROM SPJ SPJZ, J JREF
                      WHERE SPJZ.SNO = SPJX.SNO
                      AND   SPJZ.PNO = SPJY.PNO
                      AND   SPJZ.JNO = JREF.JNO
                      AND   JREF.JNO = J.JNO
                      AND   SPJZ.JNO = J.JNO ))) !
SELECT MAX(QTY) - MIN(QTY)
   FROM P PX, SPJ SPJX
   WHERE PX.PNO = SPJX.PNO
     AND SPJX.SNO =
   ANY ( SELECT S.SNO
         FROM P, SPJ, S, J
         WHERE P.PNO = SPJX.PNO
           AND S.CITY = J.CITY
           AND PX.CITY = S.CITY
         GROUP BY S.SNO )
   GROUP BY QTY !
SELECT SPJX.SNO, SPJX.PNO, SPJX.JNO, S.SNAME
  FROM SPJ SPJX, S, J JX
  WHERE S.STATUS <
    ALL ( SELECT MAX(QTY) - MIN(QTY)
          FROM SPJ SPJY, J
          WHERE JX.JNO = SPJY.JNO
            AND SPJY.QTY <
                  ALL ( SELECT MAX ( SPJ.QTY)
                        FROM SPJ
                        WHERE SPJ.JNO < SPJX.JNO
                        GROUP BY SNO )
          GROUP BY QTY ) !
SELECT CITY FROM J JX
  WHERE JX.CITY IN ( 'Paris', 'Oslo' )
    AND JX.CITY IN ( SELECT SX.CITY FROM S SX
                      WHERE SX.CITY <
                       ANY ( SELECT P.CITY FROM P
                             WHERE P.WEIGHT + SX.STATUS < 50 ) ) !
commit work release !
file cleark !
file CONNECT ( kern test !
create table t ( k1 char (8) key,
                 k2 char (8) key,
                 f1 fixed (7),
                 r1 fixed (9) range in (1,2,3,4,5)) !
insert t values ('k1', 'k2', 2, 2)!
insert t values ('k1', 'k3', 1, 2)!
insert t values ('k1', 'k5', 2, 3)!
create table t1 ( tk1 fixed (1) key,
                  tk2 char (6))!
insert t1 set tk1 = 1, tk2 = 'k2'!
insert t1 set tk1 = 2, tk2 = 'k5'!
create view vohne as select * from t where f1 < 3 !
create view vcheck as select * from t where f1 < 3 with check option !
create view vjoin as select * from t, t1 !
create view vcomplex (vc1) as select count (*)+3 from t1 !
create view vcomplex1 (vc1) as
       select count (*)+3 from t having min (f1) > 0 !
create table l ( lk1 fixed (9) key,
                 lrest char (8))!
insert l values (1, 'a') !
insert l values (2, 'a') !
insert l values (3, 'a') !
commit !
select * from t !
fetch!
select * from t1!
fetch!
select * from t, t1 where k2 = tk2 !
fetch !
* update Feld mit Range durch Expression
update t set r1 = r1-1
   where t.k2 = ( select tk2 from t1 where t.k2 = tk2)!
select * from t !
fetch !
rollback !
update vcheck set f1 = 5
      where k2 = ( select tk2 from t1 where vcheck.k2 = tk2)!
select * from t !
fetch !
rollback release !
