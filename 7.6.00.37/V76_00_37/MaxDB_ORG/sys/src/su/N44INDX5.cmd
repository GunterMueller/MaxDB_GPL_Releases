*   *ID* N44INDX5 DBCMD    changed on 1992-09-28-13.26.35 by ADMIN     *
file connect ( kern test !
create table t ( a char(8), b char(6) byte ) !
insert t set a = 'aaa', b = x'000000000000' !
create index i1 on t(a,b) !
declare erg cursor for 
select * from t where a = 'aaa' for reuse !
fetch erg !
rollback!
create table t (
k1  char (10) ,
k2  char (10) ) !
insert t values ('cc', 'dd') !
insert t values ('dd', 'dd') !
create index "t.k1"
	on t( k1 ) !
create index "t.k2"
	on t( k2 ) !
select * from t where
k1 between 'cc' and 'dd' and k2 = 'dd' !
fetch!
select count(*) from t where
k1 between 'cc' and 'dd' and k2 = 'dd' !
fetch!
select * from t where
k1 between 'dd' and 'cc' and k2 = 'dd' !
select count(*) from t where
k1 between 'dd' and 'cc' and k2 = 'dd' !
fetch!
rollback work release !
