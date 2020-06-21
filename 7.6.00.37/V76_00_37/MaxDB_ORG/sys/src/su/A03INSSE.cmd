*   *ID* A03INSSE DBCMD    changed on 1992-09-28-13.22.20 by ADMIN     *
file connect ( kern test !
create table tab1 ( spalte1 fixed(3),
                    spalte2 fixed(5,2),
                    spalte3 float(3) ) !
create table tab2 ( col1 fixed(4,1),
                    col2 fixed(6,3),
                    col3 float(4) ) !
insert tab1 set spalte1 = 7.2 !
insert tab1 set spalte2 = 7.213 !
insert tab1 set spalte3 = 1.234 !
insert tab2 values ( 123.4, 12.345, 1234 ) !
insert tab1 ( spalte1 ) select col1 from tab2 !
insert tab1 ( spalte2 ) select col2 from tab2 !
insert tab1 ( spalte3 ) select col3 from tab2 !
select * from tab1 !
fetch !
rollback work !
create table tab1 ( spalte1 fixed(5) key ) !
create table tab2 ( spalte1 fixed(5) not null ) !
create table tab3 ( spalte1 fixed(5) range between 2 and 2000 ) !
create table tab4 ( spalte1 fixed(5) range in ( 10, 100 )) !
create table tab5 ( col1 fixed(5) ) !
insert tab5 values ( 10 ) !
insert tab5 values ( null ) !
insert tab5 values ( 1 ) !
insert tab1 select * from tab5 !
select * from tab1 !
fetch !
insert tab2 select * from tab5 !
select * from tab2 !
insert tab3 select * from tab5 !
select * from tab3 !
insert tab4 select * from tab5 !
select * from tab4 !
rollback !
* K245 !
CREATE TABLE T ( A CHAR (4) )!
INSERT T SET A = 'aa'!
INSERT T SET A = 'ff'!
INSERT T SET A = 'fe'!
INSERT T SET A = 'qq'!
CREATE TABLE T1 LIKE T !
INSERT T1 SELECT * FROM T WHERE ROWNO < 3 !
select * from t1 !
fetch !
delete t1 !
* Fehlerprotokoll 124 !
create table t2 (a2 char (6))!
insert t2 set a2 = 'w'!
insert t1 select t.a from t, t2 where rowno < 3 !
select * from t1 !
fetch !
rollback !
* Fehlerprotokoll 103 !
create domain charrange char (12) range between 'aaa' and 'ccc'!
create table t (a charrange not null)!
insert t set a = null !
create table t1 (a1 char (12))!
insert t1 set a1 = null !
insert t select * from t1 !
select * from t !
fetch!
rollback !
* Fehlerprotokoll vom 3.5.90 !
create table t ( a char (8) key, b char (28))!
create table t1 (a1 char (8) key)!
insert t1 set a1 = 'abcde' !
insert t (a) select a1 from t1 !
select * from t !
fetch !
rollback work release !
* Fehlerprotokoll # 118 vom 18.6.90 !
* insert select laesst viel zu viel 'durch'
file connect ( kern test !
create table t ( a char (8) ascii) !
insert t set a = '19610725' !
insert t set a = '18.6.90' !
create table t1 ( a1 date )!
insert t1 select a from t where index (a, '.') > 0 !
insert t1 select a from t where index (a, '.') = 0 !
select * from t1 !
fetch !
rollback release !
* Fehlerprotokoll 250 !
file connect ( kern test !
create table t ( a char (18),
b char (8), c char (8), d char (8),
e fixed (3), f fixed (3), g fixed (3))!
create unique index "t.b"
	on t( b ) !
create unique index "t.c"
	on t( c ) !
create unique index "t.e"
	on t( e ) !
insert t values ('abcdefgh', 'bbbbbbbb', 'cccccccc', 'dddddddd',
123, 456, 789)!
create view v as select a, e, f, g  from t !
commit !
declare erg1 cursor for 
select '12345678' a, e+100 e, f+100 f, g+100 g
   from v where a = 'abcdefgh' for reuse!
commit !
rollback !
insert v (a, e, f, g) select * from erg1 !
select * from t !
fetch!
select * from t order by b !
fetch !
rollback !
insert t (a, e, f, g) select * from erg1 !
select * from t !
fetch!
rollback !
insert v select
             '12345678', f, g, e from v !
select * from t !
fetch!
rollback !
insert t (a, e, f, g) select
             '12345678', f, g, e from v where a = 'abcdefgh'!
select * from t !
fetch!
rollback !
drop table t !
commit !
CREATE TABLE N ( A INTEGER key, B INTEGER key, C CHAR (2) key)!
CREATE TABLE N1 LIKE N !
INSERT N1 VALUES (12345, 345678, 'aa')!
INSERT N SELECT * FROM N1!
CREATE TABLE N2 ( A INTEGER key, B CHAR (2) key, C INTEGER key)!
INSERT N2 (A,C,B) SELECT * FROM N1 !
rollback !
commit release !
