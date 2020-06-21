*   *ID* E04NULL  DBCMD    changed on 1992-09-28-13.23.46 by ADMIN     *
file connect ( kern test !
create table t (k char(20) key,mi1 char(20),mi2 char(20), mi3 char(20))!
insert into t set k = 'x1',mi1 = 'y'                     !
insert into t set k = 'x2',          mi2 = 'y'           !
insert into t set k = 'x3',                    mi3 = 'y' !
commit !
create index mia on t(mi1 asc,mi2 asc)!
create index "t.mi2"
	on t( mi2 ) !
create index mib on t(mi3)!
 ***** !
declare erg cursor for select * from t where mi1 =  'y'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi1 >= 'x'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi1 <= 'z'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi1 is null for reuse!
fetch erg!
declare erg cursor for 
select * from t where mi1 is null and mi2 >= 'x'  for reuse!
fetch erg!
declare erg cursor for 
select * from t where mi1 >= 'x'  and mi2 is null for reuse!
fetch erg!
 *****  !
declare erg cursor for select * from t where mi2 =  'y'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi2 >= 'x'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi2 <= 'z'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi2 is null for reuse!
fetch erg!
 *****  !
declare erg cursor for select * from t where mi3 =  'y'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi3 >= 'x'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi3 <= 'z'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi3 is null for reuse!
fetch erg!
 *****  !
rollback !
create index mia on t(mi1 desc,mi2 desc) !
create index "t.mi2"
	on t( mi2 desc ) !
create index mib on t(mi3 desc) !
 *****  !
declare erg cursor for select * from t where mi1 =  'y'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi1 >= 'x'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi1 <= 'z'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi1 is null for reuse!
fetch erg!
declare erg cursor for 
select * from t where mi1 is null and mi2 >= 'x'  for reuse!
fetch erg!
declare erg cursor for 
select * from t where mi1 >= 'x'  and mi2 is null for reuse!
fetch erg!
 *****  !
declare erg cursor for select * from t where mi2 =  'y'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi2 >= 'x'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi2 <= 'z'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi2 is null for reuse!
fetch erg!
 *****  !
declare erg cursor for select * from t where mi3 =  'y'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi3 >= 'x'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi3 <= 'z'  for reuse!
fetch erg!
declare erg cursor for select * from t where mi3 is null for reuse!
fetch erg!
 *****  !
drop table t !
commit release !
