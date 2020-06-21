*   *ID* VIEWCHCK DBCMD    changed on 1992-09-28-13.28.56 by ADMIN     *
*CS*1992-04-15
file xinitdbk !
file connect ( kern test !
* - - - - - - - - - - - - - - - - - -
create table tab ( s fixed(5) ) !
commit work !
create view v1 as select s from tab where s < 1000 !
create view v2 as select s from v1  where s <  100 !
create view v3 as select s from v2  where s >   10 !
 * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *          0       10      100     1000
 *      ----+-------+-------+-------+----
 *        ==========================<      V1
 *        ==================< - - - <      V2
 *                  >=======< - - - <      V3
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * !
file viewchk1 !
create view v1 as select s from tab where s < 1000 !
create view v2 as select s from v1  where s <  100 !
create view v3 as select s from v2  where s >   10
 with check option !
file viewchk1 !
create view v1 as select s from tab where s < 1000 !
create view v2 as select s from v1  where s <  100
 with check option !
create view v3 as select s from v2  where s >   10 !
file viewchk1 !
create view v1 as select s from tab where s < 1000 !
create view v2 as select s from v1  where s <  100
 with check option !
create view v3 as select s from v2  where s >   10
 with check option !
file viewchk1 !
create view v1 as select s from tab where s < 1000
 with check option !
create view v2 as select s from v1  where s <  100 !
create view v3 as select s from v2  where s >   10 !
file viewchk1 !
create view v1 as select s from tab where s < 1000
 with check option !
create view v2 as select s from v1  where s <  100 !
create view v3 as select s from v2  where s >   10
 with check option !
file viewchk1 !
create view v1 as select s from tab where s < 1000
 with check option !
create view v2 as select s from v1  where s <  100
 with check option !
create view v3 as select s from v2  where s >   10 !
file viewchk1 !
create view v1 as select s from tab where s < 1000
 with check option !
create view v2 as select s from v1  where s <  100
 with check option !
create view v3 as select s from v2  where s >   10
 with check option !
file viewchk1 !
* - - - - - - - - - - - - - - - - - -

* Durch den Wegfall von Part1 im Messagebuffer kam es zu Problemen, die
* Literale zur Qualifikation einer aenderbaren View an die korrekte
* Stelle im Datenpart zu transportieren. Ganz nebenbei wird hier noch
* getestet, wie sich der Kern bei einem fehlenden Datenpart in der
* Auftragsschnittstelle bei LONG-Befehlen verhaelt!
CREATE TABLE SYSDIALOGLIBRARY (
   OWNER       CHAR(64) ASCII DEFAULT USERGROUP,
   APPLICATION CHAR(18) ASCII DEFAULT ' ',
   MODULE      CHAR(18) ASCII DEFAULT ' ',
   PROGLANG    CHAR(18) ASCII DEFAULT ' ',
   OBJTYPE     CHAR(18) ASCII,
   SUBTYPE     CHAR(12) ASCII DEFAULT ' ',
   VERSION     CHAR(4)  ASCII DEFAULT ' ',
   LONGNAME    CHAR(40) ASCII DEFAULT ' ',
   KIND        CHAR(6)  ASCII,
   MODINDEX    CHAR(2)  ASCII,
   STATUS      CHAR(1)  ASCII,
   CREATEDATE  DATE            DEFAULT DATE,
   CREATETIME  TIME            DEFAULT TIME,
   UPDATEDATE  DATE            DEFAULT DATE,
   UPDATETIME  TIME            DEFAULT TIME,
   STRLENGTH   FIXED(18,0),
   CODETYPE    CHAR(6)  ASCII,
   VALUE_AREA  FIXED(18,0),
   CODE_AREA   FIXED(18,0),
   OBJ         LONG     BYTE,
   PRIMARY KEY (OWNER,APPLICATION,MODULE,PROGLANG,OBJTYPE,SUBTYPE,VERSION)
)!
CREATE VIEW SYSDIALLIB
AS SELECT *
   FROM SYSDIALOGLIBRARY
   WHERE owner = USERGROUP
WITH CHECK OPTION!

PARSING!
INSERT INTO sysdiallib SET owner = :username, application = :modn,
  module = ' ', proglang = 'BASIC', objtype = 'MODULE', subtype = :subtype,
  version = '->DB'!
EXECUTING!
DATA!
INSERT INTO!
1
c 18 'KERN'
c 18 'TEST'
c 12 'INFO'

INSERT INTO!
1
c 18 'KERN'
c 18 'TEST'
c 12 'INFO'

NODATA!
PARS_EXECUTE!
ROLLBACK WORK RELEASE!
* === check updateable join views 
file connect ( kern test !

create table test1
( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2))!
create index i1 on test1(k2,k1)!
create table test2
( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2),
foreign key(k1, k2) references test1(k1,k2))!
create index i2 on test2(k2,k1)!


create view testv( k1, k2, c3, c4, c5 ) as 
select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 
from test1 t1, test2 t2 
where 
t1.c4 = t2.c4 and t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 
with check option !

insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,2)!
insert into testv(k1,k2,c3,c4) values (1,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)!
insert into testv(k1,k2,c3,c4) values (1,3,1,2)!
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)!
update testv set k1=3 where k1=1!
update testv set c5=45 where k1=1!
update testv set c5=2 where k2=2!
update testv set c4=2, c5=2 where k2=2!

delete from test2!
delete from test1!

*pars_then_ex!
create or replace view testv( k1, k2, c3, c4, c5 ) as 
select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 
from test1 t1, test2 t2 
where 
t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 and t1.c4 = t2.c4
with check option !

pars_then_ex!
insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,2)!
insert into testv(k1,k2,c3,c4) values (1,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)!
insert into testv(k1,k2,c3,c4) values (1,3,1,2)!
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)!
update testv set k1=3 where k1=1!
update testv set c5=45 where k1=1!
update testv set c5=2 where k2=2!
update testv set c4=2, c5=2 where k2=2!
pars_execute!
* both tables get new roots
delete from test2!
delete from test1!
* execution of join permanent mesblocks must contain valid roots
insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
rollback work release!

* === check updateable join views 
file connect ( kern test !

create table test1
( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2))!
create index i1 on test1(k2,k1)!
create table test2
( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2),
foreign key(k1, k2) references test1(k1,k2))!
create index i2 on test2(k2,k1)!


create view testv( k1, k2, c3, c4, c5 ) as 
select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 
from test1 t1, test2 t2 
where 
t1.c4 = t2.c4 and t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 
with check option !

insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,2)!
insert into testv(k1,k2,c3,c4) values (1,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)!
insert into testv(k1,k2,c3,c4) values (1,3,1,2)!
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)!
update testv set k1=3 where k1=1!
update testv set c5=45 where k1=1!
update testv set c5=2 where k2=2!
update testv set c4=2, c5=2 where k2=2!

delete from test2!
delete from test1!

*pars_then_ex!
create or replace view testv( k1, k2, c3, c4, c5 ) as 
select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 
from test1 t1, test2 t2 
where 
t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 and t1.c4 = t2.c4
with check option !

pars_then_ex!
insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,2)!
insert into testv(k1,k2,c3,c4) values (1,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)!
insert into testv(k1,k2,c3,c4) values (1,3,1,2)!
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)!
update testv set k1=3 where k1=1!
update testv set c5=45 where k1=1!
update testv set c5=2 where k2=2!
update testv set c4=2, c5=2 where k2=2!
pars_execute!
* both tables get new roots
delete from test2!
delete from test1!
* execution of join permanent mesblocks must contain valid roots
insert into testv(k1,k2,c3,c4) values (3,2,1,1)!

pars_execute!
ROLLBACK WORK RELEASE!

file shutdwnk !
vdnexit !
