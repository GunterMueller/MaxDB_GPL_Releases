*   *ID* CONS6    DBCMD    changed on 1992-09-28-13.23.00 by ADMIN     *
set format internal!
create table b
 (b1 int key ,
  b2 int key ,
  b3 int key ,
  b4 date default '19901111'
  constraint b4 > '1990-01-01' AND b4 < '19901231')!
create table b
 (b1 int key ,
  b2 int key ,
  b3 int key ,
  b4 date default '19901111'
  constraint b4 > '19900101' AND b4 < '19901231')!
file sel_cols_tab ( B !
file sel_checkdef ( USER B B4 !
set format iso !
file sel_checkdef ( USER B B4 !
insert b values (1, 1, 1, default)!
insert b values (1, 1, 2, default)!
insert b values (1, 1, 3, default)!
insert b values (1, 1, 4, default)!
insert b values (1, 1, 5, default)!
update b set b4 = '19900601' where b3 = 1!
update b set b4 = '1990-06-01' where b3 = 1!
update b set b4 = substr('19900701', 1) where b3 = 2!
update b set b4 = substr('1990-07-01', 1) where b3 = 2!
update b set b4 = date('19900801') where b3 = 3!
update b set b4 = date('1990-08-01') where b3 = 3!
update b set b4 = char(date('19900901'), eur) where b3 = 4!
update b set b4 = char(date('1990-09-01'), eur) where b3 = 4!
update b set b4 = char(date('19900901'), iso) where b3 = 4!
update b set b4 = char(date('1990-09-01'), iso) where b3 = 4!
update b set b4 = char(date('1989-02-28'), iso) where b3 = 5!
update b set b4 = char(date('1989-02-29'), iso) where b3 = 5!
update b set b4 = date(char(date('1991-04-30'), eur)) where b3 = 5!
update b set b4 = date(char(date('1991-04-30'), iso)) where b3 = 5!
update b set b4 = date(char(date('1991-04-31'), iso)) where b3 = 5!
update b set b4 = date(char(date('1990-04-30'), iso)) where b3 = 5!
select * from b !
fetch!
drop table b!
commit!
***********************************************************
***********************************************************
***********************************************************
set format eur !
create table a
 (bd date  default date
           constraint bd > date) !
insert a values (default)!
insert a values (date)!
create table b
 (bd date  default date
           constraint (bd >= makedate(1, 1))
           and (bd <= makedate(2010, 365)),
  bt time  default time
           constraint (bt >= maketime(11, 11, 11))
           and (bt <= maketime(99, 99, 99))) !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values ('12.0.0', '12.0.0')!
update b set bt = maketime(66, 66, 66)!
select * from b !
fetch!
set format usa !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
update b set bt = maketime(77, 77, 77)!
select * from b !
fetch!
update b set bt = '6:18 am'!
update b set bt = '1:18 pm'!
select * from b !
fetch!
update b set bt = maketime(10, 10, 9)!
select * from b !
fetch!
drop table a!
drop table b!
commit!
****************************************************
set format eur !
create table b
 (bd date  default date
           constraint (bd >= makedate(1, 1))
           and (bd <= makedate(2010, 365)),
  a  int,
  b  int,
  bt time  default time
           constraint (bt >= '11.11.11')
           and (bt <= '99.59.59')) !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values (default,0,0, '13.0.0')!
insert b values (default,1,1, '14.0.0')!
insert b values (default,2,2, '15.0.0')!
create view bb as (select a, bt from b where bt > '12.0.0') !
select * from bb!
fetch!
set format usa !
select * from bb!
fetch!
alter table b add (el char (8))!
alter table b drop (b)!
select * from bb!
fetch!
drop table b!
commit release !
file connect ( kern test !
* smallint and integer have constraint implicitly !
create table SYS_ODBC_STAT (
HANDLE     CHAR        (18),
TAB_QUAL   VARCHAR     (18),
TAB_OWNER  VARCHAR     (18),
TAB_NAME   VARCHAR     (18),
NON_UNIQ   SMALLINT ,
IND_QUAL   VARCHAR     (18),
IND_NAME   VARCHAR     (18),
TYPE       SMALLINT,
INDEXSEQ   SMALLINT,
COL_NAME   VARCHAR     (18),
COLLATION  CHAR        ( 1),
CARDINAL   INTEGER ,
PAGES      INTEGER,
primary key (handle, tab_owner, tab_name,
IND_name, COL_NAME) )
!
CREATE TABLE TAB1 (S1 INT, S2 INT, S3 INT) !
create unique index "tab1.s1"
	on tab1( s1 ) !
insert tab1 values (1, 2, 3) !
insert tab1 values (2, 2, 3) !
insert tab1 values (3, 3, 4) !
insert tab1 values (4, 3, 3) !
CREATE INDEX IND1 ON TAB1 (S2 ASC, S3 DESC) !
DELETE SYS_ODBC_STAT WHERE HANDLE = '103' !
COMMIT WORK !
INSERT SYS_ODBC_STAT (HANDLE, tab_OWNER, tab_NAME, IND_NAME, 
COL_NAME, TYPE, PAGES, CARDINAL ) 
VALUES ( '103', 'DIESING', 'TAB1', 
'SQL_tab_STAT', 'SQL_tab_STAT', 0, 0, 0) !
declare SYS_INDEX cursor for
 select owner,tablename,indexname,type,
        columnname,sort
   from domain.indexcolumns
  where owner = 'KERN'
    and tablename = 'TAB1' 
   order by owner,tablename,indexname,columnno !
fetch SYS_INDEX into :a,:b,:c,:d,:e,:f !
INSERT SYS_ODBC_STAT (HANDLE, tab_OWNER, tab_NAME, NON_UNIQ, 
IND_NAME, TYPE, INDEXSEQ, COL_NAME, COLLATION, CARDINAL, 
PAGES ) 
select '103', 'DIESING', 'TAB1', 0, INDEXNAME, 1, 1, 'S1', 
substr(SORT,1,1), 0, 0 
FROM SYS_INDEX WHERE COLUMNNAME = 'S1' !
close SYS_INDEX !
select * from sys_odbc_stat !
fetch!
rollback !
create domain dom char (5) ascii check 
substr(dom, 1,1) between '0' AND '9' AND
substr(dom, 2,1) between '0' AND '9' AND
substr(dom, 3,1) between '0' AND '9' AND
substr(dom, 4,1) between '0' AND '9' AND
substr(dom, 5,1) between '0' AND '9'! 
create table friends ( name char (20) ascii,
domplz dom,
consplz  char(5) ascii
constraint cons_dom check 
substr(consplz, 1,1) between '0' AND '9' AND
substr(consplz, 2,1) between '0' AND '9' AND
substr(consplz, 3,1) between '0' AND '9' AND
substr(consplz, 4,1) between '0' AND '9' AND
substr(consplz, 5,1) between '0' AND '9', 
plz char (5) ascii,
constraint zip_dom check 
substr(plz, 1,1) between '0' AND '9' AND
substr(plz, 2,1) between '0' AND '9' AND
substr(plz, 3,1) between '0' AND '9' AND
substr(plz, 4,1) between '0' AND '9' AND
substr(plz, 5,1) between '0' AND '9')!
parsing !
minsert friends values (:a, :b, :c, :d) !
executing !
data !
minsert    t!
32767
1
1
c 20 'willi' c 5 '12345' c 5 '54321' c 5 '56789'
 
nodata !
pars_execute !
select * from friends !
fetch!
drop table friends !
commit release!

FILE connect (kern test SQLMODE ORACLE!

CREATE TABLE t (a VARCHAR2(5)  PRIMARY KEY     DISABLE)!
ALTER TABLE t ADD CONSTRAINT cn CHECK (a IN ('a','b','c'))!
file sel_constraint ( T !
file sel_checkdef ( USER T CN !
DROP TABLE t!

CREATE TABLE t (a VARCHAR2(5), PRIMARY KEY (a) DISABLE)!
DROP TABLE t!

CREATE TABLE t (a VARCHAR2(5))!
ALTER  TABLE t ADD PRIMARY KEY (a) DISABLE!
DROP TABLE t!

* Der folgende Fall funktioniert NICHT nach Oracle7 Sql-Handbuch,
* ist aber nach Aussage von Dirk ueberlebensnotwendig !
CREATE TABLE t (a VARCHAR2(5))!
ALTER  TABLE t ADD PRIMARY KEY (a) USING INDEX TABLESPACE 'x' DISABLE!
DROP TABLE t!
COMMIT WORK RELEASE!

file CONNECT ( kern test SQLMODE ORACLE!
CREATE TABLE t (a NUMBER, x VARCHAR2(64) DEFAULT USER)!
INSERT INTO t (a) VALUES (2)!
SELECT * FROM t!
FETCH INTO :A, :B!
DROP TABLE t!
COMMIT WORK RELEASE!

FILE connect (kern test!

CREATE TABLE t (a CHAR(5) KEY)!
ALTER TABLE t ADD CONSTRAINT cn CHECK (a IN ('a','b','c'))!
CREATE TABLE t2 (a CHAR(5) KEY)!
ALTER TABLE t2 ADD CONSTRAINT a CHECK (a IN ('a','b','c'))!
ALTER TABLE t2 ADD CONSTRAINT a CHECK (a IN ('a','b','c'))!
ALTER TABLE t2 ADD b CHAR(5) KEY!
DROP TABLE t!
DROP TABLE t2!
COMMIT WORK RELEASE!

FILE connect (kern test!
DROP TABLE tab1!
CREATE TABLE tab1 (
  col1 CHAR (5),
  col2 CHAR (5),
  CONSTRAINT             col1 <> 'Hallo',
  CONSTRAINT cons1 CHECK col2 = 'Wieso'
)!
file sel_constraint ( TAB1 !
DROP TABLE tab1!
CREATE TABLE tab1 (
  col1 CHAR (5),
  col2 CHAR (5),
  col3 CHAR (5),
  cg   CHAR (1) CONSTRAINT  cg  CHECK  cg  IN ('a','b'),
  "ch" CHAR (1) CONSTRAINT "ch" CHECK "ch" IN ('a','b'),
  CONSTRAINT cons1 CHECK col2 = 'Wieso',
  CONSTRAINT             col1 <> 'Hallo'
)!
SELECT * FROM sysdd.constraints WHERE tablename = 'TAB1'!
FETCH!
DROP TABLE tab1!
*COMMIT WORK RELEASE!

