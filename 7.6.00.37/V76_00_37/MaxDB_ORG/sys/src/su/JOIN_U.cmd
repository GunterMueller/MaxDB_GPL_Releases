senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* UNION.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table tab1 ( t1c1 char(5) ,
                    t1c2 char(5) ascii,
                    t1c3 fixed(6,2),
                    t1c4 float,
                    t1c5 char(5) byte )!
create table tab2 ( t2c1 char(10) ,
                    t2c2 char(10) ascii,
                    t2c3 fixed(8,2),
                    t2c4 float,
                    t2c5 char(10) byte )!
create table tab3 ( t3c1 char(25) ,
                    t3c2 char(25) ascii,
                    t3c3 fixed(10,2),
                    t3c4 float,
                    t3c5 char(25) byte )!
commit work !
insert tab1 set t1c1 = '111ab',
                t1c2 = '121ab',
                t1c3 = 113.01,
                t1c4 = 114.01,
                t1c5 = X'f1f5f18182' !
insert tab1 set t1c1 = '112ab',
                t1c2 = '122ab',
                t1c3 = 213.02,
                t1c4 = 214.02,
                t1c5 = X'f1f5f28182' !
insert tab1 set t1c1 = '113ab',
                t1c2 = '123ab',
                t1c3 = 313.03,
                t1c4 = 314.03,
                t1c5 = X'f1f5f38182' !
insert tab1 set t1c1 = '114ab',
                t1c2 = '124ab',
                t1c3 = 413.04,
                t1c4 = 414.04,
                t1c5 = X'f1f5f48182' !
insert tab2 set t2c1 = '211abcde',
                t2c2 = '221abcde',
                t2c3 = 20123.01,
                t2c4 = 20124.01,
                t2c5 = X'f2f5f18182' !
insert tab2 set t2c1 = '212abcde',
                t2c2 = '222abcde',
                t2c3 = 20223.02,
                t2c4 = 20224.02,
                t2c5 = X'f2f5f28182' !
insert tab2 set t2c1 = '213abcde',
                t2c2 = '223abcde',
                t2c3 = 20323.03,
                t2c4 = 20324.03,
                t2c5 = X'f2f5f38182' !
insert tab2 set t2c1 = '214abcde',
                t2c2 = '224abcde',
                t2c3 = 20423.04,
                t2c4 = 20424.04,
                t2c5 = X'f2f5f48182' !
insert tab3 set t3c1 = '311abcdefghij',
                t3c2 = '321abcdefghij',
                t3c3 = 3020123.01,
                t3c4 = 3020124.01,
                t3c5 = X'f3f5f18182' !
insert tab3 set t3c1 = '312abcdefghij',
                t3c2 = '322abcdefghij',
                t3c3 = 3020223.02,
                t3c4 = 3020224.02,
                t3c5 = X'f3f5f28182' !
insert tab3 set t3c1 = '313abcdefghij',
                t3c2 = '323abcdefghij',
                t3c3 = 3020323.03,
                t3c4 = 3020324.03,
                t3c5 = X'f3f5f38182' !
insert tab3 set t3c1 = '314abcdefghij',
                t3c2 = '324abcdefghij',
                t3c3 = 3020433.04,
                t3c4 = 3020434.04,
                t3c5 = X'f3f5f48182' !
commit release !

file connect ( kern test !
select distinct t1c1 from tab1, tab2 group by t1c1!
fetch !
select t1c1 from tab1, tab2 group by t1c1
   union select t1c1 from tab2, tab1 group by t1c1!
fetch !
select t3c1 from tab2, tab3 group by t3c1 !
fetch !
select t1c1 from tab1, tab2 group by t1c1
   union all select t3c1 from tab2, tab3 group by t3c1!
fetch !
select t1c1 from tab1, tab2 
   union all select distinct t3c1 from tab2, tab3 !
fetch !
select distinct t1c1 from tab1, tab2 
   union all select distinct t3c1 from tab2, tab3 !
fetch !
select distinct t1c1 from tab1, tab2 
   union all select t3c1 from tab2, tab3 !
fetch !
select count(*) from tab1, tab2 group by t1c2 !
fetch !
select count(*) from tab3 group by t3c2 !
fetch !
select count(*) from tab1, tab2 group by t1c2
union all select count(*) from tab3 group by t3c2 !
fetch !
select count(*) from tab1, tab2 group by t1c2
union select count(*) from tab3 group by t3c2 !
fetch !
commit release !

file connect ( kern test !
create table tab5 ( spalte1 fixed(5),
                    spalte2 fixed(11,6),
                    spalte3 fixed(3,3),
                    spalte4 float(7) ) !
create table tab6 ( spalte1 float(12), col2 fixed(5,2) ) !
insert tab5 values ( 348  , 76543.21, - 0.623, -12.3E12 ) !
insert tab5 values ( -9999,        0,   0.003, 12345e-7 ) !
insert tab5 values (  null,  -5.1234,       0,     null ) !
insert tab5 values (     0,     null,    null,    3E-32 ) !
insert tab6 values ( 1.34E+1,   22.33 ) !
insert tab6 values (   -0.18, -123.07 ) !
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 ))
union
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 ))
union all
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null
union
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ),
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null
union all
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ),
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null !
fetch !
drop table tab5 !
drop table tab6 !
commit release !

file connect ( kern test !
create table t1 ( t1xc1 char(10) ascii key,
                  t1xc2 char(20) ,
                  t1xc3 fixed(5,2) ) !
create table t2 ( t2xc1 fixed(7,2) key,
                  t2xc2 char(10)  key,
                  t2xc3 char(20) ascii ) !
create table t3 ( t3xc1 char(15) ascii,
                  t3xc2 char(20) ,
                  t3xc3 fixed(6,3) ) !
insert t1 values ( 'Hans', 'Bremen', 1 ) !
insert t1 values ( 'Karl', 'Bremen', 100 ) !
insert t1 values ( 'Karin', 'Hamburg', null ) !
insert t1 values ( 'Lisa', null, null ) !
insert t2 values ( 20, 'Paul', 'Muenchen' ) !
insert t2 values ( 100, 'Emil', 'Hannover' ) !
insert t2 values ( 100, 'Emma', null ) !
insert t3 values ( 'Karl', 'Hannover', 1 ) !
insert t3 values ( 'Otto', 'Dortmund', 300 ) !
insert t3 values ( null, null, null ) !
insert t3 values ( 'Ilse', 'Hamm', 200 ) !
commit work !
select * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' ) !
fetch !
select * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' )
union all
select * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' ) !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* UNIONTST.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( a fixed(5) )!
insert t1 set a = 1 !
insert t1 set a = 2 !
insert t1 set a = 3 !
insert t1 set a = 4 !
insert t1 set a = 5 !
insert t1 set a = 2 !
insert t1 set a = 3 !
insert t1 set a = 4 !
create table t2 ( b fixed(5) )!
insert t2 set b = 3 !
insert t2 set b = 4 !
insert t2 set b = 5 !
insert t2 set b = 6 !
insert t2 set b = 7 !
insert t2 set b = 4 !
insert t2 set b = 5 !
insert t2 set b = 6 !
commit work release !
file connect ( kern test !
*
SELECT * FROM (SELECT a, a a1 FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1 WHERE ref.A (+)= B))
UNION
SELECT b, b FROM T2 where b <> b + 10
 !
FETCH !
SELECT B FROM T2,t1 WHERE A (+)= B !
FETCH !
SELECT * FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1) !
FETCH !
SELECT * FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1 WHERE ref.A = B) !
FETCH !
SELECT * FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1 WHERE ref.A (+)= B) !
FETCH !
declare erg1 cursor for 
select * FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1 WHERE ref.A (+)= B) !
FETCH erg1 !
declare erg cursor for 
select * FROM (SELECT a, a a1 FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1 WHERE ref.A (+)= B)) !
SELECT * from erg
UNION
SELECT b, b FROM T2 where b <> b + 10
 !
FETCH !
SELECT b, b FROM T2 where b <> b + 10
UNION
SELECT * FROM (SELECT a, a a1 FROM T1 ref WHERE A IN
       (SELECT B FROM T2,t1 WHERE ref.A (+)= B))
 !
FETCH !
*
drop table t1 !
commit work release !
file connect ( kern test !
CREATE TABLE T1 (A1 CHAR(5), A2 CHAR(5)) !
INSERT T1 VALUES ('aaaaa','bbbbb') !
SELECT t2.a2 hugo FROM T1, t1 t2 
where t1.a1 = t2.a1
and t1.a2 > 'b' 
union all
SELECT t2.a2 hugo FROM T1, t1 t2 
where t1.a1 = t2.a1
and t1.a2 > 'b' !
fetch !
drop table t1 !
commit work !
CREATE TABLE T1 (A1 CHAR(5) key, A2 CHAR(5)) !
INSERT T1 VALUES ('aaaaa','bbbbb') !
SELECT t2.a2 hugo FROM T1, t1 t2 
where t1.a1 = t2.a1
and t1.a2 > 'b' 
union all
SELECT t2.a2 hugo FROM T1, t1 t2 
where t1.a1 = t2.a1
and t1.a2 > 'b' !
fetch !
drop table t1 !
commit work release !

FILE connect (kern test!
CREATE TABLE leer (a CHAR)!
SELECT a FROM leer UNION ALL SELECT a FROM <dual, leer>!
FETCH!
SELECT * FROM DUAL!
CREATE VIEW V AS SELECT A FROM LEER UNION ALL SELECT A FROM <DUAL, LEER>!
SELECT * FROM V !
FETCH!
SELECT * FROM DUAL!
ROLLBACK WORK !

CREATE TABLE TA ( a fixed(6)) !
INSERT TA SET a = 1 !
SELECT ta.* FROM <TA, ta t1> where 1 = 0
union all
SELECT ta.* FROM TA , ta t1!
FETCH !
SELECT ta.* FROM <TA t1, ta> where 1 = 0
union all
SELECT ta.* FROM TA , ta t1!
FETCH !
SELECT ta.* FROM <TA, ta t1> where ta.a = 1 and t1.a = 0
union all
SELECT ta.* FROM TA , ta t1!
FETCH !
SELECT ta.* FROM <TA t1, ta> where  ta.a = 1 and t1.a = 0
union all
SELECT ta.* FROM TA , ta t1!
FETCH !
ROLLBACK WORK RELEASE!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* UPDJV.vdnts
*   *ID* UPDJV    VDNTEST  changed on 1992-09-28-13.28.53 by ADMIN     *
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table worker1 (w_no fixed(5) key, w_vorname char(20),
             w_name char(20) not null, w_wohnort fixed(4)) !
create table worker2 (w_no fixed(5) key, geb date,
             abteilung fixed(4)) !
create table stadt (plz fixed(4) key, town_name char(20))!
create table abteilung (abt_no fixed(4) key, abt_name char(20),
                        abt_ort fixed(4)) !
commit !
alter table worker1 foreign key worker (w_no)
                        references worker2 !
alter table worker1 foreign key worker_wohnort (w_wohnort)
                        references stadt !
alter table worker2 foreign key worker_abt (abteilung)
                        references abteilung !
alter table abteilung foreign key abt_stadt (abt_ort)
                        references stadt !
commit !
create view worker (w_no, vorname, nachname, plz, wohnort,
                         geb, abt_no, abt_name, abt_plz, abt_stadt)
            as select
               worker1.w_no, w_vorname, w_name, w_wohnort,
               w_stadt.town_name, geb, abteilung,
               abt_name, abt_ort, abt_stadt.town_name
            from stadt abt_stadt, abteilung, stadt w_stadt,
                 worker1, worker2
            where abt_ort           = abt_stadt.plz     and
                  worker1.w_no = worker2.w_no and
                  w_wohnort         = w_stadt.plz       and
                  abteilung         = abteilung.abt_no
            with check option !
file sel_cols_tab ( WORKER !
insert worker values (1, 'Fritz', 'Meier', 1000, 'Berlin',
           '19500304', 100, 'Buchhaltung', 1000, 'Berlin') !
select * from worker1 !
fetch !
select * from worker2 !
fetch !
select * from abteilung !
fetch !
select * from stadt !
fetch !
select * from worker !
fetch !
insert worker values (2, 'Anton', 'Schmidt', 1000, 'Berlin',
           '19601105', 100, 'Buchhaltung', 1000, 'Berlin') !
insert worker values (3, 'Harry', 'Hirsch', 8000, 'Muenchen',
           '19370825', 200, 'Entwicklung', 8000, 'Muenchen') !
insert worker values (4, 'Stefan', 'May', 8000, 'Muenchen',
           '19370825', 200, 'Entwicklung', 8000, 'Stuttgart') !
select * from worker !
fetch !
insert worker values (2, 'Anton', 'Schmidt', 1000, 'Berlin',
           '19601105', 100, 'Buchhaltung', 1000, 'Berlin') !
insert worker set w_no    = 4,    vorname   = 'Willi',
                       plz     = 5000, wohnort   = 'Koeln',
                       abt_no  = 311,  abt_name  = 'Personalwesen',
                       abt_plz = 8000, abt_stadt = 'Muenchen' !
insert worker set w_no     = 4,    vorname   = 'Willi',
                       nachname = 'Wacker',
                       plz      = 5000, wohnort   = 'Koeln',
                       abt_no   = 311,  abt_name  = 'Personalwesen',
                       abt_plz  = 8000, abt_stadt = 'Muenchen' !
insert worker set w_no     = 5,    vorname   = 'Knut',
                       nachname = 'Wuchtig',
                       wohnort   = 'Koeln',
                       abt_no   = 311,  abt_name  = 'Personalwesen',
                       abt_plz  = 8000, abt_stadt = 'Muenchen' !
explain select * from worker !
select * from worker !
fetch !
update worker set geb = '19400522' key w_no = 4 !
update worker set plz = 1000 key w_no = 4 !
update worker set plz = 1000, wohnort = 'Berlin' key w_no = 4 !
select * from worker !
fetch !
update worker set geb = '19400522' key w_no = 4 , nachname = 'A' !
update worker set geb = '19400522' key nachname = 'A' !
update worker set abt_no = 100, abt_name = 'Buchhaltung'
                   key w_no = 4 !
update worker set abt_no  = 100,  abt_name  = 'Buchhaltung',
                       abt_plz = 1000, abt_stadt = 'Berlin'
                   key w_no = 4 !
commit !
declare res cursor for 
select * from worker for update !
fetch first res !
update worker set geb = '19450404' where current of res !
select * from worker !
fetch !
fetch pos (2) res into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j !
update worker set abt_plz = 8000, abt_stadt = 'Muenchen'
      where current of res !
update worker set plz = 8000, wohnort = 'Muenchen'
      where current of res !
select * from worker !
fetch !
commit !
delete worker where current of res !
select * from worker !
fetch !
delete worker key w_no = 1 !
select * from worker !
fetch !
select * from worker1 !
fetch !
select * from worker2 !
fetch !
commit !
lock row worker key w_no = 3 in exclusive mode !
select process, username, lock_timeout, req_timeout,
lock_mode, lock_state, req_mode, req_state, 
owner, tablename, tableid, rowid_length, rowid_hex, rowid
from sysdd.locks where tablename <> 'DUAL' !
fetch !
declare res cursor for 
select * from worker for update !
fetch first res !
commit !
lock row worker current of res in exclusive mode !
select process, username, lock_timeout, req_timeout,
lock_mode, lock_state, req_mode, req_state, 
owner, tablename, tableid, rowid_length, rowid_hex, rowid
from sysdd.locks where tablename <> 'DUAL' !
fetch !
drop table worker1 !
drop table worker2 !
drop table stadt !
drop table abteilung !
commit release !
 * PTS 1588 !
file connect ( kern test !
CREATE TABLE "M4SYS_MOD_USUARIO " (
"DATO"    CHAR      (1) ASCII    ,
"USUARIO" VARCHAR   (8) ASCII    ,
"COMENT"  VARCHAR   (254) ASCII    ,
"FEC_ULT_ACTUALIZAC" TIMESTAMP DEFAULT   TIMESTAMP,
"ID_USUARIO" VARCHAR   (64) ASCII    DEFAULT USER , 
PRIMARY KEY ("DATO")) !

CREATE TABLE "M4T_DEPARTAMENTOS " (
"ID_SOCIEDAD" VARCHAR   (2) ASCII    ,
"ID_DEPARTAMENTO" VARCHAR   (8) ASCII    ,
"N_DEPARTAMENTO" VARCHAR   (40) ASCII    NOT NULL,
"FEC_ULT_ACTUALIZAC" TIMESTAMP DEFAULT TIMESTAMP,
"COMENT" VARCHAR   (254) ASCII    ,
"ID_USUARIO" VARCHAR   (64) ASCII    DEFAULT USER,
"ID_RESPONSABLE" VARCHAR   (10) ASCII    NOT NULL
, PRIMARY KEY ("ID_SOCIEDAD" ,"ID_DEPARTAMENTO")) !

CREATE TABLE "M4T_HIST_DEPARTAM " (
"ID_SOCIEDAD" VARCHAR (2) ASCII  ,
"ID_EMPLEADO" VARCHAR (10) ASCII  ,
"FEC_ALTA_EMPLEADO" DATE  ,
"FEC_INICIO" DATE  ,
"FEC_FIN" DATE  ,
"COMENT" VARCHAR (254) ASCII  ,
"FEC_ULT_ACTUALIZAC" TIMESTAMP DEFAULT TIMESTAMP,
"ID_USUARIO" VARCHAR (64) ASCII  DEFAULT USER,
"ID_DEPARTAMENTO" VARCHAR (8) ASCII  NOT NULL,
"ID_MOTIVO_CAMBIO" VARCHAR (2) ASCII  DEFAULT     '0' , 
PRIMARY KEY ("ID_SOCIEDAD" ,"ID_EMPLEADO" ,"FEC_ALTA_EMPLEADO"
         ,"FEC_INICIO"),
FOREIGN KEY "FK_DEPTOS_HDEPTOS " ("ID_SOCIEDAD   " ,"ID_DEPARTAMENTO "
) REFERENCES "M4T_DEPARTAMENTOS "
("ID_SOCIEDAD   " ,"ID_DEPARTAMENTO ") ON DELETE RESTRICT ) !

 CREATE VIEW M4_DEPARTAMENTOS AS SELECT * FROM M4T_DEPARTAMENTOS 
WITH CHECK OPTION !

 CREATE VIEW M4_HIST_DEPARTAM AS SELECT * FROM M4T_HIST_DEPARTAM 
WITH CHECK OPTION !

 CREATE VIEW M4VH_1 AS SELECT H1.*, D.N_DEPARTAMENTO FROM 
M4_HIST_DEPARTAM H1, M4_DEPARTAMENTOS D WHERE H1.FEC_FIN IS NULL AND 
H1.ID_DEPARTAMENTO = D.ID_DEPARTAMENTO AND H1.ID_SOCIEDAD = D.ID_SOCIEDAD
with check option !

 CREATE VIEW M4VH_2 AS SELECT H1.*, D.N_DEPARTAMENTO FROM 
M4_HIST_DEPARTAM H1, M4_DEPARTAMENTOS D WHERE H1.FEC_FIN IS NULL AND 
H1.ID_DEPARTAMENTO = D.ID_DEPARTAMENTO AND H1.ID_SOCIEDAD = D.ID_SOCIEDAD!
file sel_table !
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* UPDJV2.vdnts
*   *ID* UPDJV2   VDNTEST  changed on 1992-11-11-17.02.15 by KERN      *
*   *ID* QUALUPD1 DBCMD    changed on 1992-09-28-13.27.50 by ADMIN     *
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 (a1 int key, a2 int) !
create table t2 (b1 int key, b2 int not null) !
alter table t1 foreign key l (a2) references t2 !
insert t2 values (20, 30) !
insert t2 values (50, 500) !
insert t1 values (10, 20) !
insert t1 values (20, 20) !
create view v1 (a,b,c) as select a1, a2, b2 from t1, t2 where
     a2 = b1 with check option !
commit !
select * from v1 !
fetch !
commit !
update v1 set b = b + c, c = 300 + 200 key a = 10 !
select * from v1 !
fetch !
rollback !
select * from v1 !
fetch !
update v1 set b = b + c, c = 300 + 200 where a >= 10 !
select * from v1 !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD2 DBCMD    changed on 1992-09-28-13.27.50 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int key, a2 int, a3 int) !
create table t2 (b1 int key, b2 int key, b3 int) !
alter table t1 foreign key l (a2, a3) references t2 !
create view v1 as select a1, a2, a3, b3 from t1, t2 where a2 = b1 and
               a3 = b2 with check option !
commit !
insert v1 values (1,2,3,4) !
insert v1 values (2,3,3,5) !
commit !
update v1 set a2 = 3, b3 = 5 * a1 key a1 = 1 !
select * from v1 !
fetch !
update v1 set a2 = 3, a3 = 4, b3 = 5 key a1 = 1 !
select * from v1 !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD3 DBCMD    changed on 1992-09-28-13.27.51 by ADMIN     *
file connect ( kern test !
create table t1 (s1 char(5) key, s2 char(5), s3 char(5),s4 char(5),
s5 char(5)) !
create table t2 (s1 char(5) key, s2 char(5) key, s3 char(5) key,
                 s5 char(5)) !
alter table t1 foreign key l (s2,s3,s4) references t2 !
create view v (a,b,c,d,e,f) as select
     t1.s1, t1.s2, t1.s3, t1.s4, t1.s5, t2.s5
     from t1, t2
     where t1.s4 = t2.s3 and
           t1.s3 = t2.s2 and
           t1.s2 = t2.s1 and
           t1.s5 > t2.s5 with check option !
insert v values ('aaaaa', 'bbbbb', 'ccccc', 'd', 'fffff','eeeee') !
select * from v !
fetch !
update v set e = 'ggggg' key a = 'aaaaa' !
select * from v !
fetch !
rollback work release !
*   *ID* QUALUPD4 DBCMD    changed on 1992-09-28-13.27.51 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int key, a2 int) !
create table t2 (b1 int key, b2 int not null) !
alter table t1 foreign key l (a2) references t2 !
insert t2 values (20, 30) !
insert t2 values (50, 500) !
insert t1 values (10, 20) !
insert t1 values (20, 50) !
commit !
select a1, a2, b2 from t1, t2 where a2 = b1 !
create view v1 (a,b,c) as select a1, a2, b2 from t1, t2 where
     a2 = b1 with check option !
select * from v1 !
fetch !
commit !
pars_then_ex !
data !
update v1 set b = b + :c, c = 300 + 200 key a = 10 !
n 38 10

pars_execute !
nodata !
select * from v1 !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD5 DBCMD    changed on 1992-09-28-13.27.51 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int key, a2 char(5)) !
create table t2 (b1 char(5) key, b2 char(5)) !
alter table t1 foreign key l (a2) references t2 !
create view v as select a1, a2, b2 from t1, t2 where a2 = b1
            with check option !
commit !
insert v values (10, 'bbbbb', 'ccccc') !
insert v values (20, 'eeeee', 'fffff') !
commit !
delete v where a1 = 10 !
select * from v !
fetch !
rollback !
select * from v !
fetch !
pars_then_ex !
data !
update v set a2 = :a, b2 = :b where a1 = :c !
c 5 'xxxxx'
c 5 'yyyyy'
n 10 10

pars_execute !
nodata !
select * from v !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD6 DBCMD    changed on 1992-09-28-13.27.52 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int key, a2 int key, a3 int key, a4 int, a5 int) !
create table t2 (b1 int key, b2 int, b3 int) !
commit !
alter table t1 foreign key l (a4) references t2 !
commit !
create view v as select a1, a2, a3, a4, a5, b2, b3 from t1, t2
                 where a4 = b1 with check option !
commit !
pars_then_ex !
data !
insert v values (1,1,1,10,1000,20,30) !

insert v values (:a,:b,:c,:e,:f,:g,:h) !
n 10 2
n 10 2
n 10 2
n 10 20
n 10 50
n 10 1100
n 10 40

insert v values (:a,:b,:c,:e,:f,:g,:h) !
n 10 2
n 10 2
n 10 2
n 10 20
n 10 90
n 10 1100
n 10 80

insert v values (:a,:b,:c,:e,:f,:g,:h) !
n 10 3
n 10 3
n 10 3
n 10 20
n 10 60
n 10 1100
n 10 50

insert v values (3,:b,3,:e,:f,:g,:h) !
n 10 3
n 10 20
n 10 50
n 10 1100
n 10 40

nodata !
pars_execute !
select * from v !
fetch !
pars_then_ex !
data !
update v set a4 = :d key a1 = :a, a2 = :b, a3 = :c !

update v set a4 = :d , b2 = 30, b3 = 40 key a1 = :a, a2 = :b, a3 = :c !
n 10 20
n 10 1
n 10 1
n 10 1

update v set a4 = :d , b2 = 1100, b3 = 40 key a1 = :a, a2 = :b, a3 = :c !
n 10 20
n 10 1
n 10 1
n 10 1

pars_execute !
nodata !
select * from v !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD7 DBCMD    changed on 1992-09-28-13.27.52 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int key, a2 char(5)) !
create table t2 (b1 char(5) key, b2 char(5)) !
alter table t1 foreign key l (a2) references t2 !
create view v as select a1, a2, b2 from t1, t2 where a2 = b1
            with check option !
commit !
insert v values (10, 'bbbbb', 'ccccc') !
insert v values (20, 'eeeee', 'fffff') !
commit !
delete v where a1 = 10 !
select * from v !
fetch !
rollback !
select * from v !
fetch !
update v set a2 = 'xxxxx', b2 = 'yyyyy' where a1 = 10 !
select * from v !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD8 DBCMD    changed on 1992-09-28-13.27.53 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int key, a2 int) !
create table t2 (b1 int key, b2 int not null, b3 int) !
alter table t1 foreign key l (a2) references t2 !
create view v as select a1, a2, b3 from t1, t2 where
                    a2 = b1 with check option !
commit !
file sel_priv_on_tab ( V !
insert t2 values (2,3,4) !
insert t1 values (1,2) !
select * from v !
fetch !
update v set a2 = 3, b3 = 4 key a1 = 1 !
insert t2 values (3,100,4) !
update v set a2 = 3, b3 = 4 key a1 = 1 !
select * from v !
fetch !
drop table t1 !
drop table t2 !
commit work release !
*   *ID* QUALUPD9 DBCMD    changed on 1992-09-28-13.27.53 by ADMIN     *
file connect ( kern test !
create table t1 (a1 char(5) ascii key, a2 char(5) ascii key,
      a3 char(5) ascii key, a4 char(5) ascii ) !
insert t1 values ('aaaaa','bbbbb','ccccc', 'ddddd') !
create table t2 (b1 char(5) ascii key, b2 char(5) ascii key,
      b3 char(5) ascii key, b4 char(5)) !
insert t2 values ('aaaaa','bbbbb','ccccc', 'ddddd') !
create table t3 (c1 char(5) ascii key, c2 char(5) ascii key,
      c3 char(5) ascii key, c4 char(5) ascii) !
insert t3 values ('aaaaa','bbbbb','ccccc', 'ddddd') !
create table t4 (d1 char(5) ascii key, d2 char(5) ascii key,
      d3 char(5) ascii key, d4 char(5)) !
insert t4 values ('bbbbb','ccccc', 'ddddd', 'eeeee') !
select a4 from t1, t2, t3, t4
     where a1 = b1 and a2 = b2 and a3 = b3 and
           b1 = c1 and b2 = c2 and b3 = c3 and
           c2 = d1 and c3 = d2 and c4 = d3 !
fetch !
select b1, a2, b3, a4, b4, c4, d4 from t1, t2, t3, t4
     where a1 = b1 and a2 = b2 and a3 = b3 and
           b1 = c1 and b2 = c2 and b3 = c3 and
           c2 = d1 and c3 = d2 and c4 = d3 !
fetch !
alter table t1 foreign key l  (a1,a2,a3) references t2 !
alter table t2 foreign key l1 (b1,b2,b3) references t3 !
alter table t3 foreign key l2 (c2,c3,c4) references t4 !
commit !
create view v as select a1, a2, a3, a4, b4, c4, d4 from t1, t2, t3, t4
     where a1 = b1 and a2 = b2 and a3 = b3 and
           b1 = c1 and b2 = c2 and b3 = c3 and
           c2 = d1 and c3 = d2 and c4 = d3 with check option !
commit !
file sel_priv_on_tab ( V !
file sel_cols_tab ( V !
commit !
lock row v key a1 = 'aaaaa', a2 = 'bbbbb', a3 = 'cc' in exclusive mode !
select process, username, lock_timeout, req_timeout,
lock_mode, lock_state, req_mode, req_state, 
owner, tablename, tableid, rowid_length, rowid_hex, rowid
from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
pars_then_ex !
data !
lock row v key a1 = :a, a2 = :b, a3 = :c in exclusive mode !
c 5 'aaaaa'
c 5 'bbbbb'
c 5 'cc'

pars_execute !
nodata !
select process, username, lock_timeout, req_timeout,
lock_mode, lock_state, req_mode, req_state, 
owner, tablename, tableid, rowid_length, rowid_hex, rowid
from sysdd.locks where tablename <> 'DUAL' !
fetch !
create view v1 as select b1, a2, b3, a4, b4, c4, d4 from t1, t2, t3, t4
     where a1 = b1 and a2 = b2 and a3 = b3 and
           b1 = c1 and b2 = c2 and b3 = c3 and
           c2 = d1 and c3 = d2 and c4 = d3 with check option !
commit !
file sel_priv_on_tab ( V1 !
file sel_cols_tab ( V1 !
select * from v1 !
fetch !
insert v1 values
    ('aaaaa', 'bbbbb', 'ccccc', 'xxxxx', 'vvvvv','ddddd', 'yyyyy') !
insert v1 values
    ('aaaab', 'bbbbb', 'ccccc', 'xxxxx', 'vvvvv','ddddd', 'yyyyy') !
insert v1 values
    ('aaaac', 'bbbbb', 'ccccc', 'xxxxx', 'vvvvv','ddddd', 'zzzzz') !
select * from v1 !
fetch !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
select * from t4 !
fetch !
commit !
lock row v1 key b1 = 'aaaaa', a2 = 'bbbbb', b3 = 'cc' in exclusive mode !
select process, username, lock_timeout, req_timeout,
lock_mode, lock_state, req_mode, req_state, 
owner, tablename, tableid, rowid_length, rowid_hex, rowid
from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
pars_then_ex !
data !
lock row v1 key b1 = :a, a2 = :b, b3 = :c in exclusive mode !
c 5 'aaaaa'
c 5 'bbbbb'
c 5 'cc'

pars_execute !
nodata !
select process, username, lock_timeout, req_timeout,
lock_mode, lock_state, req_mode, req_state, 
owner, tablename, tableid, rowid_length, rowid_hex, rowid
from sysdd.locks where tablename <> 'DUAL' !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
drop table t4 !
commit work release !
*   *ID* UPDJV1   DBCMD    changed on 1992-09-28-13.28.53 by ADMIN     *
file connect ( kern test !
create table t1 (a1 int, a2 int, a3 int, primary key (a1, a2)) !
create table t2 (b1 int key, b2 int) !
alter table t1 foreign key l1 (a2) references t2 !
create view v1 as select a1, b1, a3, b2 from t1, t2 where
      a2 = b1 with check option !
insert v1 values (1,2,3,4) !
update v1 set a3 = 3 key a1 = 1, b1 = 2 !
rollback release !
*   *ID* UPDJV2   DBCMD    changed on 1992-11-11-17.02.14 by KERN      *
file connect ( kern test !
create table t1 (a1 int, a2 int, a3 int, primary key (a1)) !
create table t2 (b1 int key, b2 int) !
alter table t1 foreign key l1 (a2) references t2 !
create view v1 as select a1, b1, a3, b2 from t1, t2 where
      a2 = b1 with check option !
create index i1 on t1 (a2) !
insert v1 values (1,2,3,4) !
update v1 set a3 = 3 key a1 = 1 !
select * from v1 !
fetch !
delete v1 where a1 = 1 !
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* from N00T0809.cmd
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( t1xc1 fixed(3) key, t1xc2 fixed(3) ) !
create table t2 ( t2xc1 fixed(3) key, t2xc2 fixed(3),
                  t2xc3 fixed(3) ) !
alter table t2 foreign key t2lt1 (t2xc2) references t1 !
create view v1 as select t1xc1, t1xc2, t2xc1, t2xc3
   from t1, t2 where t1xc1 = t2xc2 with check option !
commit work !
insert t1 values ( 5, 3 ) !
insert t1 values ( 8, 3 ) !
insert t1 values ( 4, 1 ) !
insert t1 values ( 3, 1 ) !
insert t1 values ( 2, 3 ) !
insert t2 values ( 7, 8, 3 ) !
insert t2 values ( 3, 5, 4 ) !
insert t2 values ( 5, 3, 4 ) !
insert t2 values ( 4, 2, 3 ) !
insert t2 values ( 9, 4, 2 ) !
insert t2 values ( 1, 3, 8 ) !
commit work !
select * from v1 where t1xc2 < t2xc1 order by 1,2,3,4!
fetch !
insert v1 values ( 1, 2, 6, 3 ) !
insert into v1 set t1xc1 = 6, t2xc1 = 8, t2xc3 = 2 !
select * from v1 order by 1,2,3,4!
fetch !
update v1 set t1xc2 = 3, t1xc1 = 5, t2xc3 = 4 key t2xc1 = 6 !
delete v1 key t2xc1 = 7 !
select * from v1 order by 1,2,3,4!
fetch !
commit work !
drop table t1 !
drop table t2 !
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !
*****************************************************************************
