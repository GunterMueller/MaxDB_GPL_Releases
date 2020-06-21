*   *ID* N71JOIN7 DBCMD    changed on 1992-11-11-17.02.09 by KERN      *
file connect ( kern test !
* Fehlerprotokoll 3.0-55; Sequent-Test, Frank Hoffmann !
create table t ( a char (8), b char (8))!
insert t set a = 'aaa', b = 'bbb'!
insert t set a = 'bbb', b = 'bbb'!
insert t set a = 'ccc', b = 'bbb'!
create index "t.a"
	on t( a ) !
create table t1 (a1 char(8), b1 char(8))!
insert t1 set a1 = 'aaa', b1 = 'f'!
insert t1 set a1 = 'bbb', b1 = 'f'!
insert t1 set a1 = 'ccc', b1 = 'f'!
create index "t1.b1"
	on t1( b1 ) !
explain select * from t,t1 where a1 = a and b1 = 'f'!
commit !
pars_then_ex !
data !
select * from t,t1 where a1 = a and b1 = :b!
c 8 'f'
 
nodata !
pars_execute !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
drop table t!
drop table t1 !
commit release !
